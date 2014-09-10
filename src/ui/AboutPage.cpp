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
#include <QLabel>
#include <QVBoxLayout>

#include "AboutPage.h"
#include "AppCore.h"
#include "AutoUpdate.h"
#include "Settings.h"
#include "sglobal.h"
#include "version.h"

AboutPage::AboutPage(AppCore *core, QWidget *parent)
  : QWidget(parent)
  , m_core(core)
{
  setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

  m_updateBtn = new QCheckBox(this);
  m_updateBtn->setChecked(core->settings()->value(AutoUpdate::kEnabled).toBool());

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(new QLabel(QString("<h2>%1 %2</h2>").arg(qApp->applicationName(), qApp->applicationVersion()), this));
  layout->addWidget(new QLabel(QString(ORG_COPYRIGHT).replace("(C)", "©"), this));
  layout->addStretch();
  layout->addWidget(m_updateBtn);

  retranslateUi();

  connect(m_updateBtn, SIGNAL(clicked(bool)), SLOT(onUpdateClicked(bool)));
}


void AboutPage::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWidget::changeEvent(event);
}


void AboutPage::showEvent(QShowEvent *event)
{
  QWidget::showEvent(event);

  if (m_core->autoUpdate()->status() != AutoUpdate::UpdateReady)
    m_core->autoUpdate()->check();
}


void AboutPage::onUpdateClicked(bool checked)
{
  m_core->settings()->setValue(AutoUpdate::kEnabled, checked);

  if (checked)
    m_core->autoUpdate()->check();
  else
    m_core->settings()->setValue(AutoUpdate::kReady, false);
}


void AboutPage::retranslateUi()
{
  m_updateBtn->setText(tr("Automatic update"));
}
