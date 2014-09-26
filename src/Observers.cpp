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

#include <QEvent>
#include <QShowEvent>
#include <QWidget>

#include "AppCore.h"
#include "interfaces/IObserver.h"
#include "Observers.h"

Observers *Observers::m_self = 0;

Observers::Observers(IScreenpic *screenpic, const QObjectList &observers, QObject *parent)
  : QObject(parent)
{
  m_self = this;

  for (int i = 0; i < observers.size(); ++i) {
    IObserver *observer = qobject_cast<IObserver*>(observers.at(i));

    observer->init(screenpic, this);
    m_observers.append(observer);
  }
}


Observers::~Observers()
{
  m_self = 0;
}


void Observers::hitEvent(const QString &category, const QString &action, const QString &label, int value)
{
  Q_ASSERT(m_self);
  if (!m_self)
    return;

  foreach (IObserver *observer, m_self->m_observers) {
    observer->hitEvent(category, action, label, value);
  }
}


void Observers::watch(QWidget *widget)
{
  Q_ASSERT(m_self);
  if (!m_self)
    return;

  foreach (IObserver *observer, m_self->m_observers) {
    observer->watch(qobject_cast<QWidget*>(widget));
  }
}
