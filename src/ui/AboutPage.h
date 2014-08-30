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

#ifndef ABOUTPAGE_H_
#define ABOUTPAGE_H_

#include <QWidget>

class AppCore;
class QCheckBox;

class AboutPage : public QWidget
{
  Q_OBJECT

public:
  AboutPage(AppCore *core, QWidget *parent = 0);

protected:
  void changeEvent(QEvent *event);
  void showEvent(QShowEvent *event);

private slots:
  void onUpdateClicked(bool checked);

private:
  void retranslateUi();

  AppCore *m_core;
  QCheckBox *m_updateBtn;
};

#endif /* ABOUTPAGE_H_ */
