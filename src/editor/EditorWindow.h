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

#ifndef EDITORWINDOW_H_
#define EDITORWINDOW_H_

#include <QMainWindow>
#include <QMap>
#include <QStringList>

#include "data/UploadItem.h"

class EditorScene;
class ItemColorButton;
class QActionGroup;
class QGraphicsView;
class QLineEdit;
class QPushButton;
class QRunnable;
class QSpinBox;
class Settings;
class TitleWidget;
class Translation;

class EditorWindow : public QMainWindow
{
  Q_OBJECT

public:
  EditorWindow(Settings *settings, Translation *translation, QWidget *parent = 0, Qt::WindowFlags flags = 0);
  void open(UploadItemPtr item);

signals:
  void editingFinished(UploadItemPtr item);
  void taskCreated(QRunnable *task);

protected:
  bool event(QEvent *event) override;
  void changeEvent(QEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void showEvent(QShowEvent *event) override;

private slots:
  void onColorAtCursorChanged(const QColor &color);
  void onColorChanged(QRgb color);
  void onColorSelected(const QColor &color);
  void onCopied(const QPixmap &pixmap);
  void onDropperClicked();
  void onModeChanged(int mode);
  void onRendered(const QImage &image);
  void onSelectionChanged();
  void onWidthChanged(int width);
  void saveAs();
  void setMode();

private:
  QAction *addAction(const QIcon &icon, const QString &text, int mode);
  void fillMainToolBar();
  void fillModeToolBar();
  void retranslateUi();
  void updateStyleSheet();

  bool m_firstShow;
  EditorScene *m_scene;
  ItemColorButton *m_colorBtn;
  QAction *m_colorAction;
  QAction *m_copyAction;
  QAction *m_redoAction;
  QAction *m_saveAction;
  QAction *m_undoAction;
  QActionGroup *m_modesGroup;
  QGraphicsView *m_view;
  QMap<int, QAction*> m_modes;
  QPushButton *m_publishBtn;
  QSpinBox *m_widthSpBx;
  QToolBar *m_mainToolBar;
  QToolBar *m_modeToolBar;
  Settings *m_settings;
  TitleWidget *m_titleEdit;
  Translation *m_translation;
  UploadItemPtr m_item;
};

#endif /* EDITORWINDOW_H_ */
