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

#ifndef ITEMCOLORBUTTON_H_
#define ITEMCOLORBUTTON_H_

#include <QToolButton>

class ItemColorSelector;
class WebColorWidget;

class ItemColorButton : public QToolButton
{
  Q_OBJECT

public:
  ItemColorButton(QWidget *parent = 0);
  QColor customColor() const;
  void setTempColor(const QColor &color);

signals:
  void changed(QRgb color);
  void dropperClicked();

public slots:
  void setColor(const QColor &color);

private slots:
  void onChanged(QRgb color);

private:
  QPixmap pixmap(const QColor &color) const;

  ItemColorSelector *m_selector;
  QRgb m_color;
  WebColorWidget *m_webColor;
};

#endif /* ITEMCOLORBUTTON_H_ */
