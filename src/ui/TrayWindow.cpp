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
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QToolButton>
#include <QTranslator>
#include <QUrl>
#include <QVBoxLayout>

#include "AppCore.h"
#include "Providers.h"
#include "RecentView.h"
#include "Settings.h"
#include "SettingsDialog.h"
#include "sglobal.h"
#include "ShareDnD.h"
#include "Translation.h"
#include "TrayWindow.h"
#include "version.h"

TrayWindow::TrayWindow(AppCore *core, QWidget *parent)
  : QFrame(parent, Qt::Window | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint)
  , m_pin(false)
  , m_core(core)
  , m_width(0)
{
  setFocusPolicy(Qt::WheelFocus);
  setAcceptDrops(true);

  m_topFrame = new QFrame(this);
  m_topFrame->setObjectName("top_frame");

  m_recentLabel = new QLabel(this);

  m_recentView = new RecentView(this);

  m_pinBtn = new QToolButton(this);
  m_pinBtn->setObjectName("menu_btn");
  m_pinBtn->setFocusPolicy(Qt::NoFocus);
  m_pinBtn->setAutoRaise(true);
  m_pinBtn->setIcon(QIcon(":/images/pin.png"));
  m_pinBtn->setCheckable(true);

  createMenu();

  m_controlFrame = new QFrame(this);
  m_controlFrame->setObjectName("control_frame");

  addButton(QIcon(":/images/rect.png"),    RectangleBtn);
  addButton(QIcon(":/images/desktop.png"), FullscreenBtn);
  addButton(QIcon(":/images/open.png"),    FileBtn);

  m_linksFrame = new QFrame(this);
  m_linksFrame->setObjectName("links_frame");

  m_siteBtn = new QToolButton(this);
  m_siteBtn->setObjectName("site_btn");
  m_siteBtn->setText(ORG_DOMAIN);
  m_siteBtn->setFocusPolicy(Qt::NoFocus);
  m_siteBtn->setCursor(Qt::PointingHandCursor);
  m_siteBtn->setIcon(QIcon(":/images/globe.png"));
  m_siteBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

  retranslateUi();

  QHBoxLayout *topLay = new QHBoxLayout(m_topFrame);
  topLay->addWidget(m_recentLabel);
  topLay->addStretch();
  topLay->addWidget(m_pinBtn);
  topLay->addWidget(m_menuBtn);
  topLay->setContentsMargins(8, 6, 4, 6);
  topLay->setSpacing(0);

  QHBoxLayout *controlLay = new QHBoxLayout(m_controlFrame);
  controlLay->addWidget(m_buttons.value(RectangleBtn));
  controlLay->addWidget(m_buttons.value(FullscreenBtn));
  controlLay->addWidget(m_buttons.value(FileBtn));
  controlLay->setSpacing(0);

  QHBoxLayout *linksLay = new QHBoxLayout(m_linksFrame);
  linksLay->addStretch();
  linksLay->addWidget(m_siteBtn);
  linksLay->addStretch();

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(m_topFrame);
  layout->addWidget(m_recentView);
  layout->addWidget(m_controlFrame);
  layout->addWidget(m_linksFrame);
  layout->setMargin(0);
  layout->setSpacing(0);

  foreach (QToolButton *button, m_buttons) {
    connect(button, SIGNAL(clicked()), SLOT(close()));
  }

  connect(m_buttons.value(RectangleBtn), SIGNAL(clicked()), SIGNAL(grabRect()));
  connect(m_buttons.value(FullscreenBtn), SIGNAL(clicked()), SIGNAL(grabScreen()));
  connect(m_buttons.value(FileBtn), SIGNAL(clicked()), SIGNAL(openFile()));
  connect(m_siteBtn, SIGNAL(clicked()), SLOT(onSiteClicked()));
  connect(m_pinBtn, SIGNAL(clicked(bool)), SLOT(setPin(bool)));
}


void TrayWindow::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QFrame::changeEvent(event);
}


void TrayWindow::dragEnterEvent(QDragEnterEvent *event)
{
  IProvider *provider = m_core->providers()->current();

  if (provider && !ShareDnD::getFiles(event->mimeData(), provider->maxSize(), provider->maxImages()).isEmpty())
    event->acceptProposedAction();

  QFrame::dragEnterEvent(event);
}


void TrayWindow::dropEvent(QDropEvent *event)
{
  IProvider *provider = m_core->providers()->current();
  if (!provider)
    return;

  const QList<QUrl> urls = ShareDnD::getFiles(event->mimeData(), provider->maxSize(), provider->maxImages());
  if (urls.isEmpty())
    return;

  event->acceptProposedAction();
  m_core->openFile(urls.first().toLocalFile());
}


