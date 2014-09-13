/*   Copyright (C) 2013-2014 Alexander Sedov <imp@schat.me>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QThreadPool>
#include <QTimer>

#if QT_VERSION >= 0x050000
# include <QStandardPaths>
#endif

#include "AppCore.h"
#include "AutoUpdate.h"
#include "data/ImageItem.h"
#include "data/Thumbnail.h"
#include "DateTime.h"
#include "EditorWindow.h"
#include "GlobalShortcuts.h"
#include "Grabber.h"
#include "NodeLog.h"
#include "PluginManager.h"
#include "Providers.h"
#include "RecentModel.h"
#include "ServiceThread.h"
#include "Settings.h"
#include "sglobal.h"
#include "ShareHistoryDB.h"
#include "ShareNet.h"
#include "tasks/SaveTask.h"
#include "Translation.h"
#include "TrayIcon.h"
#include "uploaders/UploadResult.h"
#include "version.h"

#define TAG "AppCore"

AppCore::AppCore(QObject *parent)
  : QObject(parent)
  , m_autoUpdate(0)
  , m_netId(-1)
  , m_recentModel(0)
  , m_tray(0)
{
# ifndef NO_DEBUG_LOG
  m_log = new NodeLog();
  m_log->open(QApplication::applicationDirPath() + "/screenpic.log", NodeLog::TraceLevel);

  LOG_DEBUG("starting...")
# endif

# ifdef Q_OS_WIN
  m_settings = new Settings(QApplication::applicationDirPath() + "/screenpic.ini", this);
# else
  m_settings = new Settings(QApplication::organizationDomain(), QApplication::applicationName(), this);
# endif

  m_settings->setDefault(Settings::kCaptureMouse, true);
  m_settings->setDefault(Settings::kEditor,       true);
  m_settings->setDefault(Settings::kSaveCopy,     false);

# if QT_VERSION >= 0x050000
  const QString documents = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
# else
  const QString documents = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
# endif

  m_settings->setDefault(Settings::kLastOpenDir,  documents);
  m_settings->setDefault(Settings::kLastSaveDir,  documents);
  m_settings->setDefault(Settings::kSaveCopyIn,   QDir::fromNativeSeparators(documents + LC('/') + QApplication::applicationName()));

  m_pluginManager = new PluginManager(this);

  m_db = new ShareHistoryDB(this);
# ifdef Q_OS_MAC
  m_db->open(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/history.db");
# else
  m_db->open(QApplication::applicationDirPath() + "/history.db");
# endif

# ifndef NO_GLOBAL_SHORTCUTS
  m_shortcuts = new GlobalShortcuts(this, this);
# endif

  qRegisterMetaType<ChatId>("ChatId");
  qRegisterMetaType<UploadItemPtr>("UploadItemPtr");
  qRegisterMetaType<Thumbnail>("Thumbnail");

  initProviders();
  initTranslation();

  m_grabber = new Grabber(this);
  m_grabber->setCaptureMouse(m_settings->value(Settings::kCaptureMouse).toBool());
  m_service = new ServiceThread(this);
  m_netId = m_service->add(new ShareNetTask(m_providers));

  connect(m_settings, SIGNAL(changed(QString,QVariant)), SLOT(onSettingsChanged(QString,QVariant)));
  connect(m_service, SIGNAL(ready(qint64,QObject*)), SLOT(onTaskReady(qint64,QObject*)));
  connect(m_grabber, SIGNAL(ready(UploadItemPtr)), SLOT(add(UploadItemPtr)));

  m_service->start();

  LOG_DEBUG("started: " << m_providers->list().join(LS(", ")))
}


AppCore::~AppCore()
{
# ifndef NO_DEBUG_LOG
  delete m_log;
# endif
}


void AppCore::stop()
{
  LOG_DEBUG("stopping...")

  m_service->quit();
  m_service->wait();

  LOG_DEBUG("stopped")
}


void AppCore::onCustomRequest(const ChatId &id, const QString &provider, const QVariant &data)
{
  QMetaObject::invokeMethod(m_net, "add", Qt::QueuedConnection, Q_ARG(ChatId, id), Q_ARG(QString, provider), Q_ARG(QVariant, data));
}


void AppCore::add(QRunnable *task)
{
  m_tasks.append(task);
  if (m_tasks.size() == 1)
    QTimer::singleShot(0, this, SLOT(startTasks()));
}


void AppCore::add(UploadItemPtr item)
{
  LOG_DEBUG("added upload item: " << item->toString())

  if (item->type() == ImageItem::Type) {
    ImageItem *i = uploaditem_cast<ImageItem*>(item.data());
    if (!i)
      return;

    if (m_settings->value(Settings::kSaveCopy).toBool())
      i->saveAs = ImageItem::toSaveAs(m_settings->value(Settings::kSaveCopyIn).toString(), LS("png"), item->date());
  }

  if (!m_settings->value(Settings::kEditor).toBool())
    return onEditingFinished(item);

  EditorWindow *window = new EditorWindow(m_settings);
  connect(window, SIGNAL(taskCreated(QRunnable*)), SLOT(add(QRunnable*)));
  connect(window, SIGNAL(editingFinished(UploadItemPtr)), SLOT(onEditingFinished(UploadItemPtr)));

  window->open(item);
  window->show();
}


void AppCore::grabRect()
{
  LOG_DEBUG("grab rectangle...")
  m_grabber->grab(Grabber::Region);
}


void AppCore::grabScreen()
{
  LOG_DEBUG("grab fullscreen...")
  m_grabber->grab(Grabber::FullScreen);
}


void AppCore::openFile()
{
  openFile(QFileDialog::getOpenFileName(0, tr("Open image"), m_settings->value(Settings::kLastOpenDir).toString(), tr("Images (*.jpg *.jpeg *.png *.gif *.JPG *.PNG)")));
}


void AppCore::openFile(const QString &fileName)
{
  if (fileName.isEmpty())
    return;

  QFile file(fileName);
  if (!file.open(QFile::ReadOnly) || file.size() > (10 * 1024 * 1024))
    return;

  ImageItem *image = new ImageItem();
  image->raw       = file.readAll();

  if (!image->image.loadFromData(image->raw)) {
    delete image;
    return;
  }

  if (m_settings->value(Settings::kSaveCopy).toBool())
    image->saveAs = ImageItem::toSaveAs(m_settings->value(Settings::kSaveCopyIn).toString(), QFileInfo(fileName).suffix(), image->date());

  m_settings->setValue(Settings::kLastOpenDir, QFileInfo(fileName).absolutePath());
  onEditingFinished(UploadItemPtr(image));
}


void AppCore::onEditingFinished(UploadItemPtr item)
{
  LOG_DEBUG("editing finished: " << item->toString())

  if (item->type() == ImageItem::Type) {
    ImageItem *i = uploaditem_cast<ImageItem*>(item.data());
    if (!i)
      return;

    m_pending.insert(item->id(), item);

    SaveTask *task = new SaveTask(item->id(), i->image, i->saveAs, i->raw, QSize(32, 32));
    connect(task, SIGNAL(ready(ChatId,QByteArray,Thumbnail)), SLOT(onImageSaved(ChatId,QByteArray,Thumbnail)));
    add(task);
  }
}


void AppCore::onFinished(const ChatId &id, const QString &provider, const QVariant &data)
{
  IProvider *p = m_providers->get(provider);
  if (!p)
    return;

  p->handleReply(id, data);
}


/*!
 * Слот вызывается после того как изображение было сохранено в результирующий формат.
 */
