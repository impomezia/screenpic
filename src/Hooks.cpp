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

#include "Hooks.h"

Hooks::Hooks(IScreenpic *screenpic, const QObjectList &hooks, QObject *parent)
  : QObject(parent)
{
  Q_UNUSED(screenpic)

  for (int i = 0; i < hooks.size(); ++i) {
    IHook *hook = qobject_cast<IHook*>(hooks.at(i));
    m_hooks.append(hook);
  }
}


void Hooks::hookAboutPage(QVBoxLayout *layout, QWidget *parent)
{
  foreach (IHook *hook, m_hooks) {
    hook->hookAboutPage(layout, parent);
  }
}


void Hooks::hookUploadData(const QString &provider, QVariant &data)
{
  foreach (IHook *hook, m_hooks) {
    hook->hookUploadData(provider, data);
  }
}
