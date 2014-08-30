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

#ifndef TRAYICON_H_
#define TRAYICON_H_

#include <QSystemTrayIcon>
#include <QUrl>

class AppCore;
class TrayWindow;
class UploadResult;

class TrayIcon : public QSystemTrayIcon
{
  Q_OBJECT

public:
  enum TrayMessages {
    Unknown,
    UploadFinished,
    LinkCopied,
    UpdateReady
  };

  TrayIcon(AppCore *core);
  ~TrayIcon();
  bool eventFilter(QObject *watched, QEvent *event) override;
  void addMessage(TrayMessages message);

public slots:
  void onUploadFinished(const UploadResult &result);

private slots:
  void onActivated(QSystemTrayIcon::ActivationReason reason);
  void onLinkCopied(const QUrl &url);
  void onMessageClicked();
  void onUpdateDone(int status);

private:
  AppCore *m_core;
  QUrl m_link;
  QWidget *m_widget;
  TrayMessages m_message;
  TrayWindow *m_window;
};

#endif /* TRAYICON_H_ */
