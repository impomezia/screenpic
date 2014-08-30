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
#include "items/PenItem.h"

PenItem::PenItem(QGraphicsItem *parent)
  : QGraphicsPathItem(parent)
{
  setFlag(ItemIsSelectable);
}


void PenItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  QStyleOptionGraphicsItem opt = *option;
  opt.state &= !QStyle::State_Selected;

  QGraphicsPathItem::paint(painter, &opt, widget);
}


void PenItem::start(const QPointF &point, const QPen &pen)
{
  grabMouse();
  QPen p = pen;
  p.setJoinStyle(Qt::RoundJoin);

  setPen(p);

  QPainterPath pp(point);
  setPath(pp);
}


void PenItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsPathItem::mouseMoveEvent(event);
  if (flags() & ItemIsMovable)
    return;

  QPainterPath p;

  if (event->modifiers() & Qt::SHIFT) {
    if (m_path.isEmpty())
      m_path = path();

    p = m_path;
    QLineF line(p.currentPosition(), event->scenePos());
    if (line.angle())
      line.setAngle((int) line.angle() / 45 * 45);

    p.lineTo(line.p2());
  }
  else {
    m_path = p;
    p = path();
    p.lineTo(event->scenePos());
  }

  setPath(p);
}


void PenItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsPathItem::mouseReleaseEvent(event);
  if (flags() & ItemIsMovable)
    return;

  ungrabMouse();
  setFlag(ItemIsMovable);

  scene()->clearSelection();
  setSelected(true);
}


QGraphicsItem *PenCreator::create(EditorScene *scene, const QPointF &point)
{
  PenItem *item = new PenItem();
  scene->addItem(item);
  item->start(point, scene->pen());

  return item;
}
