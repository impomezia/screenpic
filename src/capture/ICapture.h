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

#ifndef ICAPTURE_H_
#define ICAPTURE_H_

#include <QPixmap>

class ICapture
{
public:
  virtual ~ICapture() {}
  virtual bool isReady() const = 0;
  virtual QPixmap capture(WId window, const QRect &rect, int screens, bool cursor = false) = 0;
};

#endif /* ICAPTURE_H_ */
