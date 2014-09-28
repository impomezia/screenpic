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

#ifndef WIDTHCOMMAND_H_
#define WIDTHCOMMAND_H_

#include <QUndoCommand>

class QGraphicsItem;

class WidthCommand : public QUndoCommand
{
public:
  WidthCommand(const QList<QGraphicsItem *> items, const int &size, QUndoCommand *parent = 0);
  bool isValid() const;
  static int getWidth(QGraphicsItem *item);
  static int getWidth(const QList<QGraphicsItem *> items);
  void redo() override;
  void undo() override;

private:
  void apply(int width);

  int m_width;
  int m_originalWidth;
  QGraphicsItem *m_item;
};

#endif // WIDTHCOMMAND_H_
