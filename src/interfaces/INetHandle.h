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

#ifndef INETHANDLE_H_
#define INETHANDLE_H_

#include <QVariant>

class ChatId;
class IProviderListener;
class IScreenpic;
class Uploader;

class INetHandle
{
public:
  virtual ~INetHandle() {}
  virtual QString id() const                                            = 0;
  virtual Uploader *uploader(QObject *parent = 0) const                 = 0;
  virtual void handleReply(const ChatId &id, const QVariant &data)      = 0;
  virtual void init(IScreenpic *screenpic, IProviderListener *listener) = 0;
  virtual void networkReady()                                           = 0;
};

Q_DECLARE_INTERFACE(INetHandle, "io.rup.INetHandle/1.0")

#endif // INETHANDLE_H_
