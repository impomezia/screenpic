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

#ifndef SETTINGSPAGE_H_
#define SETTINGSPAGE_H_

#include <QWidget>

class AppCore;
class QCheckBox;
class QLineEdit;
class QPushButton;

class SettingsPage : public QWidget
{
  Q_OBJECT

public:
  SettingsPage(AppCore *core, QWidget *parent = 0);

signals:
  void adjustSizeRequest();

protected:
  void changeEvent(QEvent *event) override;

private slots:
  void browse();
  void onAutoRunClicked(bool checked);
  void onCaptureMouseClicked(bool checked);
  void onEditorClicked(bool checked);
  void onPathChanged(const QString &text);
  void onSaveCopyClicked(bool checked);

private:
  bool isAutoRun() const;
  void retranslateUi();

  AppCore *m_core;
  QCheckBox *m_autoRun;
  QCheckBox *m_captureMouse;
  QCheckBox *m_editorBtn;
  QCheckBox *m_saveCopy;
  QLineEdit *m_fileEdit;
  QPushButton *m_browseBtn;
};

#endif /* SETTINGSPAGE_H_ */
