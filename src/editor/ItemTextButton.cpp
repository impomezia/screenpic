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

#include <QHBoxLayout>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>
#include <QWidgetAction>

#include "interfaces/IScreenpic.h"
#include "ItemTextButton.h"
#include "Settings.h"

ItemTextButton::ItemTextButton(IScreenpic *screenpic, QWidget *parent)
  : ToolBarItem(parent)
  , m_screenpic(screenpic)
{
  m_screenpic->settings()->addListener(this);

  m_button = new QToolButton(this);
  m_button->setToolTip(tr("Text (T)"));
  m_button->setAutoRaise(true);
  m_button->setIconSize(QSize(24, 24));
  m_button->setCheckable(true);
  m_button->setPopupMode(QToolButton::MenuButtonPopup);
  m_button->setMenu(new QMenu(this));

  QToolBar *toolBar = new QToolBar(this);
  toolBar->setIconSize(QSize(24, 24));

  m_borderlessAction = toolBar->addAction(QIcon(":/images/text.png"), tr("Borderless text"), this, SLOT(apply()));
  m_borderlessAction->setCheckable(true);

  m_borderAction = toolBar->addAction(QIcon(":/images/text-border.png"), tr("Text with border"), this, SLOT(apply()));
  m_borderAction->setCheckable(true);

  QActionGroup *group = new QActionGroup(this);
  group->setExclusive(true);
  group->addAction(m_borderAction);
  group->addAction(m_borderlessAction);

  QWidgetAction *widgetAction = new QWidgetAction(this);
  widgetAction->setDefaultWidget(toolBar);

  m_button->menu()->addAction(widgetAction);

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->addWidget(m_button);
  layout->setMargin(0);

  reload(m_screenpic->settings()->value(Settings::kTextBorder).toBool());
  connect(m_button, SIGNAL(toggled(bool)), SIGNAL(toggled(bool)));
}


ItemTextButton::~ItemTextButton()
{
  m_screenpic->settings()->removeListener(this);
}


void ItemTextButton::onSettingsChanged(const QString &key, const QVariant &value)
{
  if (key == Settings::kTextBorder) {
    m_borderAction->setChecked(value.toBool());
    reload(value.toBool());
  }
}


void ItemTextButton::setChecked(bool checked)
{
  m_button->setChecked(checked);
}


void ItemTextButton::contextMenuEvent(QContextMenuEvent *event)
{
  Q_UNUSED(event)

  m_button->showMenu();
}


void ItemTextButton::retranslateUi()
{
  m_button->setToolTip(tr("Text (T)"));
  m_borderAction->setText(tr("Text with border"));
  m_borderlessAction->setText(tr("Borderless text"));
}


void ItemTextButton::apply()
{
  setChecked(true);
  m_button->menu()->close();

  m_screenpic->settings()->setValue(Settings::kTextBorder, m_borderAction->isChecked());
}


void ItemTextButton::reload(bool border)
{
  if (border) {
    m_button->setIcon(QIcon(":/images/text-border.png"));
    m_borderAction->setChecked(true);
  }
  else {
    m_button->setIcon(QIcon(":/images/text.png"));
    m_borderlessAction->setChecked(true);
  }
}
