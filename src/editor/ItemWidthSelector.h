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

#ifndef ITEMWIDTHSELECTOR_H_
#define ITEMWIDTHSELECTOR_H_

#include <QWidget>

class QActionGroup;
class QBoxLayout;
class QSlider;
class QSpinBox;
class QToolBar;
class QToolButton;

class ItemWidthSelector : public QWidget
{
  Q_OBJECT

public:
  ItemWidthSelector(QWidget *parent = 0);
  void setAdvanced(bool advanced);
  void setColor(const QColor &color);
  void setWidth(int width);

signals:
  void changed(int width);

private slots:
  void onTriggered(QAction *action);
  void onValueChanged(int value);

private:
  QAction *add(int width, int size);
  QPixmap pixmap(int width, const QColor &color) const;
  void addSpacing(int size);
  void addStretch();

  QActionGroup *m_actionGroup;
  QSlider *m_slider;
  QSpinBox *m_spinBox;
  QToolBar *m_toolBar;
};

#endif // ITEMWIDTHSELECTOR_H_
