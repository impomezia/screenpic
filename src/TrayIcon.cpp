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
#include <QClipboard>
#include <QDesktopServices>
#include <QThread>
#include <QTimer>

#include "App.h"
#include "AppCore.h"
#include "AutoUpdate.h"
#include "RecentModel.h"
#include "Settings.h"
#include "sglobal.h"
#include "TrayIcon.h"
#include "ui/Flyout.h"
#include "ui/RecentItemDelegate.h"
#include "ui/RecentView.h"
#include "ui/TrayWindow.h"
#include "uploaders/UploadResult.h"
#include "version.h"

TrayIcon::TrayIcon(AppCore *core)
  : QSystemTrayIcon(core)
  , m_core(core)
  , m_message(Unknown)
{
  setIcon(QIcon(QLatin1String(":/images/icon16.png")));
  setToolTip(QString("%1 %2").arg(PRODUCT_NAME).arg(VERSION_STRING));

  m_widget = new QWidget();
  RecentItemDelegate *delegate = new RecentItemDelegate(this);

  m_window = new TrayWindow(core, m_widget);
  m_window->installEventFilter(this);
  m_window->recentView()->setItemDelegate(delegate);
  m_window->recentView()->setModel(core->recentModel());

  QTimer::singleShot(0, this, SLOT(show()));
  QTimer::singleShot(1000, this, SLOT(show()));

  connect(this, SIGNAL(messageClicked()), SLOT(onMessageClicked()));
  connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(onActivated(QSystemTrayIcon::ActivationReason)));
  connect(m_window, SIGNAL(grabRect()), m_core, SLOT(grabRect()));
  connect(m_window, SIGNAL(grabScreen()), m_core, SLOT(grabScreen()));
  connect(m_window, SIGNAL(openFile()), m_core, SLOT(openFile()));
  connect(delegate, SIGNAL(closeRequest()), m_window, SLOT(close()));
  connect(delegate, SIGNAL(linkCopied(QUrl)), SLOT(onLinkCopied(QUrl)));
  connect(core->autoUpdate(), SIGNAL(done(int)), SLOT(onUpdateDone(int)));
}


TrayIcon::~TrayIcon()
{
  delete m_widget;
}


bool TrayIcon::eventFilter(QObject *watched, QEvent *event)
{
  if (watched == m_window && event->type() == QEvent::FocusOut) {
    if (m_window->isPin() || m_window->geometry().contains(QCursor::pos()) || geometry().contains(QCursor::pos()))
      return false;

    m_window->close();
  }

  return QSystemTrayIcon::eventFilter(watched, event);
}


void TrayIcon::addMessage(TrayMessages message)
{
  m_message = message;

  if (message == UploadFinished)
    showMessage(tr("%1 - Task completed").arg(PRODUCT_NAME), tr("Link %1 copied to the clipboard").arg(m_link.toString()), QSystemTrayIcon::Information, 30000);
  else if (message == LinkCopied)
    showMessage(PRODUCT_NAME, tr("Link %1 copied to the clipboard").arg(m_link.toString()), QSystemTrayIcon::Information);
  else if (message == UpdateReady)
    showMessage(tr("%1 - Update Available").arg(PRODUCT_NAME), tr("Update to version %1 is ready to install").arg(m_core->settings()->value(AutoUpdate::kVersion).toString()), QSystemTrayIcon::Information, 60000);
}


void TrayIcon::onUploadFinished(const UploadResult &result)
{
  if (result.status != 200 || result.images.isEmpty())
    return;

  const UploadResult::Image image = result.images.first();
  if (image.link.scheme() == LS("file"))
    return;

  m_link = image.link;
  QApplication::clipboard()->setText(m_link.toString());

  addMessage(UploadFinished);
}


void TrayIcon::onActivated(QSystemTrayIcon::ActivationReason reason)
{
# ifdef Q_OS_WIN
  if (reason == QSystemTrayIcon::Trigger) {
    m_core->grabRect();
    return;
  }
# else
  Q_UNUSED(reason);
# endif

  if (m_window->isVisible())
    m_window->close();
  else
    Flyout::show(m_window, geometry());
}


void TrayIcon::onLinkCopied(const QUrl &url)
{
  m_link = url;
  addMessage(LinkCopied);
}


void TrayIcon::onMessageClicked()
{
  if (m_message == UploadFinished)
    QDesktopServices::openUrl(m_link);
  else if (m_message == UpdateReady)
    static_cast<App*>(QCoreApplication::instance())->restart();
}


void TrayIcon::onUpdateDone(int status)
{
  if (status == AutoUpdate::UpdateReady)
    addMessage(UpdateReady);
}
