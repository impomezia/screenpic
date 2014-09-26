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

#ifndef IPROVIDERLISTENER_H_
#define IPROVIDERLISTENER_H_

#include "id/ChatId.h"

class IProviderListener
{
public:
  virtual ~IProviderListener() {}
  virtual void onCustomRequest(const ChatId &id, const QString &provider, const QVariant &data) = 0;
};

#endif // IPROVIDERLISTENER_H_
