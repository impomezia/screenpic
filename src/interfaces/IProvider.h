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

#ifndef IPROVIDER_H_
#define IPROVIDER_H_

#include <QIcon>

class ChatId;
class IProviderListener;
class IScreenpic;
class QObject;
class Uploader;

class IProvider
{
public:
  enum Features {
    NoFeatures,
    DeleteImages
  };

  inline virtual ~IProvider() {}
  virtual int features() const                                          = 0;
  virtual int maxImages() const                                         = 0;
  virtual QIcon icon() const                                            = 0;
  virtual qint64 maxSize() const                                        = 0;
  virtual QString id() const                                            = 0;
  virtual QString name() const                                          = 0;
  virtual QVariant data() const                                         = 0;
  virtual QWidget *settingsWidget(QWidget *parent = 0)                  = 0;
  virtual void init(IScreenpic *screenpic, IProviderListener *listener) = 0;
};

Q_DECLARE_INTERFACE(IProvider, "io.rup.IProvider/1.1")


#endif /* IPROVIDER_H_ */
