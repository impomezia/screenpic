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

#ifndef GRABBER_H_
#define GRABBER_H_

#include <QObject>
#include <QPixmap>

#include "data/UploadItem.h"

class ICapture;
class RegionGrab;

class Grabber : public QObject
{
  Q_OBJECT

public:
  enum CaptureMode {
    FullScreen,
    Region
  };

  Grabber(QObject *parent = 0);
  ~Grabber();
  inline bool isCaptureMouse() const        { return m_captureMouse; }
  inline const QPixmap &snapshot() const    { return m_snapshot; }
  inline void setCaptureMouse(bool capture) { m_captureMouse = capture; }
  void grab(CaptureMode mode);

signals:
  void ready(UploadItemPtr item);

public slots:
  const QPixmap &grabFullScreen();

private slots:
  void regionGrabbed(const QPixmap &pixmap);
  void regionUpdated(const QRect &rect);

private:
  void grabRegion();
  void performGrab();
  void ready();

  bool m_captureMouse;
  CaptureMode m_mode;
  QList<ICapture*> m_capture;
  QPixmap m_snapshot;
  QRect m_lastRegion;
  RegionGrab *m_regionGrab;
};

#endif /* GRABBER_H_ */
