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

#include <QLineEdit>
#include <QMenu>
#include <QPainter>
#include <QWidgetAction>

#include "ItemColorButton.h"
#include "ItemColorSelector.h"
#include "ItemWidthSelector.h"
#include "WebColorWidget.h"

ItemColorButton::ItemColorButton(QWidget *parent)
  : QToolButton(parent)
{
  m_selector      = new ItemColorSelector(this);
  m_webColor      = new WebColorWidget(this);
  m_widthSelector = new ItemWidthSelector(this);

  QMenu *menu = new QMenu(this);
  QWidgetAction *action = new QWidgetAction(this);
  action->setDefaultWidget(m_selector);
  menu->addAction(action);

  action = new QWidgetAction(this);
  action->setDefaultWidget(m_widthSelector);
  menu->addAction(action);

  action = new QWidgetAction(this);
  action->setDefaultWidget(m_webColor);
  menu->addAction(action);

  setMenu(menu);
  setPopupMode(InstantPopup);
  setColor(0xffd60808);

  connect(m_selector, SIGNAL(changed(QRgb)), SLOT(onChanged(QRgb)));
  connect(m_selector, SIGNAL(dropperClicked()), SIGNAL(dropperClicked()));
  connect(m_selector, SIGNAL(changed(QRgb)), m_webColor, SLOT(setRgb(QRgb)));
  connect(m_webColor, SIGNAL(changed(QColor)), SLOT(setColor(QColor)));
  connect(m_widthSelector, SIGNAL(changed(int)), SIGNAL(changed(int)));
}


QColor ItemColorButton::customColor() const
{
  return m_selector->customColor();
}


void ItemColorButton::setTempColor(const QColor &color)
{
  setIcon(pixmap(color));
}


void ItemColorButton::setWidth(int width)
{
  m_widthSelector->setWidth(width);
  m_widthSelector->setEnabled(width);
}


void ItemColorButton::setColor(const QColor &color)
{
  if (!color.isValid())
    return;

  setIcon(pixmap(color));

  const QRgb rgb = color.rgba();

  m_color = rgb;
  m_selector->setColor(rgb);
  m_widthSelector->setColor(rgb);
  m_webColor->setRgb(rgb);
}


void ItemColorButton::onChanged(QRgb color)
{
  if (m_color == color)
    return;

  setColor(color);

  emit changed(color);
}


QPixmap ItemColorButton::pixmap(const QColor &color) const
{
  QPen pen(color.darker(120));
  pen.setWidth(2);

  QPainter painter;
  QPixmap pix(24, 24);
  pix.fill(Qt::transparent);
  painter.begin(&pix);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setPen(pen);
  painter.setBrush(color);
  painter.drawEllipse(QPoint(12, 12), 8, 8);
  painter.end();

  return pix;
}
