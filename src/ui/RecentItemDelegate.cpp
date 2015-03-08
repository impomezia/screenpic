/*   Copyright (C) 2013-2015 Alexander Sedov <imp@schat.me>
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

#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>

#include "AppCore.h"
#include "Providers.h"
#include "RecentItem.h"
#include "RecentItemDelegate.h"
#include "RecentModel.h"
#include "sglobal.h"
#include "TimeAgo.h"

RecentItemDelegate::RecentItemDelegate(AppCore *core, QObject *parent)
  : QStyledItemDelegate(parent)
  , m_core(core)
  , m_column(RecentModel::ThumbColumn)
  , m_link(LS(":/images/link.png"))
  , m_menuActive(false)
{
}


bool RecentItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
  if (event->type() == QEvent::MouseMove)
    m_column = index.column();

  RecentItem *item = static_cast<RecentItem*>(index.internalPointer());

  if (event->type() == QEvent::MouseButtonRelease) {
    QMouseEvent *e      = static_cast<QMouseEvent*>(event);
    IProvider *provider = m_core->providers()->get(item->provider);

    if (e->button() == Qt::LeftButton && index.column() == RecentModel::InfoColumn && item->link.isValid() && item->link.scheme() != LS("file")) {
      copyLink(item->link);
      return true;
    }

    if (e->button() == Qt::RightButton) {
      m_menuActive = true;
      QAction *copyLinkAction = 0;
      QAction *openLinkAction = 0;
      QAction *deleteAction   = 0;

      QMenu menu;
      if (item->link.isValid() && item->link.scheme() != LS("file")) {
        copyLinkAction = menu.addAction(QIcon(LS(":/images/copy_sm.png")), tr("Copy link"));
        openLinkAction = menu.addAction(provider ? provider->icon() : QIcon(LS(":/images/globe_gray.png")), tr("Open link"));
      }

      menu.addSeparator();

      QAction *hideAction = menu.addAction(QIcon(LS(":/images/times.png")), tr("Hide"));

      if (provider && provider->features() & IProvider::DeleteImages)
        deleteAction = menu.addAction(QIcon(LS(":/images/trash.png")), tr("Delete"));

      QAction *action = menu.exec(e->globalPos());
      m_menuActive = false;

      if (action) {
        if (action == copyLinkAction)
          copyLink(item->link);
        else if (action == openLinkAction)
          openLink(item->link);
        else if (action == hideAction)
          hide(qobject_cast<RecentModel*>(model), index);
        else if (action == deleteAction)
          remove(qobject_cast<RecentModel*>(model), index, item);
      }

      return true;
    }

  }

  if (event->type() == QEvent::MouseButtonDblClick && index.column() == RecentModel::ThumbColumn && static_cast<QMouseEvent*>(event)->button() == Qt::LeftButton) {
    QDesktopServices::openUrl(item->link);
    emit closeRequest();
    return true;
  }

  return QStyledItemDelegate::editorEvent(event, model, option, index);
}


QSize RecentItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  if (index.column() == RecentModel::ThumbColumn)
    return QSize(40, 40);

  return QStyledItemDelegate::sizeHint(option, index);
}


void RecentItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QStyleOptionViewItemV4 opt = option;
  opt.rect = adjustRect(opt.rect, index.column());

  QStyledItemDelegate::paint(painter, opt, index);

  if (index.column() == RecentModel::InfoColumn) {
    const RecentItem *item = static_cast<RecentItem*>(index.internalPointer());
    if (!m_menuActive && option.state & QStyle::State_MouseOver && m_column == RecentModel::InfoColumn && item->udate && item->link.scheme() != LS("file"))
      return drawCopyLink(painter, opt);

    QRect boundingRect;
    drawLine1(painter, opt, item, &boundingRect);
    drawLine2(painter, opt, item, boundingRect.height() + 6);
  }
}


QIcon RecentItemDelegate::providerIcon(const QString &id) const
{
  IProvider *provider = m_core->providers()->get(id);
  if (provider)
    return provider->icon();

  return QIcon(LS(":/images/globe_gray.png"));
}


QRect RecentItemDelegate::adjustRect(const QRect &rect, int column) const
{
#ifdef Q_OS_MAC
  const int margin = 1;
#else
  const int margin = 3;
#endif

  QRect out = rect;
  out.setTop(rect.top() + margin);
  out.setHeight(rect.height() - margin * 2);

  if (column == RecentModel::ThumbColumn)
    out.setLeft(rect.left() + margin);
  else
    out.setRight(rect.right() - margin);

  return out;
}


QString RecentItemDelegate::bytesToHuman(qint64 size) const
{
  if (size < 1024)
    return tr("%n bytes", "", size);

  if (size < 1024 * 1024)
    return tr("%1 kB").arg((double)size / 1024, 0, 'f', 0);

  return tr("%1 MB").arg((double)size / (1024 * 1024), 0, 'f', 2);;
}


void RecentItemDelegate::copyLink(const QUrl &url)
{
  QApplication::clipboard()->setText(url.toString());
  emit linkCopied(url);

  QTimer::singleShot(40, this, SIGNAL(closeRequest()));
}


/*!
 * Рисование ссылки для копирования.
 */
