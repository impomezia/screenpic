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

#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "data/ImageItem.h"
#include "DateTime.h"
#include "ImgurUploader.h"
#include "JSON.h"
#include "sglobal.h"
#include "tools/OsInfo.h"
#include "uploaders/UploadResult.h"

ImgurUploader::ImgurUploader(QObject *parent)
  : Uploader(parent)
{
}


void ImgurUploader::request(QNetworkAccessManager *net, const ChatId &id, const QVariant &data)
{
  m_net = net;

  const QVariantMap map       = data.toMap();
  const QVariantList authData = map.value(LS("a")).toList();

  if (!authData.isEmpty() && map.contains(LS("pin")))
    getToken(id, "pin", map.value("pin").toByteArray(), authData);
}


void ImgurUploader::upload(QNetworkAccessManager *net, UploadItemPtr item, const QVariant &data)
{
  m_net = net;

  const QVariantMap map       = data.toMap();
  const QVariantList authData = map.value(LS("a")).toList();

  if (authData.size() == 3) {
    m_clientId     = authData.at(0).toString();
    m_clientSecret = authData.at(1).toString();

    m_queue.enqueue(item);
    getToken(ChatId(ChatId::MessageId), "refresh_token", authData.value(2).toByteArray(), authData);
    return;
  }

  QNetworkReply *reply = 0;

  if (item->type() == ImageItem::Type) {
    ImageItem *i = uploaditem_cast<ImageItem*>(item.data());

    QHttpPart image;
    image.setHeader(QNetworkRequest::ContentTypeHeader, "image/png");
    image.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"image\"; filename=\"" + DateTime::toDateTime(item->date()).toString(LS("yyyy-MM-dd_hh-mm-ss")).toLatin1() + ".png\"");
    image.setBody(i->raw);

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    multiPart->append(image);

    if (!item->title().isEmpty()) {
      QHttpPart title;
      title.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"title\""));
      title.setBody(item->title().toUtf8());
      multiPart->append(title);
    }

    QNetworkRequest request(QUrl(LS("https://api.imgur.com/3/image")));
    request.setRawHeader("User-Agent", OsInfo::userAgent());

    if (authData.size() == 4)
      request.setRawHeader("Authorization", "Bearer " + authData.value(3).toByteArray());
    else
      request.setRawHeader("Authorization", "Client-ID " + authData.value(0).toByteArray());

    reply = net->post(request, multiPart);
    multiPart->setParent(reply);
  }

  prepare(reply, item->id());
}


void ImgurUploader::read(UploadResult &result, QNetworkReply *reply)
{
  if (result.status != 200)
    return;

  const QVariantMap json = JSON::parse(reply->readAll()).toMap().value(LS("data")).toMap();

  UploadResult::Image image;
  image.id         = json.value(LS("id")).toString();
  image.link       = json.value(LS("link")).toUrl();
  image.deletehash = json.value(LS("deletehash")).toString();
  image.title      = json.value(LS("title")).toString();
  image.height     = json.value(LS("height")).toInt();
  image.size       = json.value(LS("size")).toInt();
  image.width      = json.value(LS("width")).toInt();

  result.provider  = LS("imgur");
  result.json      = json;
  result.desc      = json.value(LS("description")).toString();
  result.images.append(image);
}


void ImgurUploader::tokenReady()
{
  QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
  if (!reply)
    return;

  const QVariantMap data = JSON::parse(reply->readAll()).toMap();
  if (reply->error() == QNetworkReply::NoError && !m_queue.isEmpty()) {
    QVariantMap map;
    QVariantList a;
    a.append(m_clientId);
    a.append(m_clientSecret);
    a.append(data.value(LS("refresh_token")));
    a.append(data.value(LS("access_token")));
    map.insert("a", a);

    while (!m_queue.isEmpty())
      upload(m_net, m_queue.dequeue(), map);
  }

  QVariantMap map;
  map.insert(LS("type"),  LS("token"));
  map.insert(LS("reply"), data);

  emit finished(reply->property("id").toByteArray(), LS("imgur"), map);
  reply->deleteLater();
}


void ImgurUploader::getToken(const ChatId &id, const QByteArray &grantType, const QByteArray &data, const QVariantList &authData)
{
  QNetworkRequest request(QUrl(LS("https://api.imgur.com/oauth2/token")));
  request.setHeader(QNetworkRequest::ContentTypeHeader, LS("application/x-www-form-urlencoded"));
  request.setRawHeader("User-Agent", OsInfo::userAgent());

  QByteArray body = "client_id="          + authData.value(0).toByteArray() +
                    "&client_secret="     + authData.value(1).toByteArray() +
                    "&grant_type="        + grantType +
                    "&" + grantType + "=" + data;

  QNetworkReply *reply = m_net->post(request, body);
  reply->setProperty("id", id.toByteArray());
  connect(reply, SIGNAL(finished()), SLOT(tokenReady()));
}
