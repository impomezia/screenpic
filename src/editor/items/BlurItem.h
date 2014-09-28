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

#ifndef BLURITEM_H_
#define BLURITEM_H_

#include <QGraphicsRectItem>

#include "items/EditorItem.h"
#include "items/ItemCommand.h"

class BlurItem : public QGraphicsRectItem
{
public:
  enum { Type = UserType + 66 };

  BlurItem(QGraphicsItem *parent = 0);
  inline int type() const { return Type; }
  void reload(const QPixmap &pixmap);
  void start(const QPointF &point, const QPixmap &pixmap);

protected:
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
  QGraphicsPixmapItem *m_item;
  QPixmap m_pixmap;
  QPointF m_point;
};


class BlurCreator : public EditorItem
{
public:
  int width() const override    { return 0; }
  QColor color() const override { return QColor(); }
  QString id() const override   { return QLatin1String("Blur"); }
  IItemCommand *command(EditorScene *scene, const QPointF &point) override;
  QGraphicsItem *create(EditorScene *scene, const QPointF &point) override;
};


class BlurCommand : public ItemCommand
{
public:
  BlurCommand(EditorScene *scene, const QPointF &point, const QPixmap &pixmap) : ItemCommand(scene, point), m_pixmap(pixmap) {}

protected:
  void create() override;

private:
  QPixmap m_pixmap;
};

#endif /* BLURITEM_H_ */
