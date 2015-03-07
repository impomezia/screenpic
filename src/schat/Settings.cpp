/*   Copyright (C) 2013-2015 Alexander Sedov <imp@schat.me>
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

#include "interfaces/ISettingsListener.h"
#include "Settings.h"
#include "sglobal.h"

const QString Settings::kCaptureMouse     = LS("CaptureMouse");
const QString Settings::kEdition          = LS("Edition");
const QString Settings::kEditor           = LS("Editor");
const QString Settings::kFailbackProvider = LS("FailbackProvider");
const QString Settings::kLastOpenDir      = LS("LastOpenDir");
const QString Settings::kLastSaveDir      = LS("LastSaveDir");
const QString Settings::kProvider         = LS("Provider");
const QString Settings::kPublishOnClose   = LS("PublishOnClose");
const QString Settings::kSaveCopy         = LS("SaveCopy");
const QString Settings::kSaveCopyIn       = LS("SaveCopyIn");
const QString Settings::kTextBorder       = LS("TextBorder");
const QString Settings::kTranslation      = LS("Translation");

Settings::Settings(const QString &fileName, QObject *parent)
  : QSettings(fileName, QSettings::IniFormat, parent)
{
  setIniCodec("UTF-8");
}


Settings::Settings(const QString &organization, const QString &application, QObject *parent)
  : QSettings(organization, application, parent)
{
}


QVariant Settings::value(const QString &key, const QVariant &defaultValue) const
{
  if (defaultValue.isValid())
    m_default[key] = defaultValue;

  return QSettings::value(key, m_default.value(key));
}


void Settings::addListener(ISettingsListener *listener)
{
  if (!m_listeners.contains(listener))
    m_listeners.append(listener);
}


void Settings::removeListener(ISettingsListener *listener)
{
  m_listeners.removeAll(listener);
}


void Settings::setDefault(const QString &key, const QVariant &value)
{
  m_default[key] = value;
}


void Settings::setValue(const QString &key, const QVariant &value, bool notify)
{
  if (QSettings::value(key, m_default.value(key)) == value)
    return;

  if (m_default.contains(key) && m_default.value(key) == value)
    QSettings::remove(key);
  else
    QSettings::setValue(key, value);

  if (notify) {
    emit changed(key, value);

    foreach (ISettingsListener *listener, m_listeners) {
      listener->onSettingsChanged(key, value);
    }
  }
}
