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

#ifndef APPCORE_H_
#define APPCORE_H_

#include <QMap>
#include <QObject>

#include "data/UploadItem.h"

class AutoUpdate;
class GlobalShortcuts;
class Grabber;
class NodeLog;
class PluginManager;
class Providers;
class QRunnable;
class RecentModel;
class ServiceThread;
class Settings;
class ShareHistoryDB;
class ShareNet;
class Translation;
class TrayIcon;
class UploadItem;
class UploadResult;
struct Thumbnail;

class AppCore : public QObject
{
  Q_OBJECT

public:
  AppCore(QObject *parent = 0);
  ~AppCore();
  void stop();
  inline AutoUpdate *autoUpdate() const     { return m_autoUpdate; }
  inline GlobalShortcuts *shortcuts() const { return m_shortcuts; }
  inline Providers *providers() const       { return m_providers; }
  inline RecentModel *recentModel() const   { return m_recentModel; }
  inline Settings *settings() const         { return m_settings; }
  inline Translation *translation() const   { return m_translation; }

public slots:
  void add(QRunnable *task);
  void add(UploadItemPtr item);
  void customRequest(const ChatId &id, const QVariant &data);
  void grabRect();
  void grabScreen();
  void openFile();
  void openFile(const QString &fileName);

private slots:
  void onEditingFinished(UploadItemPtr item);
  void onImageSaved(const ChatId &id, const QByteArray &body, const Thumbnail &thumbnail);
  void onSettingsChanged(const QString &key, const QVariant &value);
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
  int m_netId;
  NodeLog *m_log;
  PluginManager *m_pluginManager;
  Providers *m_providers;                ///< Доступные провайдеры.
  QList<QRunnable*> m_tasks;             ///< Задачи для выполнения в отдельном потоке.
  QMap<ChatId, UploadItemPtr> m_pending;
  RecentModel *m_recentModel;            ///< Модель для показа посдедних файлов.
  ServiceThread *m_service;
  Settings *m_settings;                  ///< Настройки.
  ShareHistoryDB *m_db;                  ///< База данных c историей.
  ShareNet *m_net;
  Translation *m_translation;            ///< Модуль загрузки переводов.
  TrayIcon *m_tray;
};

#endif /* APPCORE_H_ */
