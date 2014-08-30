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

#include <QApplication>
#include <QDesktopWidget>
#include <QWidget>

#include "Flyout.h"

#define FLYOUT_OFFSET 7

void Flyout::show(QWidget *widget, const QRect &trayRect)
{
  QDesktopWidget *desktop = QApplication::desktop();

  const QRect ag = desktop->availableGeometry(trayRect.topLeft());
  const QRect sg = desktop->screenGeometry(trayRect.topLeft());
  TaskBarPos pos;

  if (ag.width() != sg.width())
    pos = (ag.left() == sg.left() ? Right : Left);
  else
    pos = (ag.top() == sg.top() ? Bottom : Top);

  widget->move(-20000, -20000);
  widget->show();

  const QSize size = widget->frameGeometry().size();
  QRect rect;

  if (pos == Bottom || pos == Top) {
    rect.setY(pos == Bottom ? ag.bottom() - size.height() - FLYOUT_OFFSET : ag.top() + FLYOUT_OFFSET);
    rect.setX((trayRect.x() + trayRect.width() / 2) - size.width() / 2);
    rect.setSize(size);

    if (!ag.contains(rect))
      rect.setX(ag.right() - size.width() - FLYOUT_OFFSET);
  }
  else {
    rect.setX(pos == Right ? ag.right() - size.width() - FLYOUT_OFFSET : ag.left() + FLYOUT_OFFSET);
    rect.setY((trayRect.y() + trayRect.height() / 2) - size.height() / 2);
    rect.setSize(size);

    if (!ag.contains(rect))
      rect.setY(ag.bottom() - size.height() - FLYOUT_OFFSET);
  }

  widget->move(rect.topLeft());
  widget->raise();
  widget->activateWindow();
}
