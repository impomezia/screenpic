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
#include <QFont>
#include <QPen>

#include "ArrowItem.h"
#include "WidthCommand.h"
#include "TextItem.h"

WidthCommand::WidthCommand(const QList<QGraphicsItem *> items, const int &size, QUndoCommand *parent)
  : QUndoCommand(parent)
  , m_width(size)
  , m_item(0)
{
  foreach (QGraphicsItem *item, items) {
    m_originalWidth = getWidth(item);
    if (m_originalWidth > 0) {
      m_item = item;
      break;
    }
  }
}


bool WidthCommand::isValid() const
{
  return m_item;
}


int WidthCommand::getWidth(QGraphicsItem *item)
{
  if (item->type() == QGraphicsRectItem::Type || item->type() == QGraphicsEllipseItem::Type || item->type() == QGraphicsPathItem::Type)
    return qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(item)->pen().width();

  if (item->type() == QGraphicsLineItem::Type)
    return qgraphicsitem_cast<QGraphicsLineItem*>(item)->pen().width();

  if (item->type() == TextItem::Type)
    return qgraphicsitem_cast<TextItem*>(item)->font().pixelSize() - 13;

  if (item->type() == ArrowItem::Type)
    return qgraphicsitem_cast<ArrowItem*>(item)->pen().width();

  return int();
}


int WidthCommand::getWidth(const QList<QGraphicsItem *> items)
{
  int size;

  foreach (QGraphicsItem *item, items) {
    size = getWidth(item);
    if (size > 0)
      return size;
  }

  return size;
}


void WidthCommand::redo()
{
  apply(m_width);
}


void WidthCommand::undo()
{
  apply(m_originalWidth);
}


void WidthCommand::apply(int width)
{
  if (!m_item)
    return;

  if (m_item->type() == QGraphicsRectItem::Type || m_item->type() == QGraphicsEllipseItem::Type || m_item->type() == QGraphicsPathItem::Type) {
    QAbstractGraphicsShapeItem *item = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(m_item);

    QPen pen = item->pen();
    pen.setWidth(width);

    item->setPen(pen);
  }
  else if (m_item->type() == QGraphicsLineItem::Type) {
    QGraphicsLineItem *item = qgraphicsitem_cast<QGraphicsLineItem*>(m_item);

    QPen pen = item->pen();
    pen.setWidth(width);

    item->setPen(pen);
  }
  else if (m_item->type() == TextItem::Type) {
    TextItem *item = qgraphicsitem_cast<TextItem*>(m_item);
    QFont font = item->font();
    font.setPixelSize(width + 13);
    item->setFont(font);
  }
  else if (m_item->type() == ArrowItem::Type) {
    qgraphicsitem_cast<ArrowItem*>(m_item)->setWidth(width);
  }
}
