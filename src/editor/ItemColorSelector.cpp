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

#include <QEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QMenu>
#include <QPainter>
#include <QToolButton>
#include <QVariant>

#include "editor/ItemColorSelector.h"
#include "sglobal.h"

ItemColorSelector::ItemColorSelector(QWidget *parent)
  : QToolBar(parent)
{
  QWidget *colors = new QWidget(this);

  QVBoxLayout *layout = new QVBoxLayout(colors);
  QHBoxLayout *line1  = new QHBoxLayout;
  QHBoxLayout *line2  = new QHBoxLayout;
  line1->setSpacing(2);
  line2->setSpacing(2);

  layout->addLayout(line1);
  layout->addLayout(line2);
  layout->setMargin(4);
  layout->setSpacing(0);

  add(0xffd60808, line1);
  add(0xffff7800, line1);
  add(0xfff3e00e, line1);
  add(0xff000000, line1);
  add(0xff1b7100, line2);
  add(0xff2343e8, line2);
  add(0xffdc54b7, line2);
  add(0xffffffff, line2);

  addWidget(colors);

  QWidget *dropper = new QWidget(this);
  m_dropperLayout = new QVBoxLayout(dropper);
  m_dropperLayout->setSpacing(0);

  m_customColorBtn = add(QColor(), m_dropperLayout);

  m_dropperBtn = new QToolButton(this);
  m_dropperBtn->setIcon(QIcon(LS(":/images/dropper.png")));
  m_dropperBtn->setAutoRaise(true);
  m_dropperBtn->setToolTip(tr("Dropper"));

  connect(m_dropperBtn, SIGNAL(clicked()), SLOT(onDropperClicked()));

  m_dropperLayout->addWidget(m_dropperBtn);


  addSeparator();
  addWidget(dropper);

  reload();
}


bool ItemColorSelector::setColor(const QColor &color)
{
  QToolButton *button = m_colors.value(color.rgba());
  foreach (QToolButton *btn, m_colors) {
    if (!button || button != btn)
      btn->setChecked(false);
  }

  if (button) {
    m_customColorBtn->setChecked(false);
    button->setChecked(true);
  }
  else {
    m_customColorBtn->setChecked(true);
    m_customColor = color;
  }

  reload();

  return true;
}


QColor ItemColorSelector::customColor() const
{
  if (m_customColorBtn->isChecked() && m_customColor.isValid())
    return m_customColor;

  return QColor();
}


void ItemColorSelector::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QToolBar::changeEvent(event);
}


void ItemColorSelector::onClicked(bool checked)
{
  QToolButton *button = qobject_cast<QToolButton*>(sender());
  if (!checked || !button)
    return;

  const QRgb color = button->property("color").toUInt();
  if (!setColor(color))
    return;

  if (qobject_cast<QMenu*>(parentWidget()))
    parentWidget()->close();

  emit changed(color);
}


void ItemColorSelector::onDropperClicked()
{
  if (qobject_cast<QMenu*>(parentWidget()))
    parentWidget()->close();

  emit dropperClicked();
}


QPixmap ItemColorSelector::pixmap(const QColor &color) const
{
  QPainter painter;
  QPixmap pix(16, 16);
  pix.fill(Qt::transparent);
  painter.begin(&pix);
  painter.setPen(color.darker(130));
  painter.setBrush(color);
  painter.drawRect(0, 0, 14, 14);
  painter.end();

  return pix;
}


QToolButton *ItemColorSelector::add(const QColor &color, QBoxLayout *layout)
{
  QToolButton *button = new QToolButton(this);
  button->setProperty("color", color.rgba());
  button->setIconSize(QSize(16, 16));
  button->setAutoRaise(true);
  button->setCheckable(true);
  button->setIcon(pixmap(color));

  layout->addWidget(button);

  if (color.isValid()) {
    m_colors.insert(color.rgba(), button);
    button->setToolTip(color.name());
  }

  connect(button, SIGNAL(clicked(bool)), SLOT(onClicked(bool)));

  return button;
}


void ItemColorSelector::reload()
{
  if (m_customColor.isValid()) {
    m_dropperLayout->setMargin(4);
    m_dropperBtn->setIconSize(QSize(16, 16));

    m_customColorBtn->setToolTip(m_customColor.name());
    m_customColorBtn->setIcon(pixmap(m_customColor));
    m_customColorBtn->setProperty("color", m_customColor.rgba());
    m_customColorBtn->show();
  }
  else {
    m_customColorBtn->setChecked(false);
    m_customColorBtn->hide();

    m_dropperLayout->setMargin(0);
    m_dropperBtn->setIconSize(QSize(24, 24));
  }
}


void ItemColorSelector::retranslateUi()
{
  m_dropperBtn->setToolTip(tr("Dropper"));
}
