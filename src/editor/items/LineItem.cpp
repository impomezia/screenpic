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

#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>

#include "EditorScene.h"
#include "items/LineItem.h"

LineItem::LineItem(QGraphicsItem *parent)
  : QGraphicsLineItem(parent)
{
  setFlag(ItemIsSelectable);
}


void LineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  QStyleOptionGraphicsItem opt = *option;
  opt.state &= !QStyle::State_Selected;

  QGraphicsLineItem::paint(painter, &opt, widget);
}


void LineItem::start(const QPointF &point, const QPen &pen)
{
  grabMouse();

  m_point = point;

  setPen(pen);
}


void LineItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsLineItem::mouseMoveEvent(event);
  if (flags() & ItemIsMovable)
    return;

  const QPointF pos = event->scenePos();

  QLineF line(m_point, pos);
  if (event->modifiers() & Qt::SHIFT && line.angle())
    line.setAngle((int) line.angle() / 45 * 45);

  setLine(line);
}


void LineItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsLineItem::mouseReleaseEvent(event);

  if (flags() & ItemIsMovable)
    return;

  ungrabMouse();
  setFlag(QGraphicsItem::ItemIsMovable);

  scene()->clearSelection();
  setSelected(true);
}


QGraphicsItem *LineCreator::create(EditorScene *scene, const QPointF &point)
{
  LineItem *item = new LineItem();
  scene->addItem(item);
  item->start(point, scene->pen());

  return item;
}
