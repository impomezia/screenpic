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
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>

#include "ImgurSettings.h"
#include "sglobal.h"

ImgurSettings::ImgurSettings(QWidget *parent)
  : QWidget(parent)
{
  m_accountGroup = new QGroupBox(this);

  m_firstStepLabel = new QLabel("1.", this);
  m_openPageBtn = new QPushButton(this);

  m_secondStepLabel = new QLabel("2.", this);
  m_pinEdit = new QLineEdit(this);

  m_pasteBtn = new QToolButton(this);
  m_pasteBtn->setObjectName("toolButton");
  m_pasteBtn->setAutoRaise(true);
  m_pasteBtn->setIcon(QIcon(LS(":/images/edit-paste.png")));

  m_thirdStepLabel = new QLabel("3.", this);
  m_completeBtn = new QPushButton(this);

  QGridLayout *accountLayout = new QGridLayout(m_accountGroup);
  accountLayout->addWidget(m_firstStepLabel, 0, 0);
  accountLayout->addWidget(m_openPageBtn, 0, 1);
  accountLayout->addWidget(m_secondStepLabel, 1, 0);
  accountLayout->addWidget(m_pinEdit, 1, 1, 1, 2);
  accountLayout->addWidget(m_pasteBtn, 1, 3);
  accountLayout->addWidget(m_thirdStepLabel, 2, 0);
  accountLayout->addWidget(m_completeBtn, 2, 1);
  accountLayout->setColumnStretch(1, 1);
  accountLayout->setColumnStretch(2, 1);
  accountLayout->setSpacing(4);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(m_accountGroup);
  layout->setMargin(0);

  retranslateUi();

  connect(m_pasteBtn, SIGNAL(clicked()), SLOT(onPaste()));
  connect(m_openPageBtn, SIGNAL(clicked()), SIGNAL(pinRequest()));
  connect(m_completeBtn, SIGNAL(clicked()), SLOT(onCompleteClicked()));
}


void ImgurSettings::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWidget::changeEvent(event);
}


void ImgurSettings::onCompleteClicked()
{
  emit pinReady(m_pinEdit->text());
}


void ImgurSettings::onPaste()
{
  m_pinEdit->clear();
  m_pinEdit->paste();
}


void ImgurSettings::retranslateUi()
{
  m_accountGroup->setTitle(tr("User account"));
  m_openPageBtn->setText(tr("Open authorize page..."));
  m_pinEdit->setPlaceholderText(tr("Enter verification PIN from authorize page"));
  m_pasteBtn->setToolTip(tr("Paste"));
  m_completeBtn->setText(tr("Complete authoruzation"));
}
