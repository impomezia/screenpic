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

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPushButton>
#include <QToolBar>
#include <QUndoStack>

#include "data/ImageItem.h"
#include "EditorScene.h"
#include "EditorWindow.h"
#include "ItemColorButton.h"
#include "items/ColorCommand.h"
#include "items/EditorItem.h"
#include "Settings.h"
#include "sglobal.h"
#include "tasks/SaveTask.h"
#include "ui/TitleWidget.h"

EditorWindow::EditorWindow(Settings *settings, QWidget *parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags)
  , m_firstShow(true)
  , m_settings(settings)
{
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowTitle("Screenpic");
  setContextMenuPolicy(Qt::NoContextMenu);

  m_scene = new EditorScene(this);

  m_mainToolBar = new QToolBar(this);
  m_mainToolBar->setMovable(false);
  m_mainToolBar->setIconSize(QSize(24, 24));

  m_modeToolBar = new QToolBar(this);
  m_modeToolBar->setMovable(false);
  m_modeToolBar->setIconSize(QSize(24, 24));

  m_modesGroup = new QActionGroup(this);
  m_modesGroup->setExclusive(true);

  fillModeToolBar();
  fillMainToolBar();

  addToolBar(Qt::BottomToolBarArea, m_mainToolBar);
  addToolBar(Qt::TopToolBarArea,    m_modeToolBar);
  m_modes[EditorScene::HandMode]->setChecked(true);

  m_view = new QGraphicsView(this);
  m_view->setDragMode(QGraphicsView::ScrollHandDrag);
  m_view->setRenderHint(QPainter::Antialiasing);
  m_view->setScene(m_scene);
  m_view->setFrameStyle(QFrame::Plain | QFrame::NoFrame);
  m_view->setFocus();
  m_view->setMouseTracking(true);

  updateStyleSheet();
  setCentralWidget(m_view);

  connect(m_scene, SIGNAL(modeChanged(int)), SLOT(onModeChanged(int)));
  connect(m_scene, SIGNAL(rendered(QImage)), SLOT(onRendered(QImage)));
  connect(m_scene, SIGNAL(copied(QPixmap)), SLOT(onCopied(QPixmap)));
  connect(m_scene, SIGNAL(saveRequest()), SLOT(saveAs()));
  connect(m_scene, SIGNAL(selectionChanged()), SLOT(onSelectionChanged()));
  connect(m_scene, SIGNAL(colorAtCursorChanged(QColor)), SLOT(onColorAtCursorChanged(QColor)));
  connect(m_scene, SIGNAL(colorSelected(QColor)), SLOT(onColorSelected(QColor)));
  connect(m_colorBtn, SIGNAL(changed(QRgb)), SLOT(onColorChanged(QRgb)));
  connect(m_colorBtn, SIGNAL(dropperClicked()), SLOT(onDropperClicked()));
}


void EditorWindow::open(UploadItemPtr item)
{
  QImage *image = 0;
  if (item->type() == ImageItem::Type) {
    ImageItem *i = uploaditem_cast<ImageItem*>(item.data());
    if (i)
      image = &i->image;
  }

  if (!image)
    return;

  m_item = item;
  m_scene->addPixmap(QPixmap::fromImage(*image));
  m_scene->setSceneRect(0, 0, image->width(), image->height());
}


bool EditorWindow::event(QEvent *event)
{
# if defined(Q_OS_WIN32)
  if (event->type() == QEvent::ApplicationPaletteChange)
    updateStyleSheet();
# endif

  return QMainWindow::event(event);
}


void EditorWindow::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QMainWindow::changeEvent(event);
}


void EditorWindow::keyPressEvent(QKeyEvent *event)
{
  QMainWindow::keyPressEvent(event);

  if (event->key() == Qt::Key_Escape && m_scene->mode() != EditorScene::TextMode)
    close();
}


void EditorWindow::showEvent(QShowEvent *event)
{
  if (m_firstShow) {
    const QRect frame  = frameGeometry();
    const QRect screen = QApplication::desktop()->availableGeometry(this);
    move((screen.width() - frame.width()) / 2, (screen.height() - frame.height()) / 2);

    m_firstShow = false;
  }

  QMainWindow::showEvent(event);
}


void EditorWindow::onColorAtCursorChanged(const QColor &color)
{
  Q_UNUSED(color)
  //m_colorBtn->setTempColor(color);
}


void EditorWindow::onColorChanged(QRgb color)
{
  m_scene->setColor(color);

  EditorItem *item = m_scene->item(m_scene->mode());
  if (item)
    m_settings->setValue(LS("Modes/") + item->id() + LS(".color"), QColor(color).name());
  else
    m_settings->setValue(LS("Color"), QColor(color).name());

  const QList<QGraphicsItem *> items = m_scene->selectedItems();
  if (items.isEmpty())
    return;

  ColorCommand *command = new ColorCommand(items, color);
  if (!command->isValid()) {
    delete command;
    return;
  }

  m_scene->undoStack()->push(command);
}


