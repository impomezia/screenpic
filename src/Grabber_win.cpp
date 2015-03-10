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

#include <qt_windows.h>

#include "capture/GDICapture.h"
#include "Grabber.h"

typedef HRESULT (STDAPICALLTYPE *DwmIsCompositionEnabled_t)(BOOL *);
static DwmIsCompositionEnabled_t pDwmIsCompositionEnabled = 0;

typedef HRESULT (STDAPICALLTYPE *DwmGetWindowAttribute_t)(HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute);
static DwmGetWindowAttribute_t pDwmGetWindowAttribute = 0;


void init()
{
  static bool initialized = false;
  if (initialized)
      return;

  HMODULE dwmapi = LoadLibraryW(L"dwmapi.dll");
  if (dwmapi) {
      pDwmIsCompositionEnabled = (DwmIsCompositionEnabled_t) GetProcAddress(dwmapi, "DwmIsCompositionEnabled");
      pDwmGetWindowAttribute   = (DwmGetWindowAttribute_t) GetProcAddress(dwmapi, "DwmGetWindowAttribute");
  }
}


bool dwmIsCompositionEnabled()
{
  init();

  BOOL enabled = false;
  if (pDwmIsCompositionEnabled)
      pDwmIsCompositionEnabled(&enabled);

  return enabled;
}


QSize getBorderSize(HWND hwnd)
{
  WINDOWINFO wi;
  wi.cbSize = sizeof(WINDOWINFO);

  if (GetWindowInfo(hwnd, &wi))
    return QSize(wi.cxWindowBorders, wi.cyWindowBorders);

  return QSize();
}


QRect maximizedWindowFix(HWND hwnd, const QRect &rect)
{
  const QSize size = getBorderSize(hwnd);
  if (!size.isValid())
    return rect;

  return QRect(rect.x() + size.width(), rect.y() + size.height(), rect.width() - size.width() * 2, rect.height() - size.height() * 2);
}


QRect getWindowRect(HWND hwnd)
{
  QRect rect;
  RECT rc;

  if (!dwmIsCompositionEnabled() || !pDwmGetWindowAttribute || FAILED(pDwmGetWindowAttribute(hwnd, 9 /*DWMWA_EXTENDED_FRAME_BOUNDS*/, &rc, sizeof(rc)))) {
    GetWindowRect(hwnd, &rc);
  }

  rect = QRect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
  if (IsZoomed(hwnd))
    rect = maximizedWindowFix(hwnd, rect);

  return rect;
}


void Grabber::grabWindow()
{
  const QRect rect = getWindowRect(GetForegroundWindow());

  foreach (ICapture *capture, m_capture) {
    m_snapshot = capture->capture(0, rect, 1, m_captureMouse);
    if (!m_snapshot.isNull())
      break;
  }
}


void Grabber::init()
{
  m_capture.prepend(new GDICapture());
}
