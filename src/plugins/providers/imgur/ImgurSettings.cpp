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
#include <QTimer>
#include <QToolButton>

#include "ImgurSettings.h"
#include "sglobal.h"
#include "ui/Spinner.h"

ImgurSettings::ImgurSettings(QWidget *parent)
  : QWidget(parent)
{
  m_loginGroup = new QGroupBox(this);

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
  m_completeBtn->setEnabled(false);
  m_spinner = new Spinner(this);
  m_errorLabel = new QLabel(this);
  m_errorLabel->setObjectName("errorLabel");

  m_accountGroup = new QGroupBox(this);
  m_userNameLabel = new QLabel(this);
  m_userNameValueLabel = new QLabel(this);
  m_userNameValueLabel->setTextFormat(Qt::PlainText);

  {
    QFont font = m_userNameValueLabel->font();
    font.setBold(true);
    m_userNameValueLabel->setFont(font);
  }

  m_logoutBtn = new QPushButton(this);

  QGridLayout *loginLayout = new QGridLayout(m_loginGroup);
  loginLayout->addWidget(m_firstStepLabel, 0, 0);
  loginLayout->addWidget(m_openPageBtn, 0, 1);
  loginLayout->addWidget(m_secondStepLabel, 1, 0);
  loginLayout->addWidget(m_pinEdit, 1, 1, 1, 3);
  loginLayout->addWidget(m_pasteBtn, 1, 4);
  loginLayout->addWidget(m_thirdStepLabel, 2, 0);
  loginLayout->addWidget(m_completeBtn, 2, 1);
  loginLayout->addWidget(m_spinner, 2, 2);
  loginLayout->addWidget(m_errorLabel, 2, 3, 1, 2);
  loginLayout->setColumnStretch(1, 1);
  loginLayout->setColumnStretch(3, 1);
  loginLayout->setSpacing(4);

  QGridLayout *accountLayout = new QGridLayout(m_accountGroup);
  accountLayout->addWidget(m_userNameLabel, 0, 0);
  accountLayout->addWidget(m_userNameValueLabel, 0, 1);
  accountLayout->addWidget(m_logoutBtn, 0, 2);
  accountLayout->setColumnStretch(1, 1);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(m_loginGroup);
  layout->addWidget(m_accountGroup);
  layout->setMargin(0);

  m_accountGroup->hide();

  retranslateUi();

  connect(m_pinEdit, SIGNAL(textChanged(QString)), SLOT(onTextChanged(QString)));
  connect(m_pasteBtn, SIGNAL(clicked()), SLOT(onPaste()));
  connect(m_openPageBtn, SIGNAL(clicked()), SIGNAL(pinRequest()));
  connect(m_completeBtn, SIGNAL(clicked()), SLOT(onCompleteClicked()));
  connect(m_logoutBtn, SIGNAL(clicked()), SLOT(onLogout()));
}


void ImgurSettings::setError(const QString &errorText)
{
  onTextChanged(m_pinEdit->text());
  m_spinner->stop();
  m_errorLabel->setText(errorText);
  m_errorLabel->show();
}


void ImgurSettings::setSuccess(const QString &username)
{
  m_loginGroup->hide();
  m_accountGroup->show();

  m_userNameValueLabel->setText(username);

  QTimer::singleShot(0, this, SLOT(queuedAdjustSize()));
}


void ImgurSettings::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWidget::changeEvent(event);
}


void ImgurSettings::onCompleteClicked()
{
  const QString text = m_pinEdit->text();
  if (text.isEmpty())
    return;

  m_completeBtn->setEnabled(false);
  m_spinner->start();
  m_errorLabel->hide();

  emit pinReady(text);
}


void ImgurSettings::onLogout()
{
  m_accountGroup->hide();
  m_loginGroup->show();

  emit logout();
}


void ImgurSettings::onPaste()
{
  m_pinEdit->clear();
  m_pinEdit->paste();
}


void ImgurSettings::onTextChanged(const QString &text)
{
  m_completeBtn->setEnabled(!text.isEmpty());
}


void ImgurSettings::queuedAdjustSize()
{
  parentWidget()->parentWidget()->parentWidget()->parentWidget()->adjustSize();
}


void ImgurSettings::retranslateUi()
{
  m_loginGroup->setTitle(tr("User account"));
  m_accountGroup->setTitle(m_loginGroup->title());
  m_openPageBtn->setText(tr("Open authorize page..."));
  m_pinEdit->setPlaceholderText(tr("Enter verification PIN from authorize page"));
  m_pasteBtn->setToolTip(tr("Paste"));
  m_completeBtn->setText(tr("Complete authoruzation"));

  m_userNameLabel->setText(tr("Username:"));
  m_logoutBtn->setText(tr("Logout"));
}
