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

#ifndef WEBCOLORWIDGET_H_
#define WEBCOLORWIDGET_H_

#include <QLineEdit>

class QBoxLayout;
class QToolButton;

class LineEdit : public QLineEdit
{
  Q_OBJECT

public:
  LineEdit(QWidget *parent = 0);
  QSize	sizeHint() const override;
};


class WebColorWidget : public QWidget
{
  Q_OBJECT

public:
  WebColorWidget(QWidget *parent = 0);

signals:
  void changed(const QColor &color);

public slots:
  void copy();
  void setRgb(QRgb color);

protected:
  void changeEvent(QEvent *event) override;
  void showEvent(QShowEvent *event) override;

private slots:
  void onReturnPressed();

private:
  void retranslateUi();

  LineEdit *m_lineEdit;
  QToolButton *m_copyBtn;
};

#endif // WEBCOLORWIDGET_H_
