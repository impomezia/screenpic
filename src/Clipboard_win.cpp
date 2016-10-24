/*   Copyright (C) 2013-2016 Alexander Sedov <imp@schat.me>
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

#include <QBuffer>
#include <qt_windows.h>

#include "Clipboard.h"


bool Clipboard::setPixmap(const QPixmap &pixmap)
{
  if (!OpenClipboard(NULL) || !EmptyClipboard()) {
    return false;
  }

  QByteArray ba;
  QBuffer buffer(&ba);
  buffer.open(QIODevice::WriteOnly);
  pixmap.toImage().save(&buffer, "BMP");

  const size_t len = ba.size() - sizeof(BITMAPFILEHEADER);
  HGLOBAL handle;

  if ((handle = GlobalAlloc(GMEM_MOVEABLE, len)) == NULL) {
    CloseClipboard();
    return false;
  }

  memcpy(GlobalLock(handle), ba.constData() + sizeof(BITMAPFILEHEADER), len);
  GlobalUnlock(handle);

  const bool result = SetClipboardData(CF_DIB, handle);

  CloseClipboard();
  GlobalFree(handle);

  return result;
}

