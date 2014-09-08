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
#include <QComboBox>
#include <QEvent>
#include <QHBoxLayout>
#include <QPushButton>

#include "HotkeyEditor.h"

HotkeyEditor::HotkeyEditor(const QString &id, const QKeySequence &shortcut, const QKeySequence &defaultShortcut, QWidget *parent)
  : QDialog(parent, Qt::Dialog | Qt::Tool)
  , m_defaultShortcut(defaultShortcut)
  , m_id(id)
{
  setAttribute(Qt::WA_DeleteOnClose);

# ifdef Q_OS_MAC
  m_metaKey  = new QCheckBox(tr("⌃"),    this);
  m_ctrlKey  = new QCheckBox(tr("⌘"),    this);
  m_altKey   = new QCheckBox(tr("⌥"),    this);
  m_shiftKey = new QCheckBox(tr("⇧"),    this);
# else
  m_metaKey  = new QCheckBox(tr("Win"),   this);
  m_ctrlKey  = new QCheckBox(tr("Ctrl"),  this);
  m_altKey   = new QCheckBox(tr("Alt"),   this);
  m_shiftKey = new QCheckBox(tr("Shift"), this);
# endif

  m_comboBox = new QComboBox(this);
  m_comboBox->setMaxVisibleItems(50);

  m_comboBox->addItem(tr("None"));
# ifndef Q_OS_MAC
  m_comboBox->addItem("Print Screen", Qt::Key_Print);
# endif
  m_comboBox->addItem("Insert",       Qt::Key_Insert);
  m_comboBox->addItem("Delete",       Qt::Key_Delete);
  m_comboBox->addItem("Home",         Qt::Key_Home);
  m_comboBox->addItem("End",          Qt::Key_End);
  m_comboBox->addItem("Page Up",      Qt::Key_PageUp);
  m_comboBox->addItem("Page Down",    Qt::Key_PageDown);
  m_comboBox->addItem("Space",        Qt::Key_Space);
  m_comboBox->addItem("Backspace",    Qt::Key_Backspace);
  m_comboBox->addItem("Left",         Qt::Key_Left);
  m_comboBox->addItem("Up",           Qt::Key_Up);
  m_comboBox->addItem("Right",        Qt::Key_Right);
  m_comboBox->addItem("Down",         Qt::Key_Down);
  m_comboBox->addItem("A",            Qt::Key_A);
  m_comboBox->addItem("B",            Qt::Key_B);
  m_comboBox->addItem("C",            Qt::Key_C);
  m_comboBox->addItem("D",            Qt::Key_D);
  m_comboBox->addItem("E",            Qt::Key_E);
  m_comboBox->addItem("F",            Qt::Key_F);
  m_comboBox->addItem("G",            Qt::Key_G);
  m_comboBox->addItem("H",            Qt::Key_H);
  m_comboBox->addItem("I",            Qt::Key_I);
  m_comboBox->addItem("J",            Qt::Key_J);
  m_comboBox->addItem("K",            Qt::Key_K);
  m_comboBox->addItem("L",            Qt::Key_L);
  m_comboBox->addItem("M",            Qt::Key_M);
  m_comboBox->addItem("N",            Qt::Key_N);
  m_comboBox->addItem("O",            Qt::Key_O);
  m_comboBox->addItem("P",            Qt::Key_P);
  m_comboBox->addItem("Q",            Qt::Key_Q);
  m_comboBox->addItem("R",            Qt::Key_R);
  m_comboBox->addItem("S",            Qt::Key_S);
  m_comboBox->addItem("T",            Qt::Key_T);
  m_comboBox->addItem("U",            Qt::Key_U);
  m_comboBox->addItem("V",            Qt::Key_V);
  m_comboBox->addItem("W",            Qt::Key_W);
  m_comboBox->addItem("X",            Qt::Key_X);
  m_comboBox->addItem("Y",            Qt::Key_Y);
  m_comboBox->addItem("Z",            Qt::Key_Z);
  m_comboBox->addItem("0",            Qt::Key_0);
  m_comboBox->addItem("1",            Qt::Key_1);
  m_comboBox->addItem("2",            Qt::Key_2);
  m_comboBox->addItem("3",            Qt::Key_3);
  m_comboBox->addItem("4",            Qt::Key_4);
  m_comboBox->addItem("5",            Qt::Key_5);
  m_comboBox->addItem("6",            Qt::Key_6);
  m_comboBox->addItem("7",            Qt::Key_7);
  m_comboBox->addItem("8",            Qt::Key_8);
  m_comboBox->addItem("9",            Qt::Key_9);
  m_comboBox->addItem("Num Pad 0",    Qt::Key_F20);
  m_comboBox->addItem("Num Pad 1",    Qt::Key_F21);
  m_comboBox->addItem("Num Pad 2",    Qt::Key_F22);
  m_comboBox->addItem("Num Pad 3",    Qt::Key_F23);
  m_comboBox->addItem("Num Pad 4",    Qt::Key_F24);
  m_comboBox->addItem("Num Pad 5",    Qt::Key_F25);
  m_comboBox->addItem("Num Pad 6",    Qt::Key_F26);
  m_comboBox->addItem("Num Pad 7",    Qt::Key_F27);
  m_comboBox->addItem("Num Pad 8",    Qt::Key_F28);
  m_comboBox->addItem("Num Pad 9",    Qt::Key_F29);
  m_comboBox->addItem("/",            Qt::Key_Slash);
  m_comboBox->addItem("*",            Qt::Key_Asterisk);
  m_comboBox->addItem("-",            Qt::Key_Minus);
  m_comboBox->addItem("+",            Qt::Key_Plus);
  m_comboBox->addItem("F1",           Qt::Key_F1);
  m_comboBox->addItem("F2",           Qt::Key_F2);
  m_comboBox->addItem("F3",           Qt::Key_F3);
  m_comboBox->addItem("F4",           Qt::Key_F4);
  m_comboBox->addItem("F5",           Qt::Key_F5);
  m_comboBox->addItem("F6",           Qt::Key_F6);
  m_comboBox->addItem("F7",           Qt::Key_F7);
  m_comboBox->addItem("F8",           Qt::Key_F8);
  m_comboBox->addItem("F9",           Qt::Key_F9);
  m_comboBox->addItem("F10",          Qt::Key_F10);
  m_comboBox->addItem("F11",          Qt::Key_F11);
  m_comboBox->addItem("F12",          Qt::Key_F12);

  m_okBtn     = new QPushButton(this);
  m_cancelBtn = new QPushButton(this);
  m_resetBtn  = new QPushButton(this);
  m_clearBtn  = new QPushButton(this);

  QHBoxLayout *keysLay = new QHBoxLayout();
  keysLay->addWidget(m_metaKey);
  keysLay->addWidget(m_ctrlKey);
  keysLay->addWidget(m_altKey);
  keysLay->addWidget(m_shiftKey);
  keysLay->addWidget(m_comboBox, 1);

  QHBoxLayout *btnLay = new QHBoxLayout();
  btnLay->addWidget(m_okBtn);
  btnLay->addWidget(m_cancelBtn);
  btnLay->addWidget(m_resetBtn);
  btnLay->addWidget(m_clearBtn);
  btnLay->setContentsMargins(0, 10, 0, 0);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addLayout(keysLay);
  layout->addLayout(btnLay);

  set(shortcut);

  connect(m_okBtn, SIGNAL(clicked()), SLOT(apply()));
  connect(m_cancelBtn, SIGNAL(clicked()), SLOT(close()));
  connect(m_resetBtn, SIGNAL(clicked()), SLOT(reset()));
  connect(m_clearBtn, SIGNAL(clicked()), SLOT(clear()));

  retranslateUi();
}


