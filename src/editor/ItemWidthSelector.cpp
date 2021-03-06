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
#include <QFrame>
#include <QHBoxLayout>
#include <QMenu>
#include <QPainter>
#include <QSlider>
#include <QSpinBox>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

#include "ItemWidthSelector.h"
#include "sglobal.h"

ItemWidthSelector::ItemWidthSelector(QWidget *parent)
  : QWidget(parent)
{
  m_toolBar = new QToolBar(this);
  m_toolBar->setIconSize(QSize(20, 20));

  m_actionGroup = new QActionGroup(this);
  m_actionGroup->setExclusive(true);

  m_slider = new QSlider(Qt::Horizontal, this);
  m_slider->setRange(1, 50);
  m_slider->setValue(3);

  m_spinBox = new QSpinBox(this);
  m_spinBox->setRange(1, 50);
  m_spinBox->setValue(3);

  addStretch();
  add(3, 3);
  add(10, 5);
  addSpacing(5);
  add(16, 8);
  addSpacing(10);
  add(20, 10);
  addSpacing(5);
  addStretch();

  QFrame *line = new QFrame(this);
  line->setFrameStyle(QFrame::HLine | QFrame::Sunken);

  QHBoxLayout *advancedLayout = new QHBoxLayout();
  advancedLayout->addWidget(m_slider);
  advancedLayout->addWidget(m_spinBox);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(line);
  layout->addWidget(m_toolBar);
  layout->addLayout(advancedLayout);
  layout->setContentsMargins(4, 9, 4, 4);

  connect(m_actionGroup, SIGNAL(triggered(QAction*)), SLOT(onTriggered(QAction*)));
  connect(m_slider, SIGNAL(valueChanged(int)), m_spinBox, SLOT(setValue(int)));
  connect(m_slider, SIGNAL(valueChanged(int)), SLOT(onValueChanged(int)));
  connect(m_spinBox, SIGNAL(valueChanged(int)), m_slider, SLOT(setValue(int)));

  setStyleSheet(LS("QToolBar { border:none; }"));
}


void ItemWidthSelector::setAdvanced(bool advanced)
{
  m_toolBar->setVisible(!advanced);
  m_slider->setVisible(advanced);
  m_spinBox->setVisible(advanced);
}


void ItemWidthSelector::setColor(const QColor &color)
{
  foreach (QAction *action, m_actionGroup->actions()) {
    action->setIcon(pixmap(action->property("size").toInt(), color));
  }
}


void ItemWidthSelector::setWidth(int width)
{
  foreach (QAction *action, m_actionGroup->actions()) {
    action->setChecked(action->property("width") == width);
  }

  m_slider->setValue(width);
}


QAction *ItemWidthSelector::add(int width, int size)
{
  QAction *action = m_toolBar->addAction(QIcon(), QString());
  action->setCheckable(true);
  action->setChecked(m_actionGroup->actions().isEmpty());
  action->setProperty("width", width);
  action->setProperty("size", size);

  m_actionGroup->addAction(action);

  return action;
}


void ItemWidthSelector::onTriggered(QAction *action)
{
  if (qobject_cast<QMenu*>(parentWidget()))
    parentWidget()->close();

  m_slider->setValue(action->property("width").toInt());
}


void ItemWidthSelector::onValueChanged(int value)
{
  setWidth(value);

  emit changed(value);
}


QPixmap ItemWidthSelector::pixmap(int size, const QColor &color) const
{
  QPainter painter;
  QPixmap pix(20, 20);
  pix.fill(Qt::transparent);
  painter.begin(&pix);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setPen(Qt::NoPen);
  painter.setBrush(color);
  painter.drawEllipse(QPoint(10, 10), size, size);
  painter.end();

  return pix;
}


void ItemWidthSelector::addSpacing(int size)
{
  QWidget *widget = new QWidget(this);
  widget->setFixedSize(size, 20);

  m_toolBar->addWidget(widget);
}


void ItemWidthSelector::addStretch()
{
  QWidget *stretch = new QWidget(this);
  stretch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  m_toolBar->addWidget(stretch);
}