void EditorWindow::onColorSelected(const QColor &color)
{
  m_colorBtn->setColor(color);
}


void EditorWindow::onCopied(const QPixmap &pixmap)
{
  QApplication::clipboard()->setPixmap(pixmap);
}


void EditorWindow::onDropperClicked()
{
  m_scene->setMode(EditorScene::DropperMode);
}


void EditorWindow::onModeChanged(int mode)
{
  m_scene->resetTextInteraction();

  if (mode != EditorScene::TextMode && m_scene->focusItem())
    m_scene->focusItem()->clearFocus();

  if (mode == EditorScene::HandMode) {
    m_view->setDragMode(QGraphicsView::ScrollHandDrag);
    m_view->setCursor(Qt::OpenHandCursor);
    m_colorAction->setEnabled(true);
  }
  else if (mode == EditorScene::DropperMode) {
    m_view->setDragMode(QGraphicsView::NoDrag);

    QCursor cursor(QPixmap(":/images/dropper-cur.png"), 0, 15);

    m_view->viewport()->setCursor(cursor);
    m_view->setCursor(cursor);
    m_colorAction->setEnabled(true);
  }
  else {
    m_scene->clearSelection();
    m_view->setDragMode(QGraphicsView::NoDrag);

    if (mode == EditorScene::BlurMode && m_scene->sceneRect() != m_scene->itemsBoundingRect())
      m_scene->toPixmap();

    EditorItem *item = m_scene->item(static_cast<EditorScene::Mode>(mode));
    QColor color = m_colorBtn->customColor();

    if (item) {
      m_view->viewport()->setCursor(item->cursor());

      if (item->color().isValid()) {
        if (!color.isValid())
          color = m_settings->value(LS("Modes/") + item->id() + LS(".color"), item->color().name()).toString();

        m_scene->setColor(color.rgba());
        m_colorBtn->setColor(color.rgba());
      }
    }

    m_colorAction->setEnabled(color.isValid());
  }

  QAction *action = m_modes.value(mode);
  if (action)
    action->setChecked(true);
}


void EditorWindow::onRendered(const QImage &image)
{
  if (m_item->type() == ImageItem::Type)
    uploaditem_cast<ImageItem*>(m_item.data())->image = image;

  m_item->setTitle(m_titleEdit->title());
  emit editingFinished(m_item);

  close();
}


void EditorWindow::onSelectionChanged()
{
  if (m_scene->mode() != EditorScene::HandMode)
    return;

  const QColor color = ColorCommand::getColor(m_scene->selectedItems());
  if (color.isValid())
    m_colorBtn->setColor(color.rgba());
}


void EditorWindow::saveAs()
{
  QString fileName = m_settings->value(Settings::kLastSaveDir).toString();
  if (m_item->type() == ImageItem::Type)
    fileName += LC('/') + QFileInfo(uploaditem_cast<ImageItem*>(m_item.data())->saveAs).fileName();

  fileName = QFileDialog::getSaveFileName(this, tr("Save"), fileName, tr("PNG (*.png)"));
  if (fileName.isEmpty())
    return;

  m_settings->setValue(Settings::kLastSaveDir, QFileInfo(fileName).absolutePath());

  emit taskCreated(new SaveTask(ChatId(ChatId::UserId), m_scene->toPixmap().toImage(), fileName));
}


void EditorWindow::setMode()
{
  QAction *action = qobject_cast<QAction*>(sender());
  if (!action)
    return;

  m_scene->setMode(static_cast<EditorScene::Mode>(action->data().toInt()));
}


QAction *EditorWindow::addAction(const QIcon &icon, const QString &text, int mode)
{
  QAction *action = m_modeToolBar->addAction(icon, text, this, SLOT(setMode()));
  action->setData(mode);
  action->setCheckable(true);

  m_modesGroup->addAction(action);
  m_modes.insert(mode, action);
  return action;
}


