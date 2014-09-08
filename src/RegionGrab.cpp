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

#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QToolTip>
#include <QPainter>

#include "Grabber.h"
#include "RegionGrab.h"

RegionGrab::RegionGrab(Grabber *grabber, const QRect &startSelection)
  : QWidget(0, Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint)
  , m_grabbing(false)
  , m_mouseDown(false)
  , m_newSelection(false)
  , m_showHelp(true)
  , m_handleSize(10)
  , m_grabber(grabber)
//  , m_selection(startSelection)
  , m_mouseOverHandle(0)
  , m_TLHandle(0, 0, m_handleSize, m_handleSize)
  , m_TRHandle(0, 0, m_handleSize, m_handleSize)
  , m_BLHandle(0, 0, m_handleSize, m_handleSize)
  , m_BRHandle(0, 0, m_handleSize,m_handleSize)
  , m_LHandle(0, 0, m_handleSize,m_handleSize)
  , m_THandle(0, 0, m_handleSize,m_handleSize)
  , m_RHandle(0, 0, m_handleSize,m_handleSize)
  , m_BHandle(0, 0, m_handleSize,m_handleSize)
{
  Q_UNUSED(startSelection);

  m_handles << &m_TLHandle << &m_TRHandle << &m_BLHandle << &m_BRHandle
            << &m_LHandle  << &m_THandle  << &m_RHandle  << &m_BHandle;

  setMouseTracking(true);
  setAttribute(Qt::WA_DeleteOnClose);

  QTimer::singleShot(0, this, SLOT(init()));
}


RegionGrab::~RegionGrab()
{
}


void RegionGrab::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Escape) {
    emit regionUpdated(m_selection);
    emit regionGrabbed(QPixmap());

#   ifdef Q_OS_MAC
    showNormal();
#   endif

    close();
  }
  else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
    grabRect();
  }
  else
    event->ignore();
}


void RegionGrab::mouseDoubleClickEvent(QMouseEvent *event)
{
  Q_UNUSED(event);

  grabRect();
}


void RegionGrab::mouseMoveEvent(QMouseEvent *event)
{
  const bool shouldShowHelp = !m_helpTextRect.contains(event->pos());
  if (shouldShowHelp != m_showHelp) {
    m_showHelp = shouldShowHelp;
    update();
  }

  if (m_mouseDown) {
    if (m_newSelection) {
      QPoint p = event->pos();
      QRect r  = rect();
      m_selection = normalizeSelection(QRect(m_dragStartPoint, limitPointToRect(p, r)));
    }
    // moving the whole selection
    else if (m_mouseOverHandle == 0) {
      QRect r = rect().normalized(), s = m_selectionBeforeDrag.normalized();
      QPoint p = s.topLeft() + event->pos() - m_dragStartPoint;
      r.setBottomRight(r.bottomRight() - QPoint(s.width(), s.height()) + QPoint(1, 1));
      if (!r.isNull() && r.isValid())
        m_selection.moveTo(limitPointToRect(p, r));
    }
    // dragging a handle
    else {
      QRect r = m_selectionBeforeDrag;
      QPoint offset = event->pos() - m_dragStartPoint;

      // dragging one of the top handles
      if (m_mouseOverHandle == &m_TLHandle || m_mouseOverHandle == &m_THandle || m_mouseOverHandle == &m_TRHandle)
        r.setTop(r.top() + offset.y());

      // dragging one of the left handles
      if (m_mouseOverHandle == &m_TLHandle || m_mouseOverHandle == &m_LHandle || m_mouseOverHandle == &m_BLHandle)
        r.setLeft(r.left() + offset.x());

      // dragging one of the bottom handles
      if (m_mouseOverHandle == &m_BLHandle || m_mouseOverHandle == &m_BHandle || m_mouseOverHandle == &m_BRHandle)
        r.setBottom(r.bottom() + offset.y());

      // dragging one of the right handles
      if (m_mouseOverHandle == &m_TRHandle || m_mouseOverHandle == &m_RHandle || m_mouseOverHandle == &m_BRHandle)
        r.setRight(r.right() + offset.x());

      r.setTopLeft(limitPointToRect(r.topLeft(), rect()));
      r.setBottomRight(limitPointToRect(r.bottomRight(), rect()));
      m_selection = normalizeSelection(r);
    }

    update();
  }
  else {
    if (m_selection.isNull())
      return;

    bool found = false;
    foreach (QRect* r, m_handles) {
      if (r->contains(event->pos())) {
        m_mouseOverHandle = r;
        found = true;
        break;
      }
    }

    if (!found) {
      m_mouseOverHandle = 0;
      if (m_selection.contains(event->pos()))
        setCursor(Qt::OpenHandCursor);
      else
        setCursor(Qt::CrossCursor);
    }
    else {
      if (m_mouseOverHandle == &m_TLHandle || m_mouseOverHandle == &m_BRHandle)
        setCursor(Qt::SizeFDiagCursor);
      if (m_mouseOverHandle == &m_TRHandle || m_mouseOverHandle == &m_BLHandle)
        setCursor(Qt::SizeBDiagCursor);
      if (m_mouseOverHandle == &m_LHandle || m_mouseOverHandle == &m_RHandle)
        setCursor(Qt::SizeHorCursor);
      if (m_mouseOverHandle == &m_THandle || m_mouseOverHandle == &m_BHandle)
        setCursor(Qt::SizeVerCursor);
    }
  }
}


