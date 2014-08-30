/*   $Id$
 *   Copyright (C) 2013 Alexander Sedov <imp@schat.me>
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

#include <QGraphicsItem>

#include "editor/EditorScene.h"
#include "items/MoveCommand.h"

MoveCommand::MoveCommand(QGraphicsItem *item, const QPointF &oldPos, QUndoCommand *parent)
  : QUndoCommand(parent)
  , m_item(item)
  , m_oldPos(oldPos)
  , m_pos(item->pos())
{
}


void MoveCommand::redo()
{
  m_item->setPos(m_pos);

  EditorScene *scene = qobject_cast<EditorScene *>(m_item->scene());
  if (scene)
    scene->reload();
}


void MoveCommand::undo()
{
  m_item->setPos(m_oldPos);

  EditorScene *scene = qobject_cast<EditorScene *>(m_item->scene());
  if (scene)
    scene->reload();
}