void EditorWindow::fillMainToolBar()
{
  m_undoAction = m_scene->undoStack()->createUndoAction(m_mainToolBar, tr("Undo (Ctrl+Z)"));
  m_undoAction->setIcon(QIcon(":/images/undo.png"));
  m_mainToolBar->addAction(m_undoAction);

  m_redoAction = m_scene->undoStack()->createRedoAction(m_mainToolBar, tr("Redo (Ctrl+Y)"));
  m_redoAction->setIcon(QIcon(":/images/redo.png"));
  m_mainToolBar->addAction(m_redoAction);
  m_mainToolBar->addSeparator();

  m_titleEdit = new TitleWidget(this);
  m_mainToolBar->addWidget(m_titleEdit);

  m_copyAction = m_mainToolBar->addAction(QIcon(":/images/copy.png"), tr("Copy (Ctrl+C)"), m_scene, SLOT(copy()));
  m_saveAction = m_mainToolBar->addAction(QIcon(":/images/save.png"), tr("Save as... (Ctrl+S)"), this, SLOT(saveAs()));

  m_publishBtn = new QPushButton(QIcon(":/images/publish.png"), tr("Publish"), this);
  m_publishBtn->setDefault(true);
  m_publishBtn->setToolTip(tr("Publish (Ctrl+Enter)"));
  m_publishBtn->setIconSize(QSize(24, 24));
  connect(m_publishBtn, SIGNAL(clicked()), m_scene, SLOT(renderImage()));

  m_mainToolBar->addSeparator();
  m_mainToolBar->addWidget(m_publishBtn);
}


void EditorWindow::fillModeToolBar()
{
  addAction(QIcon(":/images/move-tool.png"), tr("Move (H)"), EditorScene::HandMode);
  addAction(QIcon(":/images/crop.png"), tr("Crop"), EditorScene::CropMode);

  m_modeToolBar->addSeparator();
  addAction(QIcon(":/images/rectangle.png"), tr("Rectangle (U)"), EditorScene::RectangleMode);
  addAction(QIcon(":/images/ellipse.png"),   tr("Ellipse"),       EditorScene::EllipseMode);

  m_modeToolBar->addSeparator();
  addAction(QIcon(":/images/arrow.png"), tr("Arrow"), EditorScene::ArrowMode);
  addAction(QIcon(":/images/line.png"),  tr("Line"),  EditorScene::LineMode);

  m_modeToolBar->addSeparator();
  addAction(QIcon(":/images/pen.png"),       tr("Pen (P)"), EditorScene::PenMode);
  addAction(QIcon(":/images/brush.png"),     tr("Brush"),   EditorScene::BrushMode);
  addAction(QIcon(":/images/highlight.png"), tr("Marker"),  EditorScene::HighlightMode);

  m_modeToolBar->addSeparator();
  addAction(QIcon(":/images/text.png"), tr("Text (T)"), EditorScene::TextMode);
  addAction(QIcon(":/images/blur.png"), tr("Blur"), EditorScene::BlurMode);

  m_colorBtn = new ItemColorButton(this);
  m_colorBtn->setColor(m_settings->value(LS("Color")).toString());

  m_modeToolBar->addSeparator();
  m_colorAction = m_modeToolBar->addWidget(m_colorBtn);

  QAction *dropper = new QAction(this);
  dropper->setCheckable(true);

  m_modesGroup->addAction(dropper);
  m_modes.insert(EditorScene::DropperMode, dropper);
}


void EditorWindow::retranslateUi()
{
  m_modes.value(EditorScene::HandMode)->setText(tr("Hand (H)"));
  m_modes.value(EditorScene::RectangleMode)->setText(tr("Rectangle (U)"));
  m_modes.value(EditorScene::EllipseMode)->setText(tr("Ellipse"));
  m_modes.value(EditorScene::ArrowMode)->setText(tr("Arrow"));
  m_modes.value(EditorScene::LineMode)->setText(tr("Line"));
  m_modes.value(EditorScene::PenMode)->setText(tr("Pen (P)"));
  m_modes.value(EditorScene::BrushMode)->setText(tr("Brush"));
  m_modes.value(EditorScene::HighlightMode)->setText(tr("Marker"));
  m_modes.value(EditorScene::TextMode)->setText(tr("Text (T)"));
  m_modes.value(EditorScene::BlurMode)->setText(tr("Blur"));
  m_modes.value(EditorScene::CropMode)->setText(tr("Crop"));

  m_publishBtn->setText(tr("Publish"));
  m_publishBtn->setToolTip(tr("Publish (Ctrl+Enter)"));

  m_undoAction->setText(tr("Undo (Ctrl+Z)"));
  m_redoAction->setText(tr("Redo (Ctrl+Y)"));

  m_copyAction->setText(tr("Copy (Ctrl+C)"));
  m_saveAction->setText(tr("Save as... (Ctrl+S)"));
}


void EditorWindow::updateStyleSheet()
{
  m_mainToolBar->setStyleSheet(QString("QToolBar { background-color: %1; margin:0px; border:0px; border-top: 1px solid %2; }").arg(palette().color(QPalette::Window).name()).arg(palette().color(QPalette::Midlight).name()));
  m_modeToolBar->setStyleSheet(QString("QToolBar { background-color: %1; margin:0px; border:0px; border-bottom: 1px solid %2; }").arg(palette().color(QPalette::Window).name()).arg(palette().color(QPalette::Midlight).name()));
}