void RegionGrab::mousePressEvent(QMouseEvent *event)
{
  m_showHelp = !m_helpTextRect.contains(event->pos());
  if (event->button() == Qt::LeftButton) {
    m_mouseDown = true;
    m_dragStartPoint = event->pos();
    m_selectionBeforeDrag = m_selection;

    if (!m_selection.contains(event->pos())) {
      m_newSelection = true;
      m_selection = QRect();
    }
    else
      setCursor(Qt::ClosedHandCursor);
  }
  else if (event->button() == Qt::RightButton) {
    m_newSelection = false;
    m_selection = QRect();
    setCursor(Qt::CrossCursor);
  }

  update();
}


void RegionGrab::mouseReleaseEvent(QMouseEvent *event)
{
  m_mouseDown = false;
  m_newSelection = false;
  if (m_mouseOverHandle == 0 && m_selection.contains(event->pos()))
    setCursor(Qt::OpenHandCursor);

  if (event->button() == Qt::LeftButton && event->modifiers() != Qt::SHIFT)
    grabRect();

  update();
}


void RegionGrab::paintEvent(QPaintEvent *event)
{
  Q_UNUSED( event );
  if (m_grabbing)
    return;

  QPainter painter( this );

  QPalette pal(QToolTip::palette());
  QFont font = QToolTip::font();

  QColor handleColor = pal.color(QPalette::Active, QPalette::Highlight);
  handleColor.setAlpha(160);
  QColor overlayColor(0, 0, 0, 100);
  QColor textColor = pal.color(QPalette::Active, QPalette::Text);
  QColor textBackgroundColor = pal.color(QPalette::Active, QPalette::Base);
  painter.drawPixmap(0, 0, m_pixmap);
  painter.setFont(font);

  const QRect& r = m_selection;

  if (r.isNull()) {
    painter.setClipRegion(QRegion(rect()));
    painter.setPen(Qt::NoPen);
    painter.setBrush(overlayColor);
    painter.drawRect(rect());
  }
  else {
    painter.setClipRegion(QRegion(rect()).subtracted(r));
    painter.setPen(Qt::NoPen);
    painter.setBrush(overlayColor);
    painter.drawRect(rect());
    painter.setClipRect(rect());
    drawRect(&painter, r, handleColor);
  }

  // \bug Подсказка центрируется не правильно в многомониторной конфигурации.
  if (m_showHelp) {
    painter.setPen(textColor);
    painter.setBrush(textBackgroundColor);
    QString helpText = tr("Select a region using the mouse. To take the snapshot, press the Enter key or double click. Press Esc to quit.");
    QRect scrRect    = QApplication::desktop()->screenGeometry(QApplication::desktop()->primaryScreen());

    m_helpTextRect = painter.boundingRect(scrRect.adjusted(20, 20, -20, -20), Qt::AlignHCenter, helpText);
    m_helpTextRect.adjust(-4, -4, 8, 4);
    drawRect(&painter, m_helpTextRect, textColor, textBackgroundColor);
    painter.drawText(m_helpTextRect.adjusted(3, 3, -3, -3), helpText);
  }

  if (r.isNull())
    return;

  // The grabbed region is everything which is covered by the drawn
  // rectangles (border included). This means that there is no 0px
  // selection, since a 0px wide rectangle will always be drawn as a line.
  QString txt = QString("%1x%2").arg(r.width()).arg(r.height());
  QRect textRect = painter.boundingRect(rect(), Qt::AlignLeft, txt);
  QRect boundingRect = textRect.adjusted(-4, 0, 0, 0);

  // center, unsuitable for small selections
  if (textRect.width() < r.width() - 2 * m_handleSize && textRect.height() < r.height() - 2 * m_handleSize && (r.width() > 100 && r.height() > 100)) {
    boundingRect.moveCenter(r.center());
    textRect.moveCenter(r.center());
  }
  // on top, left aligned
  else if (r.y() - 3 > textRect.height() && r.x() + textRect.width() < rect().right()) {
    boundingRect.moveBottomLeft(QPoint(r.x(), r.y() - 3));
    textRect.moveBottomLeft(QPoint(r.x() + 2, r.y() - 3));
  }
  // left, top aligned
  else if (r.x() - 3 > textRect.width()) {
    boundingRect.moveTopRight(QPoint(r.x() - 3, r.y()));
    textRect.moveTopRight(QPoint(r.x() - 5, r.y()));
  }
  // at bottom, right aligned
  else if (r.bottom() + 3 + textRect.height() < rect().bottom() && r.right() > textRect.width()) {
    boundingRect.moveTopRight(QPoint(r.right(), r.bottom() + 3));
    textRect.moveTopRight(QPoint(r.right() - 2, r.bottom() + 3));
  }
  // right, bottom aligned
  else if (r.right() + textRect.width() + 3 < rect().width()) {
    boundingRect.moveBottomLeft(QPoint(r.right() + 3, r.bottom()));
    textRect.moveBottomLeft(QPoint(r.right() + 5, r.bottom()));
  }

  // if the above didn't catch it, you are running on a very tiny screen...
  drawRect(&painter, boundingRect, textColor, textBackgroundColor);

  painter.drawText(textRect, txt);

  if ((r.height() > m_handleSize * 2 && r.width() > m_handleSize * 2) || !m_mouseDown) {
    updateHandles();
    painter.setPen(Qt::NoPen);
    painter.setBrush(handleColor);
    painter.setClipRegion(handleMask(StrokeMask));
    painter.drawRect(rect());
    handleColor.setAlpha(60);
    painter.setBrush(handleColor);
    painter.setClipRegion(handleMask(FillMask));
    painter.drawRect(rect());
  }
}


