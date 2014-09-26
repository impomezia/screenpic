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

#ifndef STATISTICSPLUGIN_H_
#define STATISTICSPLUGIN_H_

#include <QObject>

#include "interfaces/IHook.h"
#include "interfaces/INetHandle.h"
#include "interfaces/IObserver.h"
#include "interfaces/IPlugin.h"

class StatisticsPlugin : public QObject, public IPlugin, public IObserver, public IHook, public INetHandle
{
  Q_OBJECT
  Q_INTERFACES(IPlugin)
  Q_INTERFACES(IObserver)
  Q_INTERFACES(IHook)
  Q_INTERFACES(INetHandle)
  Q_PLUGIN_METADATA(IID "io.rup.observer.statistics")

public:
  static const QString kCID;
  static const QString kStatistics;

  bool eventFilter(QObject *watched, QEvent *event);
  QString id() const override;
  Uploader *uploader(QObject *parent = 0) const override;
  void handleReply(const ChatId &id, const QVariant &data) override;
  void hitEvent(const QString &category, const QString &action, const QString &label, int value);
  void hookAboutPage(QVBoxLayout *layout, QWidget *parent) override;
  void hookUploadData(const QString &provider, QVariant &data) override;
  void init(IScreenpic *screenpic, IObserverListener *listener) override;
  void init(IScreenpic *screenpic, IProviderListener *listener) override;
  void networkReady() override;
  void watch(QWidget *widget) override;

private slots:
  void onAboutToQuit();
  void onStatisticsToggled(bool checked);

private:
  QByteArray commonData() const;
  QString cid() const;
  void hidden(QWidget *widget);
  void send(const QByteArray &body);
  void shown(QWidget *widget);

  void hitScreenView(QWidget *widget);

  IProviderListener *m_providerListener;
  IScreenpic *m_screenpic;
  mutable QString m_cid;
  QList<QWidget*> m_widgets;
  QString m_lastWidget;
};

#endif // STATISTICSPLUGIN_H_
