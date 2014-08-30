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

#include <QComboBox>
#include <QEvent>
#include <QGridLayout>
#include <QStackedWidget>

#include "AppCore.h"
#include "interfaces/IProvider.h"
#include "Providers.h"
#include "ServersPage.h"
#include "Settings.h"

ServersPage::ServersPage(AppCore *core, QWidget *parent)
  : QWidget(parent)
  , m_core(core)
{
  m_serversCombo = new QComboBox(this);
  m_stackedWidget = new QStackedWidget(this);

  Providers *providers = core->providers();

  foreach (const QString &id, providers->list()) {
    IProvider *provider = providers->get(id);
    if (provider) {
      m_serversCombo->addItem(provider->icon(), provider->name(), provider->id());

      QWidget *widget = provider->settingsWidget(this);
      m_stackedWidget->addWidget(widget ? widget : new QWidget(this));
    }
  }

  const int index = m_serversCombo->findData(providers->currentId());
  if (index != -1)
    m_serversCombo->setCurrentIndex(index);

  QGridLayout *layout = new QGridLayout(this);
  layout->addWidget(m_serversCombo, 0, 0);
  layout->addWidget(m_stackedWidget, 1, 0, 1, 2);
  layout->setColumnStretch(1, 1);
  layout->setRowStretch(2, 1);

  m_stackedWidget->setCurrentIndex(m_serversCombo->currentIndex());

  connect(m_serversCombo, SIGNAL(currentIndexChanged(int)), SLOT(onIndexChanged(int)));
}


void ServersPage::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWidget::changeEvent(event);
}


void ServersPage::onIndexChanged(int index)
{
  m_stackedWidget->setCurrentIndex(index);
  m_core->settings()->setValue(Settings::kProvider, m_serversCombo->itemData(index));
}


void ServersPage::retranslateUi()
{
}