void RegionGrab::resizeEvent(QResizeEvent *event)
{
  Q_UNUSED( event );
  if (m_selection.isNull())
    return;

  QRect r = m_selection;
  r.setTopLeft(limitPointToRect(r.topLeft(), rect()));
  r.setBottomRight(limitPointToRect(r.bottomRight(), rect()));

  if (r.width() <= 1 || r.height() <= 1) //this just results in ugly drawing...
    m_selection = QRect();
  else
    m_selection = normalizeSelection(r);
}


void RegionGrab::init()
{
  m_pixmap = m_grabber->grabFullScreen();

  QDesktopWidget *desktop = QApplication::desktop();
  QRect rect;

  if (desktop->screenCount() > 1) {
    for (int i = 0; i < desktop->screenCount(); ++i) {
      if (rect.isNull())
        rect = desktop->screenGeometry(i);
      else
        rect = rect.united(desktop->screenGeometry(i));
    }
  }
  else
    rect = QRect(QPoint(0, 0), m_pixmap.size());

  resize(rect.size());
  move(rect.topLeft());
  setCursor(Qt::CrossCursor);

# ifdef Q_OS_MAC
  showFullScreen();
# else
  show();
# endif

  raise();
  activateWindow();
  setFocus();
}


QPoint RegionGrab::limitPointToRect(const QPoint &p, const QRect &r) const
{
  QPoint q;
  q.setX(p.x() < r.x() ? r.x() : p.x() < r.right() ? p.x() : r.right());
  q.setY(p.y() < r.y() ? r.y() : p.y() < r.bottom() ? p.y() : r.bottom());
  return q;
}


QRect RegionGrab::normalizeSelection(const QRect &s) const
{
  QRect r = s;
  if (r.width() <= 0) {
    int l = r.left();
    int w = r.width();
    r.setLeft(l + w - 1);
    r.setRight(l);
  }

  if (r.height() <= 0) {
    int t = r.top();
    int h = r.height();
    r.setTop(t + h - 1);
    r.setBottom(t);
  }

  return r;
}


QRegion RegionGrab::handleMask(MaskType type) const
{
  QRegion mask;
  foreach(QRect *rect, m_handles) {
    if (type == StrokeMask)
      mask += QRegion(*rect).subtracted(rect->adjusted(1, 1, -1, -1));
    else
      mask += QRegion(rect->adjusted(1, 1, -1, -1));
  }

  return mask;
}


void RegionGrab::drawRect(QPainter *painter, const QRect &r, const QColor &outline, const QColor &fill)
{
  const QRegion clip = QRegion(r).subtracted(r.adjusted(1, 1, -1, -1));

  painter->save();
  painter->setClipRegion(clip);
  painter->setPen(Qt::NoPen);
  painter->setBrush(outline);
  painter->drawRect(r);

  if (fill.isValid()) {
    painter->setClipping(false);
    painter->setBrush(fill);
    painter->drawRect(r.adjusted(1, 1, -1, -1));
  }

  painter->restore();
}


void RegionGrab::grabRect()
{
  const QRect &r = m_selection;
  if (!r.isNull() && r.isValid()) {
    m_grabbing = true;
    emit regionUpdated(r);
    emit regionGrabbed(m_pixmap.copy(r));
  }

# ifdef Q_OS_MAC
  showNormal();
# endif
  close();
}


void RegionGrab::updateHandles()
{
  const QRect& r = m_selection;
  const int s2   = m_handleSize / 2;

  m_TLHandle.moveTopLeft(r.topLeft());
  m_TRHandle.moveTopRight(r.topRight());
  m_BLHandle.moveBottomLeft(r.bottomLeft());
  m_BRHandle.moveBottomRight(r.bottomRight());

  m_LHandle.moveTopLeft(QPoint(r.x(), r.y() + r.height() / 2 - s2));
  m_THandle.moveTopLeft(QPoint(r.x() + r.width() / 2 - s2, r.y()));
  m_RHandle.moveTopRight(QPoint(r.right(), r.y() + r.height() / 2 - s2));
  m_BHandle.moveBottomLeft(QPoint(r.x() + r.width() / 2 - s2, r.bottom()));
}
