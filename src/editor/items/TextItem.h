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

#ifndef TEXTITEM_H_
#define TEXTITEM_H_

#include <QGraphicsTextItem>

#include "items/EditorItem.h"
#include "items/ItemCommand.h"

class TextItem : public QGraphicsTextItem
{
public:
  enum { Type = UserType + 84 };

  TextItem(QGraphicsItem *parent = 0);
  int type() const override { return Type; }
  void start(const QPointF &point, const QPen &pen);

protected:
  void focusInEvent(QFocusEvent *event) override;
  void focusOutEvent(QFocusEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
};


class TextCreator : public EditorItem
{
public:
  int width() const override              { return 10; }
  QString id() const override             { return QLatin1String("Text"); }
  Qt::CursorShape cursor() const override { return Qt::IBeamCursor; }
  IItemCommand *command(EditorScene *scene, const QPointF &point) override;
  QGraphicsItem *create(EditorScene *scene, const QPointF &point) override;
};


class TextCommand : public ItemCommand
{
public:
  TextCommand(EditorScene *scene, const QPointF &point) : ItemCommand(scene, point) {}
  bool validate() override;

protected:
  void create() override;
};

#endif /* TEXTITEM_H_ */
