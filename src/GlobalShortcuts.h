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

#ifndef GLOBALSHORTCUTS_H_
#define GLOBALSHORTCUTS_H_

#include <QKeySequence>
#include <QMap>
#include <QObject>

class AppCore;
class ISettings;
class QxtGlobalShortcut;

class GlobalShortcuts : public QObject
{
  Q_OBJECT

public:
  static const QString kFullscreen;
  static const QString kRectangle;

  GlobalShortcuts(AppCore *core, QObject *parent = 0);
  bool set(const QString &id, const QKeySequence &shortcut);
  bool unset(const QString &id, const QKeySequence &shortcut);
  inline const QMap<QString, QxtGlobalShortcut*>& map() const { return m_map; }
  inline QxtGlobalShortcut *get(const QString &id) const      { return m_map.value(id); }

private slots:
  void grabRect();
  void grabScreen();

private:
  void init(const QString &id, const QString &str);
  void init(const QString &id, QxtGlobalShortcut *shortcut);

  AppCore *m_core;
  ISettings *m_settings;
  QMap<QString, QxtGlobalShortcut*> m_map;
};

#endif /* GLOBALSHORTCUTS_H_ */
