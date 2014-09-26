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

#include "interfaces/IProvider.h"
#include "interfaces/INetHandle.h"
#include "Providers.h"
#include "sglobal.h"

Providers::Providers(IScreenpic *screenpic, IProviderListener *listener, const QObjectList &list, QObject *parent)
  : QObject(parent)
{
  for (int i = 0; i < list.size(); ++i) {
    IProvider *provider = qobject_cast<IProvider*>(list.at(i));
    INetHandle *handle  = qobject_cast<INetHandle*>(list.at(i));

    if (provider) {
      provider->init(screenpic, listener);
      if (!add(provider))
        continue;
    }
    else
      handle->init(screenpic, listener);

    m_handlers.insert(handle->id(), handle);
  }
}


IProvider *Providers::current() const
{
  IProvider *provider = m_providers.value(currentId());
  Q_ASSERT(provider);

  return provider;
}


IProvider *Providers::get(const QString &id) const
{
  IProvider *provider = m_providers.value(id);
  Q_ASSERT(provider);

  return provider;
}


void Providers::create(QMap<QString, Uploader*> &map, QObject *parent)
{
  QMapIterator<QString, INetHandle*> i(m_handlers);
  while (i.hasNext()) {
    i.next();

    map.insert(i.key(), i.value()->uploader(parent));
  }
}


void Providers::networkReady()
{
  foreach (INetHandle *handle, m_handlers) {
    handle->networkReady();
  }
}


void Providers::setCurrentId(const QString &id)
{
  if (!m_providers.contains(id) || m_currentId == id)
    return;

  m_currentId = id;

  emit currentChanged(m_providers.value(id));
}


void Providers::handleReply(const ChatId &id, const QString &provider, const QVariant &data)
{
  if (m_handlers.contains(provider))
    m_handlers.value(provider)->handleReply(id, data);
}


bool Providers::add(IProvider *provider)
{
  Q_ASSERT(provider);
  if (!provider)
    return false;

  const QString id = provider->id();

  if (m_providers.contains(id)) {
    delete provider;
    return false;
  }

  if (m_list.isEmpty())
    m_currentId = id;

  m_providers.insert(id, provider);
  m_list.append(id);
  return true;
}
