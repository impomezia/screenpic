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

#include <QBasicTimer>
#include <QDir>
#include <QTimerEvent>

#include "data/ImageItem.h"
#include "DateTime.h"
#include "RecentItem.h"
#include "RecentModel.h"
#include "sglobal.h"
#include "ShareHistoryDB.h"
#include "tasks/ThumbTask.h"
#include "uploaders/UploadResult.h"

RecentModel::RecentModel(ShareHistoryDB *db, QObject *parent)
  : QAbstractItemModel(parent)
  , m_icon(LS(":/images/camera.png"))
  , m_db(db)
{
  m_timer = new QBasicTimer();
  m_timer->start(30000, this);

  qRegisterMetaType<QMap<qint64,QImage> >("QMap<qint64,QImage>");
}


RecentModel::~RecentModel()
{
  delete m_timer;
  qDeleteAll(m_items);
}


bool RecentModel::canFetchMore(const QModelIndex &parent) const
{
  Q_UNUSED(parent)

  return false;
}


int RecentModel::columnCount(const QModelIndex &parent) const
{
  if (!parent.isValid())
    return ColumnCount;

  return 0;
}


int RecentModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;

  return m_items.size();
}


QModelIndex RecentModel::index(int row, int column, const QModelIndex &parent) const
{
  if (!hasIndex(row, column, parent) || row >= m_items.size())
    return QModelIndex();

  return createIndex(row, column, m_items.at(row));
}


QModelIndex RecentModel::parent(const QModelIndex &index) const
{
  Q_UNUSED(index)

  return QModelIndex();
}


QVariant RecentModel::data(const QModelIndex &index, int role) const
{
  RecentItem *item = static_cast<RecentItem*>(index.internalPointer());

  if (index.column() == ThumbColumn && role == Qt::DecorationRole)
    return item->thumbnail.isNull() ? m_icon : item->thumbnail;

  if (role == Qt::ToolTipRole) {
    if (item->link.scheme() == LS("file"))
      return QDir::toNativeSeparators(item->link.toLocalFile());

    return item->link.toString();
  }

  return QVariant();
}


void RecentModel::fetchMore(const QModelIndex &parent)
{
  Q_UNUSED(parent)
}


void RecentModel::add(UploadItemPtr item)
{
  RecentItem *i = new RecentItem();
  i->cdate      = item->date();
  i->thumbnail  = QPixmap::fromImage(item->thumbnail().image);

  if (item->type() == ImageItem::Type) {
    const ImageItem *imageItem = uploaditem_cast<ImageItem*>(item.data());
    i->width  = imageItem->image.width();
    i->height = imageItem->image.height();
    i->size   = imageItem->raw.size();
  }

  m_ids.insert(item->id(), i);

  beginInsertRows(QModelIndex(), 0, 0);
  m_items.prepend(i);
  endInsertRows();
}


void RecentModel::finished(const UploadResult &result)
{
  RecentItem *item = m_ids.value(result.id);
  if (!item)
    return;

  m_ids.remove(result.id);

  if (result.status == 200 && !result.images.isEmpty()) {
    item->udate    = DateTime::utc();
    item->provider = result.provider;

    const UploadResult::Image &image = result.images[0];
    item->width      = image.width;
    item->height     = image.height;
    item->size       = image.size;
    item->deletehash = image.deletehash;
    item->link       = image.link;
  }
  else
    item->percent = -result.status;

  const QModelIndex index = this->index(m_items.indexOf(item), 1);
  if (index.isValid())
    emit dataChanged(index, index);
}


void RecentModel::init()
{
  m_items = m_db->recent(1000);
  foreach (RecentItem *item, m_items)
    m_map.insert(item->id, item);

  ThumbTask *task = new ThumbTask(m_db, m_map.keys());
  connect(task, SIGNAL(ready(QMap<qint64,QImage>)), SLOT(onThumbsReady(QMap<qint64,QImage>)));

  emit taskCreated(task);
}


void RecentModel::progress(const ChatId &id, int percent)
{
  RecentItem *item = m_ids.value(id);
  if (!item)
    return;

  item->percent = percent;

  const QModelIndex index = this->index(m_items.indexOf(item), 1);
  if (index.isValid())
    emit dataChanged(index, index);
}


void RecentModel::timerEvent(QTimerEvent *event)
{
  if (m_timer->timerId() == event->timerId())
    emit dataChanged(QModelIndex(), QModelIndex());

  QAbstractItemModel::timerEvent(event);
}


void RecentModel::onThumbsReady(const QMap<qint64, QImage> &thumbs)
{
  QMapIterator<qint64, QImage> i(thumbs);
  while (i.hasNext()) {
    i.next();
    RecentItem *item = m_map.value(i.key());
    if (item)
      item->thumbnail = QPixmap::fromImage(i.value());
  }

  emit dataChanged(QModelIndex(), QModelIndex());
}
