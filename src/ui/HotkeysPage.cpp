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
#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

#include "AppCore.h"
#include "GlobalShortcuts.h"
#include "HotkeyEditor.h"
#include "HotkeysPage.h"
#include "libqxt/qxtglobalshortcut.h"
#include "Settings.h"
#include "sglobal.h"

HotkeysPage::HotkeysPage(AppCore *core, QWidget *parent)
  : QWidget(parent)
  , m_core(core)
{
  m_primaryLabel = new QLabel(this);

  QGridLayout *primaryLay = new QGridLayout;
  primaryLay->setContentsMargins(10, 0, 0, 0);
  primaryLay->setColumnStretch(0, 1);

  init(primaryLay, GlobalShortcuts::kRectangle,  tr("Capture rectangle"));
  init(primaryLay, GlobalShortcuts::kFullscreen, tr("Capture fullscreen"));

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(m_primaryLabel);
  layout->addLayout(primaryLay);
  layout->addStretch();

  retranslateUi();
}


void HotkeysPage::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWidget::changeEvent(event);
}


void HotkeysPage::edit()
{
  QPushButton *btn = qobject_cast<QPushButton*>(sender());
  if (!btn)
    return;

  const QString id = btn->property("id").toString();
  HotkeyEditor *editor = new HotkeyEditor(id, shortcut(id), m_core->settings()->defaultValue(id).toString(), this);
  editor->open();
  editor->raise();
  editor->activateWindow();

  connect(editor, SIGNAL(changed(QString,QKeySequence)), SLOT(onChanged(QString,QKeySequence)));
}


void HotkeysPage::onChanged(const QString &id, const QKeySequence &shortcut)
{
  m_core->shortcuts()->set(id, shortcut);
  reload(id);
}


void HotkeysPage::onClicked(bool checked)
{
  QCheckBox *checkBox = qobject_cast<QCheckBox*>(sender());
  if (!checkBox)
    return;

  const QString id = checkBox->property("id").toString();
  if (checked)
    m_core->shortcuts()->set(id, shortcut(id));
  else
    m_core->shortcuts()->unset(id, shortcut(id));

  reload(id);
}


QKeySequence HotkeysPage::shortcut(const QString &id) const
{
  QxtGlobalShortcut *shortcut = m_core->shortcuts()->get(id);
  if (shortcut)
    return shortcut->shortcut();

  const QString str = m_core->settings()->value(id).toString();
  return str.startsWith(LC('-')) ? str.mid(1) : str;
}


QString HotkeysPage::toHuman(const QKeySequence &seq) const
{
  QString key = seq.toString();
  key.replace(LS("Meta"),   LS("Win"));
  key.replace(LS("Print"),  LS("Print Screen"));
  key.replace(LS("PgUp"),   LS("Page Up"));
  key.replace(LS("PgDown"), LS("Page Down"));
  key.replace(LS("Ins"),    LS("Insert"));
  key.replace(LS("Del"),    LS("Delete"));
  key.replace(LS("F20"),    LS("Num Pad 0"));
  key.replace(LS("F21"),    LS("Num Pad 1"));
  key.replace(LS("F22"),    LS("Num Pad 2"));
  key.replace(LS("F23"),    LS("Num Pad 3"));
  key.replace(LS("F24"),    LS("Num Pad 4"));
  key.replace(LS("F25"),    LS("Num Pad 5"));
  key.replace(LS("F26"),    LS("Num Pad 6"));
  key.replace(LS("F27"),    LS("Num Pad 7"));
  key.replace(LS("F28"),    LS("Num Pad 8"));
  key.replace(LS("F29"),    LS("Num Pad 9"));

  return key;
}


void HotkeysPage::init(QGridLayout *layout, const QString &id, const QString &name)
{
  Row row;
  row.checkBox = new QCheckBox(name, this);
  row.checkBox->setProperty("id", id);

  row.btn = new QPushButton(this);
  row.btn->setAutoDefault(false);
  row.btn->setProperty("id", id);

  layout->addWidget(row.checkBox, m_rows.size(), 0);
  layout->addWidget(row.btn,      m_rows.size(), 1);

  m_rows.insert(id, row);
  reload(id);

  connect(row.checkBox, SIGNAL(clicked(bool)), SLOT(onClicked(bool)));
  connect(row.btn, SIGNAL(clicked()), SLOT(edit()));
}


void HotkeysPage::reload(const QString &id)
{
  if (!m_rows.contains(id))
    return;

  const Row &row = m_rows[id];
  row.checkBox->setChecked(m_core->shortcuts()->get(id));
  row.btn->setText(toHuman(shortcut(id)));

  if (row.btn->text().isEmpty())
    row.btn->setText(tr("None"));
}


void HotkeysPage::retranslateUi()
{
  m_primaryLabel->setText(LS("<b>") + tr("Global shortcuts") + LS("</b>"));

  m_rows.value(GlobalShortcuts::kRectangle).checkBox->setText(tr("Capture rectangle"));
  m_rows.value(GlobalShortcuts::kFullscreen).checkBox->setText(tr("Capture fullscreen"));
}
