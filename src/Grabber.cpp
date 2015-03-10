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

#include <QApplication>
#include <QDesktopWidget>

#include "capture/QtCapture.h"
#include "data/ImageItem.h"
#include "Grabber.h"
#include "RegionGrab.h"

Grabber::Grabber(QObject *parent)
  : QObject(parent)
  , m_captureMouse(true)
  , m_mode(FullScreen)
  , m_regionGrab(0)
{
  m_capture.append(new QtCapture());

  init();
}


Grabber::~Grabber()
{
  qDeleteAll(m_capture);
}


void Grabber::grab(CaptureMode mode)
{
  m_mode = mode;
  performGrab();
}


void Grabber::grabRegion()
{
  m_regionGrab = new RegionGrab(this, m_lastRegion);

  connect(m_regionGrab, SIGNAL(regionGrabbed(QPixmap)), SLOT(regionGrabbed(QPixmap)));
  connect(m_regionGrab, SIGNAL(regionUpdated(QRect)), SLOT(regionUpdated(QRect)));
}


#ifndef Q_OS_WIN
void Grabber::grabWindow()
{
  grabFullScreen();
}


void Grabber::init()
{
}
#endif


void Grabber::performGrab()
{
  if (m_mode == FullScreen) {
    grabFullScreen();
    ready();
  }
  else if (m_mode == Window) {
    grabWindow();
    ready();
  }
  else if (m_mode == Region)
    grabRegion();
}


void Grabber::ready()
{
  emit ready(UploadItemPtr(new ImageItem(m_snapshot.toImage(), m_mode)));
}


/*!
 * Захват изображения со всех рабочих мониторов.
 */
const QPixmap &Grabber::grabFullScreen()
{
  QDesktopWidget *desktop = QApplication::desktop();
  QRect rect;
  const int screenCount = desktop->screenCount();

  if (screenCount > 1) {
    for (int i = 0; i < screenCount; ++i) {
      if (rect.isNull())
        rect = desktop->screenGeometry(i);
      else
        rect = rect.united(desktop->screenGeometry(i));
    }
  }
  else
    rect = desktop->screenGeometry();

  foreach (ICapture *capture, m_capture) {
    m_snapshot = capture->capture(desktop->winId(), rect, screenCount, m_captureMouse);
    if (!m_snapshot.isNull())
      return m_snapshot;
  }

  return m_snapshot;
}


void Grabber::regionGrabbed(const QPixmap &pixmap)
{
  if (!pixmap.isNull()) {
    m_snapshot = pixmap;
    ready();
  }

  QApplication::restoreOverrideCursor();
}


void Grabber::regionUpdated(const QRect &rect)
{
  m_lastRegion = rect;
}
