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

#include <QLineEdit>
#include <QHBoxLayout>

#include "TitleWidget.h"

TitleWidget::TitleWidget(QWidget *parent)
  : QWidget(parent)
{
  m_lineEdit = new QLineEdit(this);
  m_lineEdit->setMinimumHeight(24);
  m_lineEdit->setMaxLength(512);
  m_lineEdit->setPlaceholderText(tr("Enter title"));

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->addWidget(m_lineEdit, 3);
  layout->addStretch(1);
  layout->setContentsMargins(10, 0, 10, 0);
}


QString TitleWidget::title() const
{
  return m_lineEdit->text();
}
