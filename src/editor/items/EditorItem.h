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

#ifndef EDITORITEM_H_
#define EDITORITEM_H_

#include <QColor>

class EditorScene;
class IItemCommand;
class QGraphicsItem;
class QPointF;

class EditorItem
{
public:
  inline EditorItem() {}
  inline virtual ~EditorItem() {}
  inline virtual int width() const              { return 3; }
  inline virtual QColor color() const           { return 0xffd60808; }
  inline virtual QString id() const             { return QString(); }
  inline virtual Qt::CursorShape cursor() const { return Qt::CrossCursor; }
  virtual IItemCommand *command(EditorScene *scene, const QPointF &point);
  virtual QGraphicsItem *create(EditorScene *scene, const QPointF &point) = 0;
};

#endif /* EDITORITEM_H_ */
