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

#ifndef SPINNER_H_
#define SPINNER_H_

#include <QWidget>

class Spinner : public QWidget
{
  Q_OBJECT

public:
  Spinner(QWidget *parent = 0);
  inline bool isAnimated () const            { return m_timerId != -1; }
  inline const QColor &color() const         { return m_color; }
  inline int delay() const                   { return m_delay; }
  inline void setColor(const QColor &color)  { m_color = color; update(); }
  int heightForWidth(int w) const override   { return w; }
  QSize sizeHint() const override            { return QSize(16, 16); }
  void setDelay(int delay);

public slots:
  void start();
  void stop();

protected:
  void timerEvent(QTimerEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

private:
  bool m_displayedWhenStopped;
  int m_angle;
  int m_delay;
  int m_timerId;
  QColor m_color;
};

#endif // SPINNER_H
