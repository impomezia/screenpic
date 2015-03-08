/* Copyright (c) 2008-2015 Alexander Sedov <imp@schat.me>
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

#ifndef GEEKPICUPLOADER_H_
#define GEEKPICUPLOADER_H_

#include "uploaders/Uploader.h"

class GeekpicUploader : public Uploader
{
  Q_OBJECT

public:
  GeekpicUploader(QObject *parent = 0);
  void request(QNetworkAccessManager *net, const ChatId &id, const QVariant &data) override;
  void upload(QNetworkAccessManager *net, UploadItemPtr item, const QVariant &data) override;

protected:
  void read(UploadResult &result, QNetworkReply *reply) override;
};

#endif /* GEEKPICUPLOADER_H_ */
