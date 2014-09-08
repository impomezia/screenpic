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

#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>

#include "DateTime.h"
#include "RecentItem.h"
#include "sglobal.h"
#include "ShareHistoryDB.h"
#include "uploaders/UploadResult.h"

ShareHistoryDB::ShareHistoryDB(QObject *parent)
  : QObject(parent)
  , m_id(LS("history"))
{
}


int ShareHistoryDB::open(const QString &fileName)
{
  QDir dir(QFileInfo(fileName).absolutePath());
  if (!dir.exists())
    dir.mkpath(dir.absolutePath());

  QSqlDatabase db = QSqlDatabase::addDatabase(LS("QSQLITE"), m_id);
  db.setDatabaseName(fileName);
  if (!db.open())
    return 1;

  QSqlQuery query(QSqlDatabase::database(m_id));
  query.exec(LS("PRAGMA synchronous = OFF"));

  query.exec(LS(
    "CREATE TABLE IF NOT EXISTS uploads ( "
    "  id         INTEGER PRIMARY KEY,"
    "  pid        TEXT,"
    "  cdate      INTEGER DEFAULT ( 0 ),"
    "  udate      INTEGER DEFAULT ( 0 ),"
    "  type       INTEGER DEFAULT ( 0 ),"
    "  width      INTEGER DEFAULT ( 0 ),"
    "  height     INTEGER DEFAULT ( 0 ),"
    "  size       INTEGER DEFAULT ( 0 ),"
    "  provider   TEXT,"
    "  ext        TEXT,"
    "  file       TEXT,"
    "  link       TEXT,"
    "  deletehash TEXT,"
    "  thumbnail  BLOB,"
    "  data       BLOB"
    ");"
  ));

  return 0;
}


qint64 ShareHistoryDB::add(const UploadResult &result, qint64 cdate, const QString &file, const QByteArray &thumbnail)
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.prepare(LS("INSERT INTO uploads (pid, cdate, udate, width, height, size, provider, ext, file, link, deletehash, thumbnail) "
                     "VALUES (:pid, :cdate, :udate, :width, :height, :size, :provider, :ext, :file, :link, :deletehash, :thumbnail);"));

  foreach (const UploadResult::Image &image, result.images) {
    query.bindValue(LS(":pid"),        image.id);
    query.bindValue(LS(":cdate"),      cdate);
    query.bindValue(LS(":udate"),      DateTime::utc());
    query.bindValue(LS(":width"),      image.width);
    query.bindValue(LS(":height"),     image.height);
    query.bindValue(LS(":size"),       image.size);
    query.bindValue(LS(":provider"),   result.provider);
    query.bindValue(LS(":ext"),        image.type);
    query.bindValue(LS(":file"),       file);
    query.bindValue(LS(":link"),       image.link.toString());
    query.bindValue(LS(":deletehash"), image.deletehash);
    query.bindValue(LS(":thumbnail"),  thumbnail);
    query.exec();
  }

  return 0;
}


QList<RecentItem*> ShareHistoryDB::recent(int limit) const
{
  QList<RecentItem*> items;

  QSqlQuery query(QSqlDatabase::database(m_id));
  query.prepare(QString(LS("SELECT id, pid, cdate, udate, width, height, size, provider, ext, file, link, deletehash FROM uploads ORDER BY id DESC LIMIT %1;")).arg(limit));
  query.exec();

  while (query.next()) {
    RecentItem *item = new RecentItem();
    item->id         = query.value(0).toLongLong();
    item->pid        = query.value(1).toString();
    item->cdate      = query.value(2).toLongLong();
    item->udate      = query.value(3).toLongLong();
    item->width      = query.value(4).toInt();
    item->height     = query.value(5).toInt();
    item->size       = query.value(6).toInt();
    item->provider   = query.value(7).toString();
    item->ext        = query.value(8).toString();
    item->file       = query.value(9).toString();
    item->link       = query.value(10).toUrl();
    item->deletehash = query.value(11).toString();
    item->percent    = 100;

    items.append(item);
  }

  return items;
}


void ShareHistoryDB::version()
{
  QSqlQuery query(QSqlDatabase::database(m_id));

  query.exec(LS("PRAGMA user_version;"));
  if (!query.first())
    return;

  qint64 version = query.value(0).toLongLong();
  if (!version) {
    query.exec(LS("PRAGMA user_version = 1;"));
    version = 1;
  }

  query.finish();
}
