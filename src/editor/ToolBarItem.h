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

#ifndef TOOLBARITEM_H_
#define TOOLBARITEM_H_

#include <QWidget>

class ToolBarItem : public QWidget
{
  Q_OBJECT

public:
  ToolBarItem(QWidget *parent = 0);

signals:
  void toggled(bool checked);

public slots:
  virtual void setChecked(bool checked) = 0;

protected:
  virtual void retranslateUi() = 0;
  void changeEvent(QEvent *event) override;
};

#endif // TOOLBARITEM_H_
