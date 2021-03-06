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

#include <QPainter>
#include <QTimerEvent>

#include "Spinner.h"

Spinner::Spinner(QWidget *parent)
  : QWidget(parent)
  , m_displayedWhenStopped(false)
  , m_angle(0)
  , m_delay(100)
  , m_timerId(-1)
  , m_color(Qt::black)
{
  setMaximumSize(16, 16);
}


void Spinner::setDelay(int delay)
{
  if (m_timerId != -1)
    killTimer(m_timerId);

  m_delay = delay;

  if (m_timerId != -1)
    m_timerId = startTimer(m_delay);
}


void Spinner::start()
{
  m_angle = 0;

  if (m_timerId == -1)
    m_timerId = startTimer(m_delay);
}


void Spinner::stop()
{
  if (m_timerId != -1)
    killTimer(m_timerId);

  m_timerId = -1;
  update();
}


void Spinner::timerEvent(QTimerEvent *event)
{
  if (event->timerId() == m_timerId) {
    m_angle = (m_angle + 30) % 360;
    update();
  }

  QWidget::timerEvent(event);
}


void Spinner::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event)

  if (!m_displayedWhenStopped && !isAnimated())
    return;

  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);
  const int width         = qMin(this->width(), this->height());
  const int outerRadius   = (width - 1) * 0.5;
  const int innerRadius   = (width - 1) * 0.5 * 0.4;
  const int capsuleHeight = outerRadius - innerRadius;
  const int capsuleWidth  = (width > 32) ? capsuleHeight * 0.23 : capsuleHeight * 0.35;
  const int capsuleRadius = capsuleWidth / 2;

  for (int i = 0; i < 12; i++) {
    QColor color = m_color;
    color.setAlphaF(1.0f - (i / 12.0f));
    p.setPen(Qt::NoPen);
    p.setBrush(color);
    p.save();
    p.translate(rect().center());
    p.rotate(m_angle - i * 30.0f);
    p.drawRoundedRect(-capsuleWidth * 0.5, -(innerRadius + capsuleHeight), capsuleWidth, capsuleHeight, capsuleRadius, capsuleRadius);
    p.restore();
  }
}
