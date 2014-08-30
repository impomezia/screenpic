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

#include "CropItem.h"
#include "EditorScene.h"

CropItem::CropItem(QGraphicsItem *parent)
  : QGraphicsRectItem(parent)
{
  setFlag(ItemIsSelectable);
}


void CropItem::start(const QPointF &point, const QPen &pen)
{
  grabMouse();

  m_point = point;

  setPen(Qt::NoPen);
  setRect(QRectF(point, QSizeF(pen.widthF(), pen.widthF())));
}


void CropItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsRectItem::mouseMoveEvent(event);
  if (flags() & ItemIsMovable)
    return;

  const QPointF pos = event->scenePos();
  const bool square = (event->modifiers() & Qt::SHIFT);
  QRectF r;
  int flags = 0;

  r.setTopLeft(m_point);
  r.setBottomRight(event->scenePos());

  if (r.width() < 0) { // left offset.
    flags |= 1;
    r.setLeft(pos.x());
    r.setRight(m_point.x());
  }


  if (r.height() < 0) { // top offset.
    flags |= 2;
    r.setTop(pos.y());
    r.setBottom(m_point.y());
  }

  if (square) {
    const qreal min = qMin(r.width(), r.height());

    if (flags == 0) { // button right
      r.setWidth(min);
      r.setHeight(min);
    }

    if (flags == 1) { // button left
      r.setLeft(m_point.x() - min);
      r.setHeight(min);
    }

    if (flags == 2) { // top right
      r.setTop(m_point.y() - min);
      r.setWidth(min);
    }

    if (flags == 3) { // top left
      r.setLeft(m_point.x() - min);
      r.setTop(m_point.y() - min);
    }
  }

  r = r.intersected(scene()->sceneRect());

  setRect(r);
  setSelected(true);
}


void CropItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsRectItem::mouseReleaseEvent(event);
  if (flags() & ItemIsMovable)
    return;

  ungrabMouse();
  setFlag(ItemIsMovable);
  setFlag(ItemIsSelectable, false);

  EditorScene *scene = qobject_cast<EditorScene*>(this->scene());
  if (scene)
    scene->crop(rect());
}


IItemCommand *CropCreator::command(EditorScene *scene, const QPointF &point)
{
  Q_UNUSED(scene)
  Q_UNUSED(point)

  return 0;
}


QGraphicsItem *CropCreator::create(EditorScene *scene, const QPointF &point)
{
  scene->toPixmap();

  CropItem *item = new CropItem();
  scene->addItem(item);
  item->start(point, scene->pen());

  return item;
}


void CropCommand::redo()
{
  m_scene->clear();
  m_scene->addPixmap(m_crop);
  m_scene->setSceneRect(0, 0, m_crop.width(), m_crop.height());
}


void CropCommand::undo()
{
  m_scene->clear();
  m_scene->addPixmap(m_original);
  m_scene->setSceneRect(0, 0, m_original.width(), m_original.height());
}
