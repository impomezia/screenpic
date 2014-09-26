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

#include <QLabel>

#include "3rdparty/qblowfish/qblowfish.h"
#include "interfaces/IScreenpic.h"
#include "interfaces/ISettings.h"
#include "RupProviderPlugin.h"
#include "RupSettings.h"
#include "RupUploader.h"
#include "sglobal.h"

const unsigned char key[] = {
  0x03, 0xb9, 0x74, 0xde, 0xc3, 0xf3, 0xee, 0x4a,
  0xf1, 0xfa, 0x95, 0x46, 0xaa, 0x0c, 0xa9, 0x5f
};


int RupProviderPlugin::maxImages() const
{
  return 1;
}


QIcon RupProviderPlugin::icon() const
{
  return QIcon(LS(":/images/providers/rup.png"));
}


qint64 RupProviderPlugin::maxSize() const
{
  return 10 * 1024 * 1024;
}


QString RupProviderPlugin::id() const
{
  return LS("rup");
}


QString RupProviderPlugin::name() const
{
  return LS("Rup.io");
}


QVariant RupProviderPlugin::data() const
{
  QVariantMap map;
  if (m_token.size() == 42) {
    map.insert(LS("token"), m_token);
  }

  return map;
}


QWidget *RupProviderPlugin::settingsWidget(QWidget *parent)
{
  RupSettings *widget = new RupSettings(m_token, parent);
  connect(widget, SIGNAL(changed(QString)), SLOT(onTokenChanged(QString)));

  return widget;
}


Uploader *RupProviderPlugin::uploader(QObject *parent) const
{
  return new RupUploader(parent);
}


void RupProviderPlugin::handleReply(const ChatId &id, const QVariant &data)
{
  Q_UNUSED(id)
  Q_UNUSED(data)
}


void RupProviderPlugin::init(IScreenpic *screenpic, IProviderListener *listener)
{
  Q_UNUSED(listener)

  m_screenpic = screenpic;
  m_token = m_screenpic->settings()->value(id() + LS(".provider/Token")).toString();

  if (m_token.size() != 42) {
    QBlowfish bf(QByteArray::fromRawData(reinterpret_cast<const char*>(key), sizeof(key)));
    bf.setPaddingEnabled(true);

    m_token = bf.decrypted(QByteArray::fromBase64(m_token.toLatin1()));
  }
}


void RupProviderPlugin::onTokenChanged(const QString &token)
{
  QBlowfish bf(QByteArray::fromRawData(reinterpret_cast<const char*>(key), sizeof(key)));
  bf.setPaddingEnabled(true);

  m_token = token;
  m_screenpic->settings()->setValue(id() + LS(".provider/Token"), QString::fromLatin1(bf.encrypted(token.toLatin1()).toBase64()));
}

Q_EXPORT_PLUGIN2(RupProvider, RupProviderPlugin);
