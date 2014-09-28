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
#include <qmath.h>
#include <QStyleOptionGraphicsItem>

#include "EditorScene.h"
#include "items/ArrowItem.h"

#if !defined(M_PI)
# define M_PI 3.14159265358979323846
#endif

ArrowItem::ArrowItem(QGraphicsItem *parent)
  : QGraphicsLineItem(parent)
  , m_arrowHead(0)
{
  setFlag(ItemIsSelectable);
}


void ArrowItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  QStyleOptionGraphicsItem opt = *option;
  opt.state &= !QStyle::State_Selected;

  QGraphicsLineItem::paint(painter, &opt, widget);
}


void ArrowItem::setColor(const QColor &color)
{
  QPen pen = this->pen();
  pen.setColor(color);

  setPen(pen);

  pen = m_arrowHead->pen();
  pen.setColor(color);

  m_arrowHead->setPen(pen);
  m_arrowHead->setBrush(color);
}

void ArrowItem::setWidth(const int &width)
{
  QPen pen = this->pen();
  pen.setWidth(width);

  setPen(pen);

  pen = m_arrowHead->pen();
  pen.setWidth(width > 3 ? width : 1);

  m_arrowHead->setPen(pen);
}


void ArrowItem::start(const QPointF &point, const QPen &pen)
{
  grabMouse();

  m_point = point;

  setPen(pen);
}


void ArrowItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsLineItem::mouseMoveEvent(event);

  if (flags() & ItemIsMovable)
    return;

  QPointF pos = event->scenePos();

  if (event->modifiers() & Qt::SHIFT) {
    QLineF line(m_point, pos);
    if (line.angle())
      line.setAngle((int) line.angle() / 45 * 45);

    pos = line.p2();
  }

  arrowHead(pos);

  setLine(QLineF(m_point, mapFromItem(m_arrowHead, m_arrowHead->path().elementAt(3))));
}


void ArrowItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsLineItem::mouseReleaseEvent(event);

  if (flags() & ItemIsMovable)
    return;

  ungrabMouse();
  setFlag(ItemIsMovable);

  scene()->clearSelection();
  setSelected(true);
}


void ArrowItem::arrowHead(const QPointF &point)
{
  if (m_arrowHead) {
    scene()->removeItem(m_arrowHead);
    delete m_arrowHead;
  }

  QPolygonF polygon(4);
  polygon[0] = QPointF(point.x() - 15, point.y() + 5);
  polygon[1] = point;
  polygon[2] = QPointF(point.x() - 15, point.y() - 5);
  polygon[3] = QPointF(point.x() - 10, point.y());

  QPen pen(this->pen().color());
  pen.setJoinStyle(Qt::MiterJoin);

  const int width = this->pen().width();
  if (width > 3)
    pen.setWidth(width);

  QPainterPath pp;
  pp.addPolygon(polygon);
  pp.closeSubpath();

  m_arrowHead = new QGraphicsPathItem(this);
  m_arrowHead->setPen(pen);
  m_arrowHead->setBrush(pen.color());
  m_arrowHead->setPath(pp);
  m_arrowHead->setTransformOriginPoint(point);
  m_arrowHead->setRotation(-QLineF(m_point, point).angle());
}


QGraphicsItem *ArrowCreator::create(EditorScene *scene, const QPointF &point)
{
  ArrowItem *item = new ArrowItem();
  scene->addItem(item);
  item->start(point, scene->pen());

  return item;
}
