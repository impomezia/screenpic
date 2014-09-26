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

#include <QApplication>
#include <QCheckBox>
#include <QDesktopWidget>
#include <QShowEvent>
#include <QUrl>
#include <QUuid>
#include <QVBoxLayout>

#include "interfaces/IProviderListener.h"
#include "interfaces/IScreenpic.h"
#include "interfaces/ISettings.h"
#include "interfaces/ITranslation.h"
#include "sglobal.h"
#include "Statistics.h"
#include "StatisticsPlugin.h"
#include "StatisticsUploader.h"

const QString StatisticsPlugin::kCID        = LS("CID");
const QString StatisticsPlugin::kStatistics = LS("Statistics");

bool StatisticsPlugin::eventFilter(QObject *watched, QEvent *event)
{
  const QEvent::Type type = event->type();

  if (type == QEvent::Show || type == QEvent::WindowActivate) {
    shown(qobject_cast<QWidget*>(watched));
  }

  if (type == QEvent::Hide) {
    hidden(qobject_cast<QWidget*>(watched));
  }

  return QObject::eventFilter(watched, event);
}


QString StatisticsPlugin::id() const
{
  return LS("statistics");
}


Uploader *StatisticsPlugin::uploader(QObject *parent) const
{
  return new StatisticsUploader(parent);
}


void StatisticsPlugin::handleReply(const ChatId &id, const QVariant &data)
{
  Q_UNUSED(id)
  Q_UNUSED(data)
}


void StatisticsPlugin::hitEvent(const QString &category, const QString &action, const QString &label, int value)
{
  QByteArray body = commonData() + "&t=event&ec=" + QUrl::toPercentEncoding(category) + "&ea=" + QUrl::toPercentEncoding(action);

  if (!label.isEmpty())
    body += "&el=" + QUrl::toPercentEncoding(label);

  if (value >= 0)
    body += "&ev=" + QByteArray::number(value);

  send(body);
}


void StatisticsPlugin::hookAboutPage(QVBoxLayout *layout, QWidget *parent)
{
  QCheckBox *checkBox = new QCheckBox(tr("Send anonymous usage data"), parent);
  checkBox->setChecked(m_screenpic->settings()->value(kStatistics).toBool());

  connect(checkBox, SIGNAL(toggled(bool)), SLOT(onStatisticsToggled(bool)));

  layout->addWidget(checkBox);
}


void StatisticsPlugin::hookUploadData(const QString &provider, QVariant &data)
{
  if (provider != LS("geekpic"))
    return;

  const QString cid = this->cid();
  if (cid.isEmpty())
    return;

  QVariantMap map = data.toMap();
  map.insert(LS("cid"), cid);

  data = map;
}


void StatisticsPlugin::init(IScreenpic *screenpic, IObserverListener *listener)
{
  Q_UNUSED(listener)

  m_screenpic = screenpic;
}


void StatisticsPlugin::init(IScreenpic *screenpic, IProviderListener *listener)
{
  m_screenpic = screenpic;
  m_screenpic->settings()->setDefault(kStatistics, true);

  m_providerListener = listener;

  connect(QApplication::instance(), SIGNAL(aboutToQuit()), SLOT(onAboutToQuit()));
}


void StatisticsPlugin::networkReady()
{
  send(commonData() + "&t=screenview&cd=app&sc=start");
}


void StatisticsPlugin::watch(QWidget *widget)
{
  widget->installEventFilter(this);
}


void StatisticsPlugin::onAboutToQuit()
{
  send(commonData() + "&t=screenview&cd=app&sc=end");
}


void StatisticsPlugin::onStatisticsToggled(bool checked)
{
  if (!checked)
    m_cid.clear();

  m_screenpic->settings()->setValue(kStatistics, checked);
}


QByteArray StatisticsPlugin::commonData() const
{
  const QRect sr = QApplication::desktop()->screenGeometry();
  QByteArray body;

  body += "v=1";
  body += "&tid=" + QByteArray(MP_ID);
  body += "&cid=" + cid().toLatin1();
  body += "&an="  + QApplication::applicationName().toLatin1();
  body += "&av="  + QApplication::applicationVersion().toLatin1();
  body += "&sr="  + QString("%1x%2").arg(sr.width()).arg(sr.height()).toLatin1();
  body += "&ul="  + m_screenpic->translation()->name().toLower().replace(LC('_'), LC('-'));
  body += "&fl="  + QByteArray(qVersion());

  return body;
}


QString StatisticsPlugin::cid() const
{
  if (!m_screenpic->settings()->value(kStatistics).toBool())
    return QString();

  if (m_cid.isEmpty()) {
    QUuid uuid = QUuid::fromRfc4122(QByteArray::fromBase64(m_screenpic->settings()->value(kCID).toString().toLatin1()));
    if (uuid.isNull())
      uuid = QUuid::createUuid();

    m_cid = uuid.toString().remove(0, 1).remove(36, 1);
    m_screenpic->settings()->setValue(kCID, QString(uuid.toRfc4122().toBase64()).remove(LC('=')));
  }

  return m_cid;
}


void StatisticsPlugin::hidden(QWidget *widget)
{
  m_widgets.removeAll(widget);

  if (m_widgets.isEmpty())
    return hitScreenView(0);
}


void StatisticsPlugin::send(const QByteArray &body)
{
  if (cid().isEmpty())
    return;

  m_providerListener->onCustomRequest(ChatId(ChatId::MessageId), id(), body);
}


void StatisticsPlugin::shown(QWidget *widget)
{
  if (!m_widgets.contains(widget))
    m_widgets.append(widget);

  hitScreenView(widget);
}


void StatisticsPlugin::hitScreenView(QWidget *widget)
{
  const QString name = widget ? widget->objectName() : LS("app");

  if (name == LS("SettingsDialog") || name == m_lastWidget)
    return;

  m_lastWidget = name;
  send(commonData() + "&t=screenview&cd=" + name.toLatin1());
}

Q_EXPORT_PLUGIN2(Statistics, StatisticsPlugin);