void RecentItemDelegate::drawCopyLink(QPainter *painter, const QStyleOptionViewItemV4 &option) const
{
  QFont font = painter->font();
  font.setBold(true);

  QRect rect = option.rect;
  rect.setLeft(rect.left() + 19);

  painter->save();
  painter->setFont(font);
  painter->setPen(0xff0066cc);
  painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, tr("Copy link"));
  painter->drawPixmap(option.rect.left(), option.rect.top() + (option.rect.height() / 2 - 8), m_link);
  painter->restore();
}


/*!
 * Рисование первой сторки.
 */
void RecentItemDelegate::drawLine1(QPainter *painter, const QStyleOptionViewItemV4 &option, const RecentItem *item, QRect *boundingRect) const
{
  QRect rect = option.rect;
  rect.setTop(rect.top() + 3);

  painter->save();

  if (!item->udate && item->percent >= 0) {
    rect.setHeight(13);
    *boundingRect = rect;

    QStyleOptionProgressBarV2 pbo;
    pbo.rect        = rect;
    pbo.state       = option.state;
    pbo.minimum     = 0;
    pbo.maximum     = 100;
    pbo.progress    = item->percent;
    pbo.text        = QString::number(item->percent) + "%";
    pbo.textVisible = true;

    QApplication::style()->drawControl(QStyle::CE_ProgressBar, &pbo, painter);
    painter->restore();
    return;
  }

  QFont font = painter->font();
  font.setBold(true);

  if (item->percent < 0)
    painter->setPen(0xffd60808);

  painter->setFont(font);
  painter->drawText(rect, Qt::AlignLeft | Qt::AlignTop, item->percent >= 0 ? TimeAgo::toString(item->udate) : tr("Upload error: %1").arg(qAbs(item->percent)), boundingRect);
  painter->restore();
}


/*!
 * Рисование второй сторки.
 */
void RecentItemDelegate::drawLine2(QPainter *painter, const QStyleOptionViewItemV4 &option, const RecentItem *item, int offset) const
{
  QRect rect = option.rect;
  rect.setTop(rect.top() + offset);

  painter->save();
  painter->setPen(0xff969696);
  painter->drawText(rect, Qt::AlignLeft | Qt::AlignTop, QString(LS("%1x%2")).arg(item->width).arg(item->height));

  rect.setLeft(rect.left() + 64);
  painter->drawText(rect, Qt::AlignLeft | Qt::AlignTop, bytesToHuman(item->size));
  painter->restore();
}


void RecentItemDelegate::hide(RecentModel *model, const QModelIndex &index)
{
  Q_ASSERT(model);
  if (!model)
    return;

  model->remove(index);
}


void RecentItemDelegate::openLink(const QUrl &url)
{
  QDesktopServices::openUrl(url);

  QTimer::singleShot(40, this, SIGNAL(closeRequest()));
}


void RecentItemDelegate::remove(RecentModel *model, const QModelIndex &index, RecentItem *item)
{
  m_core->remove(item->deletehash, item->provider);
  hide(model, index);
}