void HotkeyEditor::set(const QKeySequence &shortcut)
{
  const uint allMods = Qt::SHIFT | Qt::CTRL | Qt::ALT | Qt::META;
  const uint key     = shortcut.isEmpty() ? Qt::Key(0) : Qt::Key((shortcut[0] ^ allMods) & shortcut[0]);
  const uint mods    = shortcut.isEmpty() ? Qt::KeyboardModifiers(0) : Qt::KeyboardModifiers(shortcut[0] & allMods);

  m_metaKey->setChecked(mods  & Qt::META);
  m_ctrlKey->setChecked(mods  & Qt::CTRL);
  m_altKey->setChecked(mods   & Qt::ALT);
  m_shiftKey->setChecked(mods & Qt::SHIFT);

  const int index = m_comboBox->findData(key);
  m_comboBox->setCurrentIndex(index == -1 ? 0 : index);
}


void HotkeyEditor::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QDialog::changeEvent(event);
}


void HotkeyEditor::apply()
{
  int key = m_comboBox->itemData(m_comboBox->currentIndex()).toInt();
  if (key) {
    if (m_metaKey->isChecked())
      key |= Qt::META;

    if (m_ctrlKey->isChecked())
      key |= Qt::CTRL;

    if (m_altKey->isChecked())
      key |= Qt::ALT;

    if (m_shiftKey->isChecked())
      key |= Qt::SHIFT;
  }

  emit changed(m_id, key);
  close();
}


void HotkeyEditor::clear()
{
  set(QKeySequence());
}


void HotkeyEditor::reset()
{
  set(m_defaultShortcut);
}


void HotkeyEditor::retranslateUi()
{
  setWindowTitle(tr("Edit Hotkey"));

  m_comboBox->setItemText(0, tr("None"));

  m_okBtn->setText(tr("OK"));
  m_cancelBtn->setText(tr("Cancel"));
  m_resetBtn->setText(tr("Reset"));
  m_clearBtn->setText(tr("Clear"));
}
