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

#ifndef IMGURPROVIDER_H_
#define IMGURPROVIDER_H_

#include <QObject>

#include "interfaces/IPlugin.h"
#include "interfaces/IProvider.h"

class ImgurProviderPlugin : public QObject, IPlugin, IProvider
{
  Q_OBJECT
  Q_INTERFACES(IPlugin)
  Q_INTERFACES(IProvider)
  Q_PLUGIN_METADATA(IID "io.rup.provider.imgur")

public:
  int maxImages() const override;
  QIcon icon() const override;
  qint64 maxSize() const override;
  QString id() const override;
  QString name() const override;
  QVariant data() const override;
  QWidget *settingsWidget(QWidget *parent = 0) override;
  Uploader *uploader(QObject *parent = 0) const override;
  void init(ISettings *settings) override;

private slots:
  void onPinReady(const QString &pin);
  void onPinRequest();

private:
  QString m_clientId;
  QString m_clientSecret;
};

#endif /* IMGURPROVIDER_H_ */
