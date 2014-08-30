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

#ifndef SHAREDB_H_
#define SHAREDB_H_

#include <QObject>

class RecentItem;
class UploadResult;

class ShareHistoryDB : public QObject
{
  Q_OBJECT

public:
  ShareHistoryDB(QObject *parent = 0);
  inline const QString &id() const { return m_id; }
  int open(const QString &fileName);
  qint64 add(const UploadResult &result, qint64 cdate, const QString &file = QString(), const QByteArray &thumbnail = QByteArray());
  QList<RecentItem*> recent(int limit) const;

private:
  void version();

  const QString m_id;
};

#endif /* SHAREDB_H_ */
