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

#ifndef AUTOUPDATE_H_
#define AUTOUPDATE_H_

#include <QFile>
#include <QObject>
#include <QUrl>
#include <QVariant>

#include "tools/Ver.h"

class QBasicTimer;
class QCryptographicHash;
class QNetworkAccessManager;
class QNetworkReply;
class ISettings;

class UpdateInfo
{
public:
  inline UpdateInfo() : size(0) {}
  inline UpdateInfo(const QUrl &url) : size(0), url(url) {}
  UpdateInfo(const QVariantMap &data);
  bool isValid() const;

  int size;        ///< Размер обновлений.
  QByteArray hash; ///< SHA1 контрольная сумма обновлений.
  QUrl url;        ///< Адрес источинка обновлений или адрес загружаемого файла.
  Ver version;     ///< Доступная версия.
};


class AutoUpdate : public QObject
{
  Q_OBJECT

public:
  /// Состояние закачки.
  enum DownloadState {
    Idle,           ///< Нет активной закачки.
    DownloadJSON,   ///< Закачка JSON файла с информацией об обновлении.
    DownloadUpdate, ///< Закачка файла обновления.
    Paused          ///< Загрузка остановлена.
  };

  /// Статус проверки обновлений.
  enum Status {
    Unknown,         ///< Обновления не проверялись.
    CheckError,      ///< Не удалось получить информацию об обновлениях.
    NoUpdates,       ///< Нет доступных обновлений, установлена последняя версия.
    UpdateAvailable, ///< Доступно обновление.
    DownloadError,   ///< Ошибка при скачивании обновления.
    UpdateReady      ///< Обновление скачано и готово к применению.
  };

  static const QString kEnabled;
  static const QString kReady;
  static const QString kUrl;
  static const QString kVersion;

  AutoUpdate(ISettings *settings, QObject *parent);
  ~AutoUpdate();
  inline Status status() const { return m_status; }

signals:
  void done(int status);

public slots:
  void check();

protected:
  void timerEvent(QTimerEvent *event);

private slots:
  void download();
  void onFinished();
  void onReadyRead();
  void startDownload();

private:
  void checkUpdate();
  void readJSON();
  void setDone(Status status);

  DownloadState m_state;           ///< Состояние закачки.
  ISettings *m_settings;           ///< Настройки.
  QBasicTimer *m_timer;            ///< Таймер периодической проверки.
  QByteArray m_rawJSON;            ///< Сырые JSON данные.
  QCryptographicHash *m_sha1;      ///< Класс для проверки SHA1 хеша файла.
  QFile m_file;                    ///< Файл обновлений.
  qint64 m_lastCheck;              ///< Время последней успешной проверки обновлений.
  QNetworkAccessManager *m_net;    ///< Менеджер доступа к сети.
  QNetworkReply *m_current;        ///< Текущий ответ за запрос скачивания файла.
  Status m_status;                 ///< Статус проверки обновлений.
  UpdateInfo m_info;               ///< Информация об обновлении.
};

#endif /* AUTOUPDATE_H_ */
