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

#include <QDesktopServices>
#include <QUrl>

#include "ImgurKeys.h"
#include "ImgurProviderPlugin.h"
#include "ImgurSettings.h"
#include "ImgurUploader.h"
#include "interfaces/IProviderListener.h"
#include "sglobal.h"

ImgurProviderPlugin::ImgurProviderPlugin()
  : QObject()
  , IPlugin()
  , IProvider()
  , m_listener(0)
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

  map.insert("a", a);
  return map;
}


QWidget *ImgurProviderPlugin::settingsWidget(QWidget *parent)
{
# ifndef IMGUR_HAS_CLIENT_SECRET
  return 0;
# endif

  ImgurSettings *widget = new ImgurSettings(parent);
  connect(widget, SIGNAL(pinRequest()), SLOT(onPinRequest()));
  connect(widget, SIGNAL(pinReady(QString)), SLOT(onPinReady(QString)));

  return widget;
}


Uploader *ImgurProviderPlugin::uploader(QObject *parent) const
{
  return new ImgurUploader(parent);
}


void ImgurProviderPlugin::handleReply(const ChatId &id, const QVariant &data)
{
  Q_UNUSED(id)
  Q_UNUSED(data)
}


void ImgurProviderPlugin::init(ISettings *settings, IProviderListener *listener)
{
  Q_UNUSED(settings);

  m_listener = listener;

  m_clientId     = QString::fromLatin1(QByteArray::fromRawData(reinterpret_cast<const char*>(clientId), sizeof(clientId)));
  m_clientSecret = QString::fromLatin1(QByteArray::fromRawData(reinterpret_cast<const char*>(clientSecret), sizeof(clientSecret)).toHex());
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

Q_EXPORT_PLUGIN2(ImgurProvider, ImgurProviderPlugin);
