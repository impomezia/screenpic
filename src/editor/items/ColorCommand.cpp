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

#include <QAbstractGraphicsShapeItem>
#include <QPen>

#include "ArrowItem.h"
#include "ColorCommand.h"
#include "TextItem.h"

ColorCommand::ColorCommand(const QList<QGraphicsItem *> items, const QColor &color, QUndoCommand *parent)
  : QUndoCommand(parent)
  , m_color(color)
  , m_item(0)
{
  foreach (QGraphicsItem *item, items) {
    m_original = getColor(item);
    if (m_original.isValid()) {
      m_item = item;
      break;
    }
  }
}


bool ColorCommand::isValid() const
{
  return m_item;
}


QColor ColorCommand::getColor(QGraphicsItem *item)
{
  if (item->type() == QGraphicsRectItem::Type || item->type() == QGraphicsEllipseItem::Type || item->type() == QGraphicsPathItem::Type)
    return qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(item)->pen().color();

  if (item->type() == QGraphicsLineItem::Type)
    return qgraphicsitem_cast<QGraphicsLineItem*>(item)->pen().color();

  if (item->type() == TextItem::Type)
    return qgraphicsitem_cast<TextItem*>(item)->defaultTextColor();

  if (item->type() == ArrowItem::Type)
    return qgraphicsitem_cast<ArrowItem*>(item)->pen().color();

  return QColor();
}


QColor ColorCommand::getColor(const QList<QGraphicsItem *> items)
{
  QColor color;

  foreach (QGraphicsItem *item, items) {
    color = getColor(item);
    if (color.isValid())
      return color;
  }

  return color;
}


void ColorCommand::redo()
{
  apply(m_color);
}


void ColorCommand::undo()
{
  apply(m_original);
}


void ColorCommand::apply(const QColor &color)
{
  if (!m_item)
    return;

  if (m_item->type() == QGraphicsRectItem::Type || m_item->type() == QGraphicsEllipseItem::Type || m_item->type() == QGraphicsPathItem::Type) {
    QAbstractGraphicsShapeItem *item = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(m_item);

    QPen pen = item->pen();
    pen.setColor(color);

    item->setPen(pen);
  }
  else if (m_item->type() == QGraphicsLineItem::Type) {
    QGraphicsLineItem *item = qgraphicsitem_cast<QGraphicsLineItem*>(m_item);

    QPen pen = item->pen();
    pen.setColor(color);

    item->setPen(pen);
  }
  else if (m_item->type() == TextItem::Type) {
    qgraphicsitem_cast<TextItem*>(m_item)->setDefaultTextColor(color);
  }
  else if (m_item->type() == ArrowItem::Type) {
    qgraphicsitem_cast<ArrowItem*>(m_item)->setColor(color);
  }
}
