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

#ifndef RECENTMODEL_H_
#define RECENTMODEL_H_

#include <QAbstractItemModel>
#include <QPixmap>

#include "data/UploadItem.h"

class QBasicTimer;
class QRunnable;
class RecentItem;
class ShareHistoryDB;
class UploadResult;

class RecentModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  enum Columns
  {
    ThumbColumn,
    InfoColumn,
    ColumnCount
  };

  RecentModel(ShareHistoryDB *db, QObject *parent = 0);
  ~RecentModel();
  virtual bool canFetchMore(const QModelIndex &parent) const;
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
  virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
  virtual QModelIndex parent(const QModelIndex &index) const;
  virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  virtual void fetchMore(const QModelIndex &parent);
  void add(UploadItemPtr item);
  void finished(const UploadResult &result);
  void init();
  void progress(const ChatId &id, int percent);

signals:
  void taskCreated(QRunnable *task);

protected:
  virtual void timerEvent(QTimerEvent *event);

private slots:
  void onThumbsReady(const QMap<qint64, QImage> &thumbs);

private:
  QBasicTimer *m_timer;             ///< Таймер для периодического обновления списка.
  QList<RecentItem*> m_items;       ///< Информация о загруженных файлах.
  QMap<ChatId, RecentItem*> m_ids;  ///< Информация о текущих загрузках.
  QMap<qint64, RecentItem*> m_map;  ///< Карта соответствия загрузок и идентификаторов в базе.
  QPixmap m_icon;                   ///< Иконка по умолчанию.
  ShareHistoryDB *m_db;             ///< База данных.
};

#endif /* RECENTMODEL_H_ */
