/*   $Id$
 *   Copyright (C) 2013 Alexander Sedov <imp@schat.me>
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

#include "AppCore.h"
#include "GlobalShortcuts.h"
#include "libqxt/qxtglobalshortcut.h"
#include "Settings.h"
#include "sglobal.h"

const QString GlobalShortcuts::kFullscreen = LS("Shortcuts/Fullscreen");
const QString GlobalShortcuts::kRectangle  = LS("Shortcuts/Rectangle");

GlobalShortcuts::GlobalShortcuts(AppCore *core, QObject *parent)
  : QObject(parent)
  , m_core(core)
  , m_settings(core->settings())
{
  init(kFullscreen, LS("Print"));
  init(kRectangle,  LS("Ctrl+Print"));
}


bool GlobalShortcuts::set(const QString &id, const QKeySequence &shortcut)
{
  if (!m_map.contains(id))
    return false;

  QxtGlobalShortcut *gs = m_map.value(id);
  if (gs && gs->shortcut() == shortcut)
    return true;

  if (gs)
    delete gs;

  gs = new QxtGlobalShortcut(this);
  if (!gs->setShortcut(shortcut)) {
    delete gs;
    gs = 0;
  }

  m_core->settings()->setValue(id, shortcut.toString());
  init(id, gs);
  return gs;
}


bool GlobalShortcuts::unset(const QString &id, const QKeySequence &shortcut)
{
  if (!m_map.contains(id))
    return false;

  QxtGlobalShortcut *gs = m_map.value(id);
  if (gs)
    delete gs;

  m_map.insert(id, 0);
  m_core->settings()->setValue(id, LC('-') + shortcut.toString());
  return true;
}


void GlobalShortcuts::init(const QString &id, const QString &str)
{
  m_settings->setDefault(id, str);
  const QKeySequence seq      = str.startsWith(LC('-')) ? QKeySequence() : QKeySequence(m_settings->value(id).toString());
  QxtGlobalShortcut *shortcut = seq.isEmpty() ? 0 : new QxtGlobalShortcut(this);
  if (shortcut && !shortcut->setShortcut(seq)) {
    delete shortcut;
    shortcut = 0;
  }

  init(id, shortcut);
}


void GlobalShortcuts::init(const QString &id, QxtGlobalShortcut *shortcut)
{
  m_map.insert(id, shortcut);
  if (!shortcut)
    return;

  if (id == kFullscreen)
    connect(shortcut, SIGNAL(activated()), m_core, SLOT(grabScreen()));
  else if (id == kRectangle)
    connect(shortcut, SIGNAL(activated()), m_core, SLOT(grabRect()));
}
