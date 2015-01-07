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

#ifndef EDITORSCENE_H_
#define EDITORSCENE_H_

#include <QGraphicsScene>
#include <QMap>

class EditorItem;
class QUndoStack;

class EditorScene : public QGraphicsScene
{
  Q_OBJECT

public:
  enum Mode {
    HandMode,
    RectangleMode,
    EllipseMode,
    LineMode,
    ArrowMode,
    PenMode,
    TextMode,
    BrushMode,
    HighlightMode,
    BlurMode,
    CropMode,
    DropperMode,
    ModesCount
  };

  EditorScene(QObject *parent = 0);
  ~EditorScene();
  inline const QPen& pen() const           { return m_pen; }
  inline EditorItem *item(Mode mode) const { return m_items.value(mode); }
  inline Mode mode() const                 { return m_mode; }
  inline QUndoStack *undoStack()           { return m_undoStack; }
  inline void setColor(QRgb color)         { m_pen.setColor(color); }
  inline void setWidth(int width)          { m_pen.setWidth(width); }
  QColor colorAt(const QPointF &pos);
  QPixmap toPixmap(bool resize = true, const QRectF &rect = QRectF());
  void crop(const QRectF &rect);
  void resetTextInteraction();
  void setMode(Mode mode);
  void setModeData(Mode mode, const QVariant &data);

signals:
  void colorAtCursorChanged(const QColor &color);
  void colorSelected(const QColor &color);
  void copied(const QPixmap &pixmap);
  void modeChanged(int mode);
  void rendered(const QImage &image);
  void saveRequest();

public slots:
  void copy();
  void reload();
  void renderImage();

protected:
  void keyPressEvent(QKeyEvent *event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
  Mode m_mode;
  QGraphicsItem *m_movingItem;
  QMap<Mode, EditorItem*> m_items;
  QPen m_pen;
  QPointF m_oldPos;
  QUndoStack *m_undoStack;
};

#endif /* EDITORSCENE_H_ */
