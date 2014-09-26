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

#ifndef OBSERVERS_H_
#define OBSERVERS_H_

#include <QObject>

class AppCore;
class IObserver;
class IScreenpic;

#include "interfaces/IObserverListener.h"

class Observers : public QObject, public IObserverListener
{
  Q_OBJECT

public:
  Observers(IScreenpic *screenpic, const QObjectList &observers, QObject *parent = 0);
  ~Observers();
  static void hitEvent(const QString &category, const QString &action, const QString &label = QString(), int value = -1);
  static void watch(QWidget *widget);

private:
  QList<IObserver*> m_observers;
  static Observers *m_self;
};

#endif // OBSERVERS_H_
