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

#include <QApplication>
#include <QCheckBox>
#include <QDir>
#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "AppCore.h"
#include "Settings.h"
#include "SettingsPage.h"
#include "sglobal.h"

SettingsPage::SettingsPage(AppCore *core, QWidget *parent)
  : QWidget(parent)
  , m_core(core)
{
  m_autoRun = new QCheckBox(this);
  m_autoRun->setChecked(isAutoRun());

  m_editorBtn = new QCheckBox(this);
  m_editorBtn->setChecked(!m_core->settings()->value(Settings::kEditor).toBool());

  m_captureMouse = new QCheckBox(this);
  m_captureMouse->setChecked(m_core->settings()->value(Settings::kCaptureMouse).toBool());

  m_saveCopy  = new QCheckBox(this);
  m_saveCopy->setChecked(m_core->settings()->value(Settings::kSaveCopy).toBool());

  m_fileEdit  = new QLineEdit(QDir::toNativeSeparators(m_core->settings()->value(Settings::kSaveCopyIn).toString()), this);
  m_fileEdit->setVisible(m_saveCopy->isChecked());

  m_browseBtn = new QPushButton(this);
  m_browseBtn->setAutoDefault(false);
  m_browseBtn->setVisible(m_saveCopy->isChecked());

# ifdef Q_OS_MAC
  m_autoRun->hide();
  m_captureMouse->hide();
# endif

  QHBoxLayout *saveLay = new QHBoxLayout();
  saveLay->addWidget(m_fileEdit);
  saveLay->addWidget(m_browseBtn);
  saveLay->setContentsMargins(18, 0, 0, 0);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(m_autoRun);
  layout->addWidget(m_editorBtn);
  layout->addWidget(m_captureMouse);
  layout->addWidget(m_saveCopy);
  layout->addLayout(saveLay);
  layout->addStretch();

  connect(m_autoRun, SIGNAL(clicked(bool)), SLOT(onAutoRunClicked(bool)));
  connect(m_editorBtn, SIGNAL(clicked(bool)), SLOT(onEditorClicked(bool)));
  connect(m_captureMouse, SIGNAL(clicked(bool)), SLOT(onCaptureMouseClicked(bool)));
  connect(m_saveCopy, SIGNAL(clicked(bool)), SLOT(onSaveCopyClicked(bool)));
  connect(m_browseBtn, SIGNAL(clicked()), SLOT(browse()));
  connect(m_fileEdit, SIGNAL(textChanged(QString)), SLOT(onPathChanged(QString)));

  retranslateUi();
}


void SettingsPage::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWidget::changeEvent(event);
}


void SettingsPage::browse()
{
  const QString dir = QFileDialog::getExistingDirectory(this, QString(), m_fileEdit->text());
  if (dir.isEmpty())
    return;

  m_core->settings()->setValue(Settings::kSaveCopyIn, QDir::fromNativeSeparators(dir));
  m_fileEdit->setText(dir);
}


void SettingsPage::onAutoRunClicked(bool checked)
{
  QSettings reg(LS("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"), QSettings::NativeFormat);
  if (checked)
    reg.setValue(QApplication::applicationName(), QDir::toNativeSeparators(QApplication::applicationFilePath()));
  else
    reg.remove(QApplication::applicationName());
}


void SettingsPage::onCaptureMouseClicked(bool checked)
{
  m_core->settings()->setValue(Settings::kCaptureMouse, checked);
}


void SettingsPage::onEditorClicked(bool checked)
{
  m_core->settings()->setValue(Settings::kEditor, !checked);
}


void SettingsPage::onPathChanged(const QString &text)
{
  if (text.isEmpty())
    return;

  m_core->settings()->setValue(Settings::kSaveCopyIn, QDir::fromNativeSeparators(text));
}


void SettingsPage::onSaveCopyClicked(bool checked)
{
  m_fileEdit->setVisible(checked);
  m_browseBtn->setVisible(checked);

  m_core->settings()->setValue(Settings::kSaveCopy, checked);

  adjustSize();
  emit adjustSizeRequest();
}


bool SettingsPage::isAutoRun() const
{
  QSettings reg(LS("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"), QSettings::NativeFormat);
  const QString value = reg.value(QApplication::applicationName(), "").toString();
  if (value.isEmpty())
    return false;

  return QApplication::applicationFilePath() == QDir::fromNativeSeparators(value);
}


void SettingsPage::retranslateUi()
{
  m_autoRun->setText(tr("Run at Windows start up"));
  m_editorBtn->setText(tr("Skip editor window (instant upload)"));
  m_captureMouse->setText(tr("Capture mouse cursor"));
  m_saveCopy->setText(tr("Save screenshot copy in selected folder"));
  m_browseBtn->setText(tr("Browse..."));
}
