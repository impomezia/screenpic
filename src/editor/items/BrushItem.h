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

#ifndef BRUSHITEM_H_
#define BRUSHITEM_H_

#include <QGraphicsPathItem>

#include "items/EditorItem.h"
#include "items/ItemCommand.h"

class BrushItem : public QGraphicsPathItem
{
public:
  BrushItem(QGraphicsItem *parent = 0);
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
  void start(const QPointF &point, const QPen &pen);

protected:
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
  QPainterPath m_path;
};


class BrushCreator : public EditorItem
{
public:
  QString id() const override            { return QLatin1String("Brush"); }
  Qt::CursorShape cursor() const override { return Qt::PointingHandCursor; }
  QGraphicsItem *create(EditorScene *scene, const QPointF &point) override;
};

#endif /* BRUSHITEM_H_ */