void AppCore::onImageSaved(const ChatId &id, const QByteArray &body, const Thumbnail &thumbnail)
{
  if (!m_pending.contains(id))
    return;

  UploadItemPtr& item = m_pending[id];
  item->setThumbnail(thumbnail);

  if (item->type() == ImageItem::Type && m_service->isReady()) {
    uploaditem_cast<ImageItem*>(item.data())->raw = body;

    LOG_DEBUG("image converted: " << item->toString())

    IProvider *provider = m_providers->current();
    Q_ASSERT(provider);

    QMetaObject::invokeMethod(m_net, "add", Qt::QueuedConnection, Q_ARG(UploadItemPtr, item), Q_ARG(QString, provider->id()), Q_ARG(QVariant, provider->data()));

    m_recentModel->add(item);
  }
}


void AppCore::onSettingsChanged(const QString &key, const QVariant &value)
{
  if (key == Settings::kProvider)
    m_providers->setCurrentId(value.toString());
  else if (key == Settings::kCaptureMouse)
    m_grabber->setCaptureMouse(value.toBool());
}


void AppCore::onTaskReady(qint64 counter, QObject *object)
{
  if (counter == m_netId) {
    m_net = qobject_cast<ShareNet*>(object);
    if (!m_net)
      return;

    m_recentModel = new RecentModel(m_db, this);
    connect(m_recentModel, SIGNAL(taskCreated(QRunnable*)), SLOT(add(QRunnable*)));

    m_recentModel->init();
    m_autoUpdate = new AutoUpdate(m_settings, this);
    m_tray       = new TrayIcon(this);

    connect(m_net, SIGNAL(finished(UploadResult)), m_tray, SLOT(onUploadFinished(UploadResult)));
    connect(m_net, SIGNAL(finished(UploadResult)), SLOT(onUploadFinished(UploadResult)));
    connect(m_net, SIGNAL(finished(ChatId,QString,QVariant)), SLOT(onFinished(ChatId,QString,QVariant)));
    connect(m_net, SIGNAL(uploadProgress(ChatId,int)), SLOT(onUploadProgress(ChatId,int)));
  }
}


/*!
 * Слот вызывается при успешном сохранении картинки.
 */
void AppCore::onUploadFinished(const UploadResult &result)
{
  UploadItemPtr item = m_pending.take(result.id);
  if (!item)
    return;

  QString saveAs;
  if (item->type() == ImageItem::Type)
    saveAs = uploaditem_cast<ImageItem*>(item.data())->saveAs;

  m_db->add(result, item->date(), saveAs, item->thumbnail().raw);
  m_recentModel->finished(result);
}


void AppCore::onUploadProgress(const ChatId &id, int percent)
{
  m_recentModel->progress(id, percent);
}


void AppCore::startTasks()
{
  QThreadPool *pool = QThreadPool::globalInstance();

  while (!m_tasks.isEmpty())
    pool->start(m_tasks.takeFirst());
}


/*!
 * Инициализация провайдеров.
 */
void AppCore::initProviders()
{
  m_providers = new Providers(this);
  QObjectList providers = m_pluginManager->get("IProvider");
  for (int i = 0; i < providers.size(); ++i) {
    IProvider *provider = qobject_cast<IProvider*>(providers.at(i));

    provider->init(m_settings, this);
    m_providers->add(provider);
  }

  m_settings->setDefault(Settings::kProvider, LS(ORG_PROVIDER));
  m_providers->setCurrentId(m_settings->value(Settings::kProvider).toString());
}


/*!
 * Инициализация переводов.
 */
void AppCore::initTranslation()
{
  m_translation = new Translation(this);
  m_translation->setSearch(QStringList());
  m_translation->setPrefix(LS("app_"));

  m_settings->setDefault(Settings::kTranslation, LS("auto"));
  m_translation->load(m_settings->value(Settings::kTranslation).toString());
}
