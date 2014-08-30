/*
 *   Copyright (C) 2007 Luca Gugelmann <lucag@student.ethz.ch>
 *   Copyright (C) 2013 Alexander Sedov <imp@schat.me>
 *
 *   This program is free software; you can redistribute it and/or modify it
 *   under the terms of the GNU Library General Public License version 2 as
 *   published by the Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef REGIONGRAB_H_
#define REGIONGRAB_H_

#include <QWidget>

class Grabber;

class RegionGrab : public QWidget
{
  Q_OBJECT

  enum MaskType {
    StrokeMask,
    FillMask
  };

public:
  RegionGrab(Grabber *grabber, const QRect &startSelection);
  ~RegionGrab();

signals:
  void regionGrabbed(const QPixmap &pixmap);
  void regionUpdated(const QRect &rect);

protected:
  void keyPressEvent(QKeyEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void paintEvent(QPaintEvent *event);
  void resizeEvent(QResizeEvent *event);

private slots:
  void init();

private:
  QPoint limitPointToRect(const QPoint &p, const QRect &r) const;
  QRect normalizeSelection(const QRect &s) const;
  QRegion handleMask(MaskType type) const;
  static void drawRect(QPainter *painter, const QRect &r, const QColor &outline, const QColor &fill = QColor());
  void grabRect();
  void updateHandles();

  bool m_grabbing;
  bool m_mouseDown;
  bool m_newSelection;
  bool m_showHelp;
  const int m_handleSize;
  Grabber *m_grabber;
  QPixmap m_pixmap;
  QPoint m_dragStartPoint;
  QRect m_selection;
  QRect m_selectionBeforeDrag;
  QRect* m_mouseOverHandle;
  QVector<QRect*> m_handles;

  // naming convention for handles
  // T top, B bottom, R Right, L left
  // 2 letters: a corner
  // 1 letter: the handle on the middle of the corresponding side
  QRect m_TLHandle;
  QRect m_TRHandle;
  QRect m_BLHandle;
  QRect m_BRHandle;
  QRect m_LHandle;
  QRect m_THandle;
  QRect m_RHandle;
  QRect m_BHandle;
  QRect m_helpTextRect;
};

#endif /* REGIONGRAB_H_ */