void TrayWindow::keyPressEvent(QKeyEvent *event)
{
  QFrame::keyPressEvent(event);

  if (event->key() == Qt::Key_Escape)
    close();

  if (event->matches(QKeySequence::Paste)) {
    IProvider *provider = m_core->providers()->current();
    if (!provider)
      return;

    const QMimeData *mimeData = QApplication::clipboard()->mimeData();
    const QList<QUrl> urls    = ShareDnD::getFiles(mimeData, provider->maxSize(), provider->maxImages());
    if (urls.isEmpty())
      return;

    m_core->openFile(urls.first().toLocalFile());
  }
}


void TrayWindow::showEvent(QShowEvent *event)
{
  QFrame::showEvent(event);

  if (m_width)
    return;

  foreach (QToolButton *button, m_buttons) {
    if (button->width() > m_width)
      m_width = button->width();
  }

  foreach (QToolButton *button, m_buttons) {
    button->setMinimumWidth(m_width);
  }
}


void TrayWindow::onLangChanged(QAction *action)
{
  const QString lang = action->data().toString();
  if (m_core->translation()->name().left(2) == lang)
    return;

  m_core->translation()->load(lang);
  m_core->settings()->setValue(LS("Translation"), lang);
}


void TrayWindow::onSiteClicked()
{
  QDesktopServices::openUrl(QUrl(LS(ORG_SITE)));

  close();
}


void TrayWindow::showSettings()
{
  if (!m_settingsDialog)
    m_settingsDialog = new SettingsDialog(m_core);

  m_settingsDialog->show();
}


QToolButton *TrayWindow::addButton(const QIcon &icon, Buttons type)
{
  QToolButton *btn = new QToolButton(this);
  btn->setObjectName("action_btn");
  btn->setFocusPolicy(Qt::NoFocus);
  btn->setAutoRaise(true);
  btn->setIconSize(QSize(32, 32));
  btn->setIcon(icon);
  btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  m_buttons.insert(type, btn);

  return btn;
}


void TrayWindow::addLang(const QString &lang)
{
  QTranslator translator;
  translator.load(":/translations/app_" + lang + ".qm");

  QAction *action = m_langMenu->addAction(QIcon(":/images/" + lang + ".png"), translator.translate("Translation", "English"));
  action->setCheckable(true);
  action->setData(lang);

  m_langs.insert(lang, action);
}


void TrayWindow::createMenu()
{
  m_menu = new QMenu(this);
  m_settingsAction = m_menu->addAction(QIcon(":/images/cog.png"), tr("Settings"), this, SLOT(showSettings()));

  m_langMenu = m_menu->addMenu(QIcon(":/images/language.png"), tr("Language"));
  addLang("en");
  addLang("ru");

  QActionGroup *group = new QActionGroup(this);
  group->setExclusive(true);
  foreach (QAction *action, m_langs) {
    group->addAction(action);
  }

  m_menu->addSeparator();

  m_exitAction = m_menu->addAction(QIcon(":/images/quit.png"), tr("Exit"), QApplication::instance(), SLOT(quit()));

  m_menuBtn = new QToolButton(this);
  m_menuBtn->setObjectName("menu_btn");
  m_menuBtn->setFocusPolicy(Qt::NoFocus);
  m_menuBtn->setAutoRaise(true);
  m_menuBtn->setIcon(QIcon(":/images/cog.png"));
  m_menuBtn->setMenu(m_menu);
  m_menuBtn->setPopupMode(QToolButton::InstantPopup);

  connect(m_langMenu, SIGNAL(triggered(QAction*)), SLOT(onLangChanged(QAction*)));
}


void TrayWindow::retranslateUi()
{
  m_width = 0;

  const QString lang = m_core->translation()->name().left(2);
  if (m_langs.contains(lang))
    m_langs.value(lang)->setChecked(true);

  m_recentLabel->setText(tr("Recent uploads"));
  m_settingsAction->setText(tr("Settings"));
  m_langMenu->setTitle(tr("Language"));
  m_exitAction->setText(tr("Exit"));

  m_buttons.value(RectangleBtn)->setText(tr("Rectangle"));
  m_buttons.value(RectangleBtn)->setToolTip(tr("Capture Rectangle"));

  m_buttons.value(FullscreenBtn)->setText(tr("Fullscreen"));
  m_buttons.value(FullscreenBtn)->setToolTip(tr("Capture Fullscreen"));

  m_buttons.value(FileBtn)->setText(tr("Open"));
  m_buttons.value(FileBtn)->setToolTip(tr("Open Image"));
}
