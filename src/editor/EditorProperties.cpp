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

#include <QCheckBox>
#include <QVBoxLayout>

#include "EditorProperties.h"
#include "interfaces/IScreenpic.h"
#include "Settings.h"

EditorProperties::EditorProperties(IScreenpic *screenpic, QWidget *parent)
  : DialogFrame(parent)
  , m_screenpic(screenpic)
{
  m_layout->addWidget(line());

  m_publishOnCloseBtn = new QCheckBox(this);
  m_publishOnCloseBtn->setChecked(m_screenpic->settings()->value(Settings::kPublishOnClose).toBool());

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(m_publishOnCloseBtn);
  layout->setContentsMargins(0, 10, 0, 0);
  layout->setSpacing(10);

  m_layout->addLayout(layout);

  retranslateUi();

  connect(m_publishOnCloseBtn, SIGNAL(clicked()), SLOT(reload()));
}


void EditorProperties::retranslateUi()
{
  setTitle(tr("Editor properties"));

  m_publishOnCloseBtn->setText(tr("Publish image when closing the editor"));
}


void EditorProperties::reload()
{
  m_screenpic->settings()->setValue(Settings::kPublishOnClose, m_publishOnCloseBtn->isChecked());
}
