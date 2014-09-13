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

#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>

#include "3rdparty/qblowfish/qblowfish.h"
#include "ImgurKeys.h"
#include "ImgurProviderPlugin.h"
#include "ImgurSettings.h"
#include "ImgurUploader.h"
#include "interfaces/IProviderListener.h"
#include "interfaces/ISettings.h"
#include "JSON.h"
#include "sglobal.h"

ImgurProviderPlugin::ImgurProviderPlugin()
  : QObject()
  , IPlugin()
  , IProvider()
  , m_listener(0)
  , m_expires(0)
{
}


int ImgurProviderPlugin::maxImages() const
{
  return 1;
}


QIcon ImgurProviderPlugin::icon() const
{
  return QIcon(LS(":/images/providers/imgur.png"));
}


qint64 ImgurProviderPlugin::maxSize() const
{
  return 5 * 1024 * 1024;
}


QString ImgurProviderPlugin::id() const
{
  return LS("imgur");
}


QString ImgurProviderPlugin::name() const
{
  return LS("Imgur.com");
}


QVariant ImgurProviderPlugin::data() const
{
  QVariantMap map;
  QVariantList a;
  a.append(m_clientId);
  a.append(m_clientSecret);

  if (!m_refreshToken.isEmpty())
    a.append(m_refreshToken);

  if (!m_accessToken.isEmpty() && m_expires != 0 && (m_expires - QDateTime::currentMSecsSinceEpoch()) > 600000)
    a.append(m_accessToken);

  map.insert("a", a);
  return map;
}


QWidget *ImgurProviderPlugin::settingsWidget(QWidget *parent)
{
# ifndef IMGUR_HAS_CLIENT_SECRET
  return 0;
# endif

  m_settingsWidget = new ImgurSettings(parent);
  connect(m_settingsWidget, SIGNAL(pinRequest()), SLOT(onPinRequest()));
  connect(m_settingsWidget, SIGNAL(pinReady(QString)), SLOT(onPinReady(QString)));

  return m_settingsWidget;
}


Uploader *ImgurProviderPlugin::uploader(QObject *parent) const
{
  return new ImgurUploader(parent);
}


void ImgurProviderPlugin::handleReply(const ChatId &id, const QVariant &data)
{
  Q_UNUSED(id)
  const QVariantMap map = data.toMap();

  if (map.isEmpty())
    return;

  if (map.value(LS("type")) == LS("token")) {
    const QVariantMap reply = map.value(LS("reply")).toMap();
    if (reply.isEmpty())
      return;

    if (!reply.contains(LS("access_token"))) {
      if (m_settingsWidget)
        m_settingsWidget->setError(reply.value(LS("data")).toMap().value(LS("error")).toString());

      return;
    }

    m_accessToken  = reply.value(LS("access_token")).toString();
    m_username     = reply.value(LS("account_username")).toString();
    m_refreshToken = reply.value(LS("refresh_token")).toString();
    m_expires      = QDateTime::currentMSecsSinceEpoch() + (reply.value(LS("expires_in")).toInt() * 1000);

    saveToken();
  }
}


void ImgurProviderPlugin::init(ISettings *settings, IProviderListener *listener)
{
  m_settings = settings;
  m_listener = listener;

  m_clientId     = QString::fromLatin1(QByteArray::fromRawData(reinterpret_cast<const char*>(clientId), sizeof(clientId)));
  m_clientSecret = QString::fromLatin1(QByteArray::fromRawData(reinterpret_cast<const char*>(clientSecret), sizeof(clientSecret)).toHex());

  const QByteArray token = m_settings->value(id() + LS(".provider/Token")).toString().toLatin1();
  if (!token.isEmpty()) {
    QBlowfish bf(QByteArray::fromRawData(reinterpret_cast<const char*>(key), sizeof(key)));
    bf.setPaddingEnabled(true);

    const QVariantList data = JSON::parse(bf.decrypted(QByteArray::fromBase64(token))).toList();
    if (data.size() >= 4) {
      m_clientId     = data.at(0).toString();
      m_clientSecret = data.at(1).toString();
      m_refreshToken = data.at(2).toString();
      m_username     = data.at(3).toString();
    }
  }
}


void ImgurProviderPlugin::onPinReady(const QString &pin)
{
  if (pin.isEmpty())
    return;

  QVariantMap map = data().toMap();
  map.insert(LS("pin"), pin);

  m_listener->onCustomRequest(ChatId().init(ChatId::MessageId), id(), map);
}


void ImgurProviderPlugin::onPinRequest()
{
  QDesktopServices::openUrl(QUrl(LS("https://api.imgur.com/oauth2/authorize?client_id=") + m_clientId + LS("&response_type=pin")));
}


void ImgurProviderPlugin::saveToken()
{
  QVariantList token;
  token.append(m_clientId);
  token.append(m_clientSecret);
  token.append(m_refreshToken);
  token.append(m_username);

  QBlowfish bf(QByteArray::fromRawData(reinterpret_cast<const char*>(key), sizeof(key)));
  bf.setPaddingEnabled(true);

  m_settings->setValue(id() + LS(".provider/Token"), QString::fromLatin1(bf.encrypted(JSON::generate(token)).toBase64()));
}

Q_EXPORT_PLUGIN2(ImgurProvider, ImgurProviderPlugin);
