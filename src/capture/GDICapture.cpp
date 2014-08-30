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

#include <qt_windows.h>

#include "GDICapture.h"

QPoint screenToClient(int x, int y)
{
  QPoint point;
  point.setX(x - GetSystemMetrics(SM_XVIRTUALSCREEN));
  point.setY(y - GetSystemMetrics(SM_YVIRTUALSCREEN));

  return point;
}


void drawMouse(HDC hdc)
{
  CURSORINFO ci;
  ZeroMemory(&ci, sizeof(ci));
  ci.cbSize = sizeof(ci);

  if (!GetCursorInfo(&ci) || ci.flags != CURSOR_SHOWING)
    return;

  HICON icon = CopyIcon(ci.hCursor);
  if (!icon)
    return;

  ICONINFO ii;
  ZeroMemory(&ii, sizeof(ii));

  if (GetIconInfo(icon, &ii)) {
    const QPoint point = screenToClient(ci.ptScreenPos.x, ci.ptScreenPos.y);

    DrawIconEx(hdc, point.x() - ii.xHotspot, point.y() - ii.yHotspot, icon, 0, 0, 0, NULL, DI_NORMAL);

    if (ii.hbmMask)
      DeleteObject(ii.hbmMask);

    if (ii.hbmColor)
      DeleteObject(ii.hbmColor);
  }

  DestroyIcon(icon);
}


static QImage imageFromWinHBITMAP(HDC hdc, HBITMAP bitmap, int w, int h, bool alpha)
{
  BITMAPINFO bmi;
  ZeroMemory(&bmi, sizeof(bmi));

  bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth       = w;
  bmi.bmiHeader.biHeight      = -h;
  bmi.bmiHeader.biPlanes      = 1;
  bmi.bmiHeader.biBitCount    = 32;
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biSizeImage   = w * h * 4;

  QImage image(w, h, alpha ? QImage::Format_ARGB32_Premultiplied : QImage::Format_RGB32);

  if (image.isNull())
    return image;

  int i = h;
  for (int y = 0; y < h; ++y) {
    --i;

    QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(y));

    if (!GetDIBits(hdc, bitmap, i, 1, reinterpret_cast<void *>(line), &bmi, DIB_RGB_COLORS))
      return QImage();

    if (alpha)
      continue;

    for (int x = 0; x < w; ++x) {
      *&line[x] |= 0xff000000;
    }
  }

  return image;
}


QPixmap GDICapture::capture(WId window, const QRect &rect, int screens, bool cursor)
{
  Q_UNUSED(screens)
  Q_UNUSED(cursor)

  HWND hwnd      = window ? (HWND) window : GetDesktopWindow();
  HDC display_dc = GetDC(hwnd);
  HDC bitmap_dc  = CreateCompatibleDC(display_dc);
  HBITMAP bitmap = CreateCompatibleBitmap(display_dc, rect.width(), rect.height());

  SelectObject(bitmap_dc, bitmap);

  BitBlt(bitmap_dc, 0, 0, rect.width(), rect.height(), display_dc, rect.x(), rect.y(), SRCCOPY | CAPTUREBLT);
  
  if (cursor)
    drawMouse(bitmap_dc);

  const QPixmap pixmap = QPixmap::fromImage(imageFromWinHBITMAP(bitmap_dc, bitmap, rect.width(), rect.height(), false));

  ReleaseDC(hwnd, display_dc);
  DeleteDC(bitmap_dc);
  DeleteObject(bitmap);

  return pixmap;
}
