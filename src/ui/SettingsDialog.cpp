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
#include <QLabel>
#include <QStackedWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QTimer>

#include "AboutPage.h"
#include "HotkeysPage.h"
#include "Observers.h"
#include "ServersPage.h"
#include "SettingsDialog.h"
#include "SettingsPage.h"
#include "sglobal.h"

SettingsDialog::SettingsDialog(AppCore *core, QWidget *parent)
  : QDialog(parent)
  , m_width(0)
{
  setObjectName("SettingsDialog");
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);
  setWindowTitle("Settings");
  setMinimumWidth(400);

  QFrame *topFrame = new QFrame(this);
  topFrame->setObjectName(LS("TopFrame"));
  topFrame->setStyleSheet(LS("#TopFrame{background-color:#fff;}"));

  m_settingsBtn = addBtn(QIcon(LS(":/images/cog32.png")), tr("Settings"));
  m_settingsBtn->setChecked(true);

# ifndef NO_GLOBAL_SHORTCUTS
  m_hotkeysBtn = addBtn(QIcon(LS(":/images/hotkeys.png")), tr("Hotkeys"));
# endif

  m_serversBtn = addBtn(QIcon(LS(":/images/servers.png")), tr("Servers"));
  m_aboutBtn = addBtn(QIcon(LS(":/images/icon32.png")), tr("About"));

  m_pages = new QStackedWidget(this);
  m_pages->addWidget(m_settingsPage = new SettingsPage(core, this));

# ifndef NO_GLOBAL_SHORTCUTS
  m_pages->addWidget(m_hotkeysPage = new HotkeysPage(core, this));
# endif

  m_pages->addWidget(m_serversPage = new ServersPage(core, this));
  m_pages->addWidget(m_aboutPage = new AboutPage(core, this));

  QFrame *line = new QFrame(this);
  line->setFrameStyle(QFrame::HLine | QFrame::Sunken);

  QHBoxLayout *btnLay = new QHBoxLayout(topFrame);
  btnLay->addWidget(m_settingsBtn);
# ifndef NO_GLOBAL_SHORTCUTS
  btnLay->addWidget(m_hotkeysBtn);
# endif
  btnLay->addWidget(m_serversBtn);
  btnLay->addStretch();
  btnLay->addWidget(m_aboutBtn, 0, Qt::AlignTop);
  btnLay->setMargin(4);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(topFrame);
  layout->addWidget(line);
  layout->addWidget(m_pages);
  layout->setMargin(0);
  layout->setSpacing(0);

  m_settingsPage->setMinimumWidth(minimumWidth());

  Observers::watch(this);

  for (int i = 0; i < m_pages->count(); ++i) {
    Observers::watch(m_pages->widget(i));
  }

  connect(m_settingsPage, SIGNAL(adjustSizeRequest()), SLOT(onAdjustSizeRequest()));
  connect(m_serversPage, SIGNAL(adjustSizeRequest()), SLOT(onAdjustSizeRequest()));
}


void SettingsDialog::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QDialog::changeEvent(event);
}


void SettingsDialog::showEvent(QShowEvent *event)
{
  QDialog::showEvent(event);

  if (m_width)
    return;

  foreach (QToolButton *button, m_buttons) {
    if (button->width() > m_width)
      m_width = button->width();
  }

  if (m_width < 64)
    m_width = 64;

  foreach (QToolButton *button, m_buttons) {
    button->setMinimumWidth(m_width);
  }
}


void SettingsDialog::onAdjustSizeRequest()
{
  QTimer::singleShot(0, this, SLOT(adjustSize()));
}


void SettingsDialog::tooglePage()
{
  QToolButton *button = qobject_cast<QToolButton*>(sender());

  m_settingsBtn->setChecked(button == m_settingsBtn);
# ifndef NO_GLOBAL_SHORTCUTS
  m_hotkeysBtn->setChecked(button == m_hotkeysBtn);
# endif
  m_serversBtn->setChecked(button == m_serversBtn);
  m_aboutBtn->setChecked(button == m_aboutBtn);

  if (button == m_settingsBtn)
    m_pages->setCurrentWidget(m_settingsPage);

# ifndef NO_GLOBAL_SHORTCUTS
  if (button == m_hotkeysBtn)
    m_pages->setCurrentWidget(m_hotkeysPage);
# endif

  if (button == m_serversBtn)
    m_pages->setCurrentWidget(m_serversPage);

  if (button == m_aboutBtn)
    m_pages->setCurrentWidget(m_aboutPage);

  for (int i = 0; i < m_pages->count(); ++i) {
      QSizePolicy::Policy policy = QSizePolicy::Ignored;
      if (i == m_pages->currentIndex())
          policy = QSizePolicy::Expanding;

      QWidget *page = m_pages->widget(i);
      page->setSizePolicy(policy, policy);
  }

  onAdjustSizeRequest();
}


QToolButton *SettingsDialog::addBtn(const QIcon &icon, const QString &text)
{
  QToolButton *button = new QToolButton(this);
  button->setObjectName("settings_btn");
  button->setIcon(icon);
  button->setText(text);
  button->setAutoRaise(true);
  button->setCheckable(true);
  button->setIconSize(QSize(32, 32));
  button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

  m_buttons.append(button);

  connect(button, SIGNAL(clicked()), SLOT(tooglePage()));

  return button;
}


void SettingsDialog::retranslateUi()
{
  m_settingsBtn->setText(tr("Settings"));
  m_serversBtn->setText(tr("Servers"));
  m_aboutBtn->setText(tr("About"));

# ifndef NO_GLOBAL_SHORTCUTS
  m_hotkeysBtn->setText(tr("Hotkeys"));
# endif
}
