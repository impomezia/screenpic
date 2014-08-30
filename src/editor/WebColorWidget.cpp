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

#include <QEvent>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMenu>
#include <QToolButton>

#include "WebColorWidget.h"
#include "sglobal.h"

LineEdit::LineEdit(QWidget *parent)
  : QLineEdit(parent)
{
  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
}


QSize	LineEdit::sizeHint() const
{
  QSize size = QLineEdit::sizeHint();
  size.setWidth(64);

  return size;
}


WebColorWidget::WebColorWidget(QWidget *parent)
  : QWidget(parent)
{
  m_lineEdit = new LineEdit(this);

  m_copyBtn = new QToolButton(this);
  m_copyBtn->setAutoRaise(true);
  m_copyBtn->setIcon(QIcon(LS(":/images/edit-copy.png")));

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->addWidget(m_lineEdit);
  layout->addWidget(m_copyBtn);
  layout->setContentsMargins(9, 6, 0, 4);
  layout->setSpacing(2);
  layout->setStretch(0, 1);

  connect(m_lineEdit, SIGNAL(returnPressed()), SLOT(onReturnPressed()));
  connect(m_copyBtn, SIGNAL(clicked()), SLOT(copy()));

  retranslateUi();
}


void WebColorWidget::copy()
{
  m_lineEdit->selectAll();
  m_lineEdit->copy();

  if (qobject_cast<QMenu*>(parentWidget()))
    parentWidget()->close();
}


void WebColorWidget::setRgb(QRgb color)
{
  m_lineEdit->setText(QColor(color).name());
}


void WebColorWidget::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWidget::changeEvent(event);
}


void WebColorWidget::showEvent(QShowEvent *event)
{
  QWidget::showEvent(event);

  m_lineEdit->setFocus();
  m_lineEdit->selectAll();
}


void WebColorWidget::onReturnPressed()
{
  const QColor color(m_lineEdit->text());
  if (!color.isValid())
    return;

  if (qobject_cast<QMenu*>(parentWidget()))
    parentWidget()->close();

  emit changed(color);
}


void WebColorWidget::retranslateUi()
{
  m_copyBtn->setToolTip(tr("Copy"));
}
