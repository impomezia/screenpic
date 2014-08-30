/*   $Id$
 *   Copyright (C) 2013 Alexander Sedov <imp@schat.me>
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

#ifndef HOTKEYEDITOR_H_
#define HOTKEYEDITOR_H_

#include <QDialog>

class QCheckBox;
class QComboBox;
class QPushButton;

class HotkeyEditor : public QDialog
{
  Q_OBJECT

public:
  HotkeyEditor(const QString &id, const QKeySequence &shortcut, const QKeySequence &defaultShortcut, QWidget *parent = 0);
  void set(const QKeySequence &shortcut);

signals:
  void changed(const QString &id, const QKeySequence &shortcut);

protected:
  void changeEvent(QEvent *event);

private slots:
  void apply();
  void clear();
  void reset();

private:
  void retranslateUi();

  QCheckBox *m_altKey;
  QCheckBox *m_ctrlKey;
  QCheckBox *m_metaKey;
  QCheckBox *m_shiftKey;
  QComboBox *m_comboBox;
  QKeySequence m_defaultShortcut;
  QPushButton *m_cancelBtn;
  QPushButton *m_clearBtn;
  QPushButton *m_okBtn;
  QPushButton *m_resetBtn;
  QString m_id;
};

#endif /* HOTKEYEDITOR_H_ */
