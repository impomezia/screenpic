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

#include <QDebug>

#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "sglobal.h"
#include "StatisticsUploader.h"
#include "tools/OsInfo.h"
#include "uploaders/UploadResult.h"

StatisticsUploader::StatisticsUploader(QObject *parent)
  : Uploader(parent)
{
}


void StatisticsUploader::request(QNetworkAccessManager *net, const ChatId &id, const QVariant &data)
{
  Q_UNUSED(id)

  QNetworkRequest request(QUrl(LS("https://www.google-analytics.com/collect")));
  request.setHeader(QNetworkRequest::ContentTypeHeader, LS("application/x-www-form-urlencoded"));
  request.setRawHeader("User-Agent", OsInfo::userAgent());

  QNetworkReply *reply = net->post(request, data.toByteArray());
  connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
}


void StatisticsUploader::upload(QNetworkAccessManager *net, UploadItemPtr item, const QVariant &data)
{
  Q_UNUSED(net)
  Q_UNUSED(item)
  Q_UNUSED(data)
}


void StatisticsUploader::read(UploadResult &result, QNetworkReply *reply)
{
  Q_UNUSED(result)
  Q_UNUSED(reply)
}
