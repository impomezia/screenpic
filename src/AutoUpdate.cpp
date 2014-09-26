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

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>

#include "AutoUpdate.h"
#include "DateTime.h"
#include "JSON.h"
#include "NodeLog.h"
#include "Settings.h"
#include "sglobal.h"
#include "tools/OsInfo.h"
#include "version.h"

const QString AutoUpdate::kEnabled = LS("Update/Enabled");
const QString AutoUpdate::kReady   = LS("Update/Ready");
const QString AutoUpdate::kUrl     = LS("Update/Url");
const QString AutoUpdate::kVersion = LS("Update/Version");

#define TAG "AutoUpdate"

bool UpdateInfo::isValid() const
{
  return size && version.toUInt() != 0 && !url.isEmpty() && hash.size() == 20;
}


UpdateInfo::UpdateInfo(const QVariantMap &data)
  : size(0)
{
  if (data.isEmpty())
    return;

  version  = data.value(LS("version")).toString();
  url      = data.value(LS("url")).toUrl();
  size     = data.value(LS("size")).toInt();
  hash     = QByteArray::fromHex(data.value(LS("hash")).toByteArray());
}


AutoUpdate::AutoUpdate(ISettings *settings, QObject *parent)
  : QObject(parent)
  , m_state(Idle)
  , m_settings(settings)
  , m_lastCheck(0)
  , m_current(0)
  , m_status(Unknown)
{
  m_settings->setDefault(kEnabled, true);
  m_settings->setDefault(kUrl,     QUrl(LS(ORG_UPDATE_URL) + LS("/release.json")));

  m_net  = new QNetworkAccessManager(this);
  m_sha1 = new QCryptographicHash(QCryptographicHash::Sha1);

  m_timer = new QBasicTimer();
  m_timer->start(60 * 60 * 1000, this);

  QFile::remove(QString(LS("%1/update/%2-%3.exe")).arg(qApp->applicationDirPath()).arg(qApp->applicationName().toLower()).arg(qApp->applicationVersion()));
  QTimer::singleShot(0, this, SLOT(check()));
}


AutoUpdate::~AutoUpdate()
{
  if (m_timer->isActive())
    m_timer->stop();

  delete m_timer;
  delete m_sha1;
}


void AutoUpdate::check()
{
  if (m_state != Idle || !m_settings->value(kEnabled).toBool())
    return;

  m_state = DownloadJSON;
  m_rawJSON.clear();

  m_info = UpdateInfo(m_settings->value(kUrl).toString() + LC('?') + QString::number(QDateTime::currentDateTime().toTime_t()));
  if (!m_info.url.isValid())
    return setDone(CheckError);

  LOG_DEBUG("check for updates, url: " << m_info.url.toString());

  QTimer::singleShot(0, this, SLOT(startDownload()));
}


void AutoUpdate::timerEvent(QTimerEvent *event)
{
  if (event->timerId() == m_timer->timerId() && qAbs(DateTime::utc() - m_lastCheck) > (12 * 60 * 60 * 1000))
    check();

  QObject::timerEvent(event);
}


void AutoUpdate::download()
{
  m_state = DownloadUpdate;
  m_sha1->reset();

  QDir().mkpath(qApp->applicationDirPath() + LS("/update"));

  m_file.setFileName(QString(LS("%1/update/%2-%3.exe")).arg(qApp->applicationDirPath()).arg(qApp->applicationName().toLower()).arg(m_info.version.toString()));
  if (!m_file.open(QIODevice::WriteOnly))
    return setDone(DownloadError);

  startDownload();
}


void AutoUpdate::onFinished()
{
  if (!m_current->error()) {
    if (m_state == DownloadJSON)
      readJSON();
    else
      checkUpdate();
  }
  else
    setDone(m_state == DownloadJSON ? CheckError : DownloadError);

  m_current->deleteLater();
}


void AutoUpdate::onReadyRead()
{
  if (m_state == DownloadUpdate) {
    QByteArray data = m_current->readAll();
    m_sha1->addData(data);
    m_file.write(data);
  }
  else
    m_rawJSON.append(m_current->readAll());
}


void AutoUpdate::startDownload()
{
  QNetworkRequest request(m_info.url);
  request.setRawHeader("Referer",    m_info.url.toEncoded());
  request.setRawHeader("User-Agent", OsInfo::userAgent());

  const qint64 pos = m_file.pos();
  if (pos)
    request.setRawHeader("Range", "bytes=" + QByteArray::number(pos) + "-");

  m_current = m_net->get(request);
  connect(m_current, SIGNAL(finished()), SLOT(onFinished()));
  connect(m_current, SIGNAL(readyRead()), SLOT(onReadyRead()));
}


void AutoUpdate::checkUpdate()
{
  m_file.close();
  if (m_info.hash == m_sha1->result()) {
    m_settings->setValue(kVersion, m_info.version.toString());
    setDone(UpdateReady);
  }
  else
    setDone(DownloadError);
}


void AutoUpdate::readJSON()
{
  const QVariantMap data = JSON::parse(m_rawJSON).toMap();
  m_rawJSON.clear();
  if (data.isEmpty())
    return setDone(CheckError);

  m_info = UpdateInfo(data);
  if (!m_info.isValid())
    return setDone(CheckError);

  m_lastCheck = DateTime::utc();

  if (Ver(QCoreApplication::applicationVersion()) >= m_info.version)
    return setDone(NoUpdates);

  setDone(UpdateAvailable);

  QTimer::singleShot(0, this, SLOT(download()));
}


void AutoUpdate::setDone(Status status)
{
  LOG_DEBUG("done: " << status);

  m_status = status;
  m_state  = Idle;

  if (m_file.isOpen())
    m_file.close();

  m_settings->setValue(kReady, status == UpdateReady);
  emit done(status);
}
