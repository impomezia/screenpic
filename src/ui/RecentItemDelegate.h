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

#ifndef RECENTITEMDELEGATE_H_
#define RECENTITEMDELEGATE_H_

#include <QStyledItemDelegate>

class AppCore;
class RecentItem;
class RecentModel;

class RecentItemDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  RecentItemDelegate(AppCore *core, QObject *parent = 0);
  bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:
  void closeRequest();
  void linkCopied(const QUrl &url);

private:
  QIcon providerIcon(const QString &id) const;
  QRect adjustRect(const QRect &rect, int column) const;
  QString bytesToHuman(qint64 size) const;
  void copyLink(const QUrl &url);
  void drawCopyLink(QPainter *painter, const QStyleOptionViewItemV4 &option) const;
  void drawLine1(QPainter *painter, const QStyleOptionViewItemV4 &option, const RecentItem *item, QRect *boundingRect) const;
  void drawLine2(QPainter *painter, const QStyleOptionViewItemV4 &option, const RecentItem *item, int offset) const;
  void hide(RecentModel *model, const QModelIndex &index);
  void openLink(const QUrl &url);
  void remove(RecentModel *model, const QModelIndex &index, RecentItem *item);

  AppCore *m_core;
  bool m_menuActive;
  int m_column;
  QPixmap m_link;
};

#endif /* RECENTITEMDELEGATE_H_ */
