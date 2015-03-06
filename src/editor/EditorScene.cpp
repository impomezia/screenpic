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

#include <QGraphicsSceneMouseEvent>
#include <QImage>
#include <QKeyEvent>
#include <QPainter>
#include <QUndoStack>

#include "EditorScene.h"
#include "items/ArrowItem.h"
#include "items/BlurItem.h"
#include "items/BrushItem.h"
#include "items/CropItem.h"
#include "items/EllipseItem.h"
#include "items/HighlightItem.h"
#include "items/LineItem.h"
#include "items/MoveCommand.h"
#include "items/PenItem.h"
#include "items/RectangleItem.h"
#include "items/TextItem.h"
#include "sglobal.h"

EditorScene::EditorScene(QObject *parent)
  : QGraphicsScene(parent)
  , m_mode(HandMode)
  , m_movingItem(0)
{
  m_undoStack = new QUndoStack(this);

  m_pen.setColor(0xffd60808);
  m_pen.setWidth(3);

  m_items.insert(ArrowMode,     new ArrowCreator());
  m_items.insert(BlurMode,      new BlurCreator());
  m_items.insert(BrushMode,     new BrushCreator());
  m_items.insert(EllipseMode,   new EllipseCreator());
  m_items.insert(HighlightMode, new HighlightCreator());
  m_items.insert(LineMode,      new LineCreator());
  m_items.insert(PenMode,       new PenCreator());
  m_items.insert(RectangleMode, new RectangleCreator());
  m_items.insert(TextMode,      new TextCreator());
  m_items.insert(CropMode,      new CropCreator());
}


EditorScene::~EditorScene()
{
  qDeleteAll(m_items);
}


void EditorScene::resetTextInteraction()
{
  const QList<QGraphicsItem *> items = this->items();

  foreach (QGraphicsItem *item, items) {
    TextItem *i = qgraphicsitem_cast<TextItem*>(item);
    if (i) {
      i->setTextInteractionFlags(Qt::NoTextInteraction);
      i->setSelected(false);
    }
  }
}


void EditorScene::setMode(Mode mode)
{
  if (m_mode == mode)
    return;

  m_mode = mode;
  emit modeChanged(mode);
}


void EditorScene::setModeData(Mode mode, const QVariant &data)
{
  if (m_items.contains(mode))
    m_items.value(mode)->setData(data);

  if (mode == TextMode) {
    QList<QGraphicsItem *> items = selectedItems();
    if (!items.isEmpty() && items.first()->type() == TextItem::Type) {
      TextItem *item = qgraphicsitem_cast<TextItem*>(items.first());
      item->setTextBorder(data.toBool());
      item->setDefaultTextColor(pen().color());
    }
  }
}


void EditorScene::crop(const QRectF &rect)
{
  const QPixmap original = toPixmap();
  const QPixmap crop     = toPixmap(true, rect);

  undoStack()->clear();
  undoStack()->push(new CropCommand(this, original, crop));
}


QColor EditorScene::colorAt(const QPointF &pos)
{
  QPixmap pix(1, 1);
  pix.fill(Qt::transparent);

  QPainter painter(&pix);
  painter.setRenderHint(QPainter::Antialiasing);
  render(&painter, QRectF(), QRectF(pos, QSizeF(1, 1)));
  painter.end();

  return pix.toImage().pixel(0, 0);
}


QPixmap EditorScene::toPixmap(bool resize, const QRectF &rect)
{
  clearSelection();

  if (m_mode == TextMode && focusItem())
    focusItem()->clearFocus();

  if (resize)
    setSceneRect(itemsBoundingRect());

  QPixmap pix((!rect.isNull() ? rect : sceneRect()).size().toSize());
  pix.fill(Qt::transparent);

  QPainter painter(&pix);
  painter.setRenderHint(QPainter::Antialiasing);
  render(&painter, QRectF(), rect);
  painter.end();

  return pix;
}


void EditorScene::copy()
{
  emit copied(toPixmap());
}


void EditorScene::reload()
{
  const QList<QGraphicsItem *> items = this->items();
  QList<BlurItem *> blurs;

  foreach (QGraphicsItem *item, items) {
    if (item->type() == BlurItem::Type) {
      blurs.append(qgraphicsitem_cast<BlurItem *>(item));
      item->hide();
    }
  }

  if (blurs.isEmpty())
    return;

  const QPixmap pixmap = toPixmap(false);
  foreach (BlurItem *item, blurs) {
    item->show();
    item->reload(pixmap);
  }
}


void EditorScene::renderImage()
{
  emit rendered(toPixmap().toImage());
}


void EditorScene::keyPressEvent(QKeyEvent *event)
{
  QGraphicsScene::keyPressEvent(event);

  if (event->modifiers() == Qt::ControlModifier && (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return))
    renderImage();

  if (m_mode == TextMode && qgraphicsitem_cast<TextItem*>(focusItem()))
    return;

  if (event->modifiers() == Qt::NoModifier) {
    if (event->key() == Qt::Key_H)
      setMode(HandMode);
    else if (event->key() == Qt::Key_U)
      setMode(RectangleMode);
    else if (event->key() == Qt::Key_P)
      setMode(PenMode);
    else if (event->key() == Qt::Key_T)
      setMode(TextMode);
  }

  if (event->matches(QKeySequence::Undo))
    m_undoStack->undo();
  else if (event->matches(QKeySequence::Redo))
    m_undoStack->redo();
  else if (event->matches(QKeySequence::Copy))
    copy();
  else if (event->matches(QKeySequence::Save))
    emit saveRequest();
}


void EditorScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  if (m_mode == TextMode)
    return;

  QGraphicsScene::mouseMoveEvent(event);

  if (mode() != DropperMode)
    return;

  emit colorAtCursorChanged(colorAt(event->scenePos()));
}


void EditorScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  if (m_mode == HandMode) {
    m_movingItem = itemAt(event->buttonDownScenePos(Qt::LeftButton), QTransform());
    if (m_movingItem && event->button() == Qt::LeftButton)
      m_oldPos = m_movingItem->scenePos();

    clearSelection();
  }

  QGraphicsScene::mousePressEvent(event);

  if (event->button() != Qt::LeftButton)
    return;

  if (mode() == DropperMode) {
    emit colorSelected(colorAt(event->scenePos()));
    return;
  }

  if (m_mode != HandMode && m_items.contains(m_mode)) {
    IItemCommand *command = m_items.value(m_mode)->command(this, event->scenePos());

    if (command && command->validate())
        m_undoStack->push(command);
    else if (!command)
      addItem(m_items.value(m_mode)->create(this, event->scenePos()));
  }
}


void EditorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsScene::mouseReleaseEvent(event);

  if (m_movingItem && event->button() == Qt::LeftButton) {
    if (m_oldPos != m_movingItem->pos()) {
      m_undoStack->push(new MoveCommand(m_movingItem, m_oldPos));
      reload();
    }

    m_movingItem = 0;
  }
}
