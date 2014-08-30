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

#ifndef COLORCOMMAND_H_
#define COLORCOMMAND_H_

#include <QColor>
#include <QUndoCommand>

class QColor;
class QGraphicsItem;

class ColorCommand : public QUndoCommand
{
public:
  ColorCommand(const QList<QGraphicsItem *> items, const QColor &color, QUndoCommand *parent = 0);
  bool isValid() const;
  static QColor getColor(QGraphicsItem *item);
  static QColor getColor(const QList<QGraphicsItem *> items);
  void redo() override;
  void undo() override;

private:
  void apply(const QColor &color);

  QColor m_color;
  QColor m_original;
  QGraphicsItem *m_item;
};

#endif // COLORCOMMAND_H_
