/*   Copyright (C) 2013-2015 Alexander Sedov <imp@schat.me>
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

#ifndef APPCORE_H_
#define APPCORE_H_

#include <QMap>
#include <QObject>

#include "data/UploadItem.h"
#include "interfaces/IProviderListener.h"
#include "interfaces/IScreenpic.h"
#include "interfaces/ISettingsListener.h"

class AutoUpdate;
class GlobalShortcuts;
class Grabber;
class IHook;
class NodeLog;
class Observers;
class PluginManager;
class Providers;
class QRunnable;
class RecentModel;
class ServiceThread;
class ShareHistoryDB;
class ShareNet;
class TrayIcon;
class UploadItem;
class UploadResult;
struct Thumbnail;

class AppCore : public QObject, public IProviderListener, public IScreenpic, public ISettingsListener
{
  Q_OBJECT

public:
  AppCore(QObject *parent = 0);
  ~AppCore();
  inline AutoUpdate *autoUpdate() const      { return m_autoUpdate; }
  inline GlobalShortcuts *shortcuts() const  { return m_shortcuts; }
  inline IHook *hooks() const                { return m_hooks; }
  inline Providers *providers() const        { return m_providers; }
  inline RecentModel *recentModel() const    { return m_recentModel; }
  ISettings *settings() const override       { return m_settings; }
  ITranslation *translation() const override { return m_translation; }
  QString edition() const override;
  void remove(const QString &deletehash, const QString &providerId);
  void stop();

  void onCustomRequest(const ChatId &id, const QString &provider, const QVariant &data) override;
  void onSettingsChanged(const QString &key, const QVariant &value) override;

public slots:
  void add(QRunnable *task);
  void add(UploadItemPtr item);
  void grabRect();
  void grabScreen();
  void grabWindow();
  void openFile();
  void openFile(const QString &fileName);

private slots:
  void onEditingFinished(UploadItemPtr item);
  void onImageSaved(const ChatId &id, const QByteArray &body, const Thumbnail &thumbnail);
  void onTaskReady(qint64 counter, QObject *object);
  void onUploadFinished(const UploadResult &result);
  void onUploadProgress(const ChatId &id, int percent);
  void startTasks();

private:
  void initProviders();
  void initTranslation();

  AutoUpdate *m_autoUpdate;              ///< Менеджер автоматического обновления.
  GlobalShortcuts *m_shortcuts;          ///< Менеджер глобальных горячих клавиш.
  Grabber *m_grabber;
  IHook *m_hooks;
  int m_netId;
  ISettings *m_settings;                 ///< Настройки.
  ITranslation *m_translation;           ///< Модуль загрузки переводов.
  NodeLog *m_log;
  Observers *m_observers;
  PluginManager *m_pluginManager;
  Providers *m_providers;                ///< Доступные провайдеры.
  QList<QRunnable*> m_tasks;             ///< Задачи для выполнения в отдельном потоке.
  QMap<ChatId, UploadItemPtr> m_pending;
  RecentModel *m_recentModel;            ///< Модель для показа посдедних файлов.
  ServiceThread *m_service;
  ShareHistoryDB *m_db;                  ///< База данных c историей.
  ShareNet *m_net;
  TrayIcon *m_tray;
};

#endif /* APPCORE_H_ */
