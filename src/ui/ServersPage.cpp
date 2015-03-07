/*   Copyright (C) 2013-2015 Alexander Sedov <imp@schat.me>
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
#include <QLabel>
#include <QStackedWidget>

#include "AppCore.h"
#include "interfaces/IProvider.h"
#include "Providers.h"
#include "ServersPage.h"
#include "Settings.h"
#include "sglobal.h"

ServersPage::ServersPage(AppCore *core, QWidget *parent)
  : QWidget(parent)
  , m_core(core)
{
  setObjectName("ServersPage");
  setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

  m_primaryLabel = new QLabel(this);
  m_failbackLabel = new QLabel(this);

  m_serversCombo = new QComboBox(this);
  m_stackedWidget = new QStackedWidget(this);

  m_failbackCombo = new QComboBox(this);

  Providers *providers = core->providers();

  foreach (const QString &id, providers->list()) {
    IProvider *provider = providers->get(id);
    if (provider) {
      m_serversCombo->addItem(provider->icon(), provider->name(), provider->id());
      m_failbackCombo->addItem(provider->icon(), provider->name(), provider->id());

      QWidget *widget = provider->settingsWidget(this);
      if (!widget)
        widget = new QWidget(this);

      widget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
      m_stackedWidget->addWidget(widget);
    }
  }

  int index = m_serversCombo->findData(providers->currentId());
  if (index != -1)
    m_serversCombo->setCurrentIndex(index);

  index = m_serversCombo->findData(m_core->settings()->value(Settings::kFailbackProvider).toString());
  if (index != -1)
    m_failbackCombo->setCurrentIndex(index);

  QGridLayout *primaryLay = new QGridLayout;
  primaryLay->addWidget(m_serversCombo, 0, 0);
  primaryLay->addWidget(m_stackedWidget, 1, 0, 1, 2);
  primaryLay->setColumnStretch(1, 1);
  primaryLay->setRowStretch(2, 1);
  primaryLay->setContentsMargins(10, 0, 0, 0);

  QGridLayout *failbackLay = new QGridLayout;
  failbackLay->addWidget(m_failbackCombo, 0, 0);
  failbackLay->setColumnStretch(1, 1);
  failbackLay->setRowStretch(2, 1);
  failbackLay->setContentsMargins(10, 0, 0, 0);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(m_primaryLabel);
  layout->addLayout(primaryLay);
  layout->addWidget(m_failbackLabel);
  layout->addLayout(failbackLay);
  layout->addStretch();

  m_stackedWidget->setCurrentIndex(m_serversCombo->currentIndex());
  m_stackedWidget->currentWidget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  connect(m_serversCombo, SIGNAL(currentIndexChanged(int)), SLOT(onIndexChanged(int)));
  connect(m_failbackCombo, SIGNAL(currentIndexChanged(int)), SLOT(onFailbackChanged(int)));

  retranslateUi();
}


void ServersPage::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWidget::changeEvent(event);
}


void ServersPage::onFailbackChanged(int index)
{
  m_core->settings()->setValue(Settings::kFailbackProvider, m_failbackCombo->itemData(index));
}


void ServersPage::onIndexChanged(int index)
{
  m_stackedWidget->setCurrentIndex(index);
  m_core->settings()->setValue(Settings::kProvider, m_serversCombo->itemData(index));

  for (int i = 0; i < m_stackedWidget->count(); ++i) {
      QSizePolicy::Policy policy = QSizePolicy::Ignored;
      if (i == m_stackedWidget->currentIndex())
          policy = QSizePolicy::Expanding;

      m_stackedWidget->widget(i)->setSizePolicy(policy, policy);
  }

  emit adjustSizeRequest();
}


void ServersPage::retranslateUi()
{
  m_primaryLabel->setText(LS("<b>") + tr("Primary server") + LS("</b>"));
  m_failbackLabel->setText(LS("<b>") + tr("Failback server") + LS("</b>"));
}
