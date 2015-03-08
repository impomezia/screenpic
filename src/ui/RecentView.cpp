/*   Copyright (C) 2013-2015 Alexander Sedov <imp@schat.me>
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

#include <QHeaderView>

#include "RecentView.h"

RecentView::RecentView(QWidget *parent)
  : QTreeView(parent)
{
  setObjectName("RecentView");

  setFrameShadow(QFrame::Plain);
  setFrameShape(QFrame::NoFrame);
  setFocusPolicy(Qt::NoFocus);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setAlternatingRowColors(true);
  setRootIsDecorated(false);
  setHeaderHidden(true);
  setMinimumHeight(164);
  setMinimumWidth(200);
  setMouseTracking(true);

# if QT_VERSION >= 0x050000
  header()->setSectionResizeMode(QHeaderView::ResizeToContents);
# else
  header()->setResizeMode(QHeaderView::ResizeToContents);
# endif
}


QSize RecentView::sizeHint() const
{
  return QSize(164, 128);
}
