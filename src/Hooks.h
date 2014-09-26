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

#ifndef HOOKS_H_
#define HOOKS_H_

#include <QObject>

#include "interfaces/IHook.h"

class IScreenpic;

class Hooks : public QObject, public IHook
{
  Q_OBJECT
  Q_INTERFACES(IHook)

public:
  Hooks(IScreenpic *screenpic, const QObjectList &hooks, QObject *parent = 0);
  void hookAboutPage(QVBoxLayout *layout, QWidget *parent) override;
  void hookUploadData(const QString &provider, QVariant &data) override;

private:
  QList<IHook*> m_hooks;
};

#endif // HOOKS_H
