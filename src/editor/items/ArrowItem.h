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

#ifndef ARROWITEM_H_
#define ARROWITEM_H_

#include <QGraphicsLineItem>

#include "items/EditorItem.h"
#include "items/ItemCommand.h"

class ArrowItem : public QGraphicsLineItem
{
public:
  enum { Type = UserType + 65 };

  ArrowItem(QGraphicsItem *parent = 0);
  inline int type() const { return Type; }
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
  void setColor(const QColor &color);
  void setWidth(const int &width);
  void start(const QPointF &point, const QPen &pen);

protected:
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
  qreal calculateAngle(const QPointF &start, const QPointF &end) const;
  void arrowHead(const QPointF &point);

  QGraphicsPathItem *m_arrowHead;
  QPointF m_point;
};


class ArrowCreator : public EditorItem
{
public:
  QString id() const override { return QLatin1String("Arrow"); }
  QGraphicsItem *create(EditorScene *scene, const QPointF &point) override;
};

#endif /* ARROWITEM_H_ */
