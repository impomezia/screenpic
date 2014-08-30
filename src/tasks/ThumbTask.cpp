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

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>

#include "sglobal.h"
#include "ShareHistoryDB.h"
#include "tasks/ThumbTask.h"

ThumbTask::ThumbTask(ShareHistoryDB *db, const QList<qint64> &ids)
  : QObject()
  , QRunnable()
  , m_ids(ids)
  , m_db(db)
{
}


void ThumbTask::run()
{
  if (m_ids.isEmpty())
    return;

  QMap<qint64, QImage> out;

  QSqlQuery query(QSqlDatabase::database(m_db->id()));
  query.prepare(LS("SELECT id, thumbnail FROM uploads WHERE id >= :id1 AND id <= :id2;"));
  query.bindValue(LS(":id1"), m_ids.first());
  query.bindValue(LS(":id2"), m_ids.last());
  query.exec();

  while (query.next())
    out.insert(query.value(0).toLongLong(), QImage::fromData(query.value(1).toByteArray(), "PNG"));

  if (out.isEmpty())
    return;

  emit ready(out);
}
