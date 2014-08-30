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

#include <QGraphicsBlurEffect>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QTransform>

#include "EditorScene.h"
#include "items/BlurItem.h"

BlurItem::BlurItem(QGraphicsItem *parent)
  : QGraphicsRectItem(parent)
{
  m_item = new QGraphicsPixmapItem(this);

  QGraphicsBlurEffect *blur = new QGraphicsBlurEffect();
  blur->setBlurRadius(6);
  blur->setBlurHints(QGraphicsBlurEffect::PerformanceHint);
  m_item->setGraphicsEffect(blur);

  setFlag(ItemIsSelectable);
}


void BlurItem::reload(const QPixmap &pixmap)
{
  const QRectF r = rect();
  if (r.isNull() && !r.isValid())
    return;

  QTransform t = sceneTransform();
  t.translate(qAbs(scene()->sceneRect().left()), qAbs(scene()->sceneRect().top()));
  m_item->setPixmap(pixmap.copy(t.mapRect(r).toRect()));
}


void BlurItem::start(const QPointF &point, const QPixmap &pixmap)
{
  grabMouse();

  setPen(Qt::NoPen);

  m_point  = point;
  m_pixmap = pixmap;
}


void BlurItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsRectItem::mouseMoveEvent(event);

  const QPointF pos = event->scenePos();
  QRectF r;

  r.setTopLeft(m_point);
  r.setBottomRight(event->scenePos());

  if (r.width() < 0) {
    r.setLeft(pos.x());
    r.setRight(m_point.x());
  }

  if (r.height() < 0) {
    r.setTop(pos.y());
    r.setBottom(m_point.y());
  }

  r = r.intersected(m_item->scene()->sceneRect());

  setRect(r);
  setSelected(true);
  m_item->setOffset(r.topLeft());

  reload(m_pixmap);
}


void BlurItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  ungrabMouse();
  setFlag(ItemIsSelectable, false);
  m_pixmap = QPixmap();

  QGraphicsRectItem::mouseReleaseEvent(event);
}


IItemCommand *BlurCreator::command(EditorScene *scene, const QPointF &point)
{
  return new BlurCommand(scene, point, scene->toPixmap());
}


QGraphicsItem *BlurCreator::create(EditorScene *scene, const QPointF &point)
{
  Q_UNUSED(scene)
  Q_UNUSED(point);

  return 0;
}


void BlurCommand::create()
{
  BlurItem *item = new BlurItem();
  m_scene->addItem(item);
  item->start(m_point, m_pixmap);

  m_item = item;
}
