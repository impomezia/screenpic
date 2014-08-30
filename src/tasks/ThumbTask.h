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

#ifndef THUMBTASK_H_
#define THUMBTASK_H_

#include <QImage>
#include <QMap>
#include <QObject>
#include <QRunnable>

class ShareHistoryDB;

class ThumbTask : public QObject, public QRunnable
{
  Q_OBJECT

public:
  ThumbTask(ShareHistoryDB *db, const QList<qint64> &ids);
  virtual void run();

signals:
  void ready(const QMap<qint64, QImage> &thumbs);

private:
  QList<qint64> m_ids;
  ShareHistoryDB *m_db;
};

#endif /* THUMBTASK_H_ */
