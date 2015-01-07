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

#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QTextCursor>
#include <QTextDocument>

#include "EditorScene.h"
#include "items/TextItem.h"

TextItem::TextItem(QGraphicsItem *parent)
  : QGraphicsTextItem(parent)
  , m_textBorder(true)
{
  setFlag(ItemIsMovable);
  setFlag(ItemIsSelectable);
  setFlag(ItemIsFocusable);

  document()->setDocumentMargin(8);

  QFont font = this->font();
  font.setFamily("PT Sans");

  setFont(font);
}


void TextItem::start(const QPointF &point, const QPen &pen)
{
  setTextInteractionFlags(Qt::TextEditorInteraction);
  setDefaultTextColor(pen.color());

  QFont f = font();
  f.setPixelSize(13 + pen.width());

  QPointF p = point;
  p.setY(p.y() - 12);

  setFont(f);
  setPos(p);
  setFocus();
}


void TextItem::focusInEvent(QFocusEvent *event)
{
  setCursor(Qt::IBeamCursor);
  scene()->clearSelection();
  setSelected(true);

  QGraphicsTextItem::focusOutEvent(event);
}


void TextItem::focusOutEvent(QFocusEvent *event)
{
  if(event->reason() != Qt::PopupFocusReason) {
    QTextCursor cursor(textCursor());
    cursor.clearSelection();
    setTextCursor(cursor);
  }

  unsetCursor();

  QGraphicsTextItem::focusOutEvent(event);
}


void TextItem::keyPressEvent(QKeyEvent *event)
{
  QGraphicsTextItem::keyPressEvent(event);

  if (event->key() == Qt::Key_Escape) {
    setSelected(false);
    clearFocus();
  }
}


void TextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  if (!m_textBorder || (!hasFocus() && toPlainText().isEmpty())) {
    return QGraphicsTextItem::paint(painter, option, widget);
  }

  painter->save();
  painter->setPen(QPen(QBrush(0x002c559d), 2));
  painter->setBrush(QColor(0, 0, 0, 204));
  painter->drawRoundedRect(boundingRect(), 4, 4);
  painter->restore();

  QGraphicsTextItem::paint(painter, option, widget);
}


IItemCommand *TextCreator::command(EditorScene *scene, const QPointF &point)
{
  TextCommand *command = new TextCommand(scene, point);
  command->setTextBorder(m_data.toBool());
  return command;
}


QGraphicsItem *TextCreator::create(EditorScene *scene, const QPointF &point)
{
  Q_UNUSED(scene)
  Q_UNUSED(point);

  return 0;
}


bool TextCommand::validate()
{
  TextItem *item = qgraphicsitem_cast<TextItem*>(m_scene->itemAt(m_point, QTransform()));
  if (item) {
    item->setTextInteractionFlags(Qt::TextEditorInteraction);
    item->setFocus();
    return false;
  }

  return true;
}


void TextCommand::create()
{
  TextItem *item = new TextItem();
  item->setTextBorder(m_textBorder);
  m_scene->addItem(item);
  item->start(m_point, m_scene->pen());

  m_item = item;
}
