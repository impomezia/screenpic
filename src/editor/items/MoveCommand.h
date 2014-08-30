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

#ifndef MOVECOMMAND_H_
#define MOVECOMMAND_H_

#include <QUndoCommand>
#include <QPointF>

class QGraphicsItem;

class MoveCommand : public QUndoCommand
{
public:
  MoveCommand(QGraphicsItem *item, const QPointF &oldPos, QUndoCommand *parent = 0);
  virtual void redo();
  virtual void undo();

private:
  QGraphicsItem *m_item;
  QPointF m_oldPos;
  QPointF m_pos;
};

#endif /* MOVECOMMAND_H_ */
