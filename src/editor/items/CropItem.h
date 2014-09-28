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

#ifndef CROPITEM_H_
#define CROPITEM_H_

#include <QGraphicsRectItem>
#include <QUndoCommand>

#include "items/EditorItem.h"

class CropItem : public QGraphicsRectItem
{
public:
  CropItem(QGraphicsItem *parent = 0);
  void start(const QPointF &point, const QPen &pen);

protected:
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
  QPointF m_point;
  QPixmap m_pixmap;
};


class CropCreator : public EditorItem
{
public:
  int width() const override    { return 0; }
  QColor color() const override { return QColor(); }
  QString id() const override   { return QLatin1String("Crop"); }
  IItemCommand *command(EditorScene *scene, const QPointF &point) override;
  QGraphicsItem *create(EditorScene *scene, const QPointF &point) override;
};


class CropCommand : public QUndoCommand
{
public:
  CropCommand(EditorScene *scene, const QPixmap &original, const QPixmap &crop)
    : QUndoCommand()
    , m_scene(scene)
    , m_crop(crop)
    , m_original(original)
  {}

  void redo() override;
  void undo() override;

private:
  void apply(const QPixmap &pixmap);

  EditorScene *m_scene;
  QPixmap m_crop;
  QPixmap m_original;
};

#endif // CROPITEM_H_
