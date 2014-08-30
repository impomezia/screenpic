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

#ifndef ITEMCOLORSELECTOR_H_
#define ITEMCOLORSELECTOR_H_

#include <QMap>
#include <QToolBar>

class QBoxLayout;
class QToolButton;
class QVBoxLayout;

class ItemColorSelector : public QToolBar
{
  Q_OBJECT

public:
  ItemColorSelector(QWidget *parent = 0);
  bool setColor(const QColor &color);
  QColor customColor() const;

signals:
  void changed(QRgb color);
  void dropperClicked();

protected:
  void changeEvent(QEvent *event) override;

private slots:
  void onClicked(bool checked);
  void onDropperClicked();

private:
  QPixmap pixmap(const QColor &color) const;
  QToolButton *add(const QColor &color, QBoxLayout *layout);
  void reload();
  void retranslateUi();

  QColor m_customColor;
  QMap<QRgb, QToolButton*> m_colors;
  QToolButton *m_customColorBtn;
  QToolButton *m_dropperBtn;
  QVBoxLayout *m_dropperLayout;
};

#endif /* ITEMCOLORSELECTOR_H_ */
