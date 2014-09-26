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

#ifndef IHOOK_H_
#define IHOOK_H_

#include <QVariant>

class QVBoxLayout;
class QWidget;

class IHook
{
public:
  virtual ~IHook() {}
  virtual void hookAboutPage(QVBoxLayout *layout, QWidget *parent) = 0;
  virtual void hookUploadData(const QString &provider, QVariant &data) = 0;
};

Q_DECLARE_INTERFACE(IHook, "io.rup.IHook/1.0")

#endif // IHOOK_H_
