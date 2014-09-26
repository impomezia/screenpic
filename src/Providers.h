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

#ifndef PROVIDERS_H_
#define PROVIDERS_H_

#include <QMap>
#include <QObject>
#include <QStringList>

#include "interfaces/IProvider.h"

class INetHandle;
class IProvider;
class Uploader;

class Providers : public QObject
{
  Q_OBJECT

public:
  Providers(IScreenpic *screenpic, IProviderListener *listener, const QObjectList &list, QObject *parent = 0);
  inline bool contains(const QString &id) const { return m_providers.contains(id); }
  inline const QString& currentId() const       { return m_currentId; }
  inline const QStringList& list() const        { return m_list; }
  IProvider *current() const;
  IProvider *get(const QString &id) const;
  void create(QMap<QString, Uploader*> &map, QObject *parent = 0);
  void networkReady();
  void setCurrentId(const QString &id);

signals:
  void currentChanged(IProvider *provider);

public slots:
  void handleReply(const ChatId &id, const QString &provider, const QVariant &data);

private:
  bool add(IProvider *provider);

  QMap<QString, INetHandle*> m_handlers;
  QMap<QString, IProvider*> m_providers; ///< Провайдеры.
  QString m_currentId;                   ///< Идентификатор текущего провайдера.
  QStringList m_list;                    ///< Список провайдеров.
};

#endif /* UPLOADPROVIDERS_H_ */
