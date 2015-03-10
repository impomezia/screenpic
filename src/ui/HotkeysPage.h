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

#ifndef HOTKEYSPAGE_H_
#define HOTKEYSPAGE_H_

#include <QMap>
#include <QWidget>

class AppCore;
class QCheckBox;
class QGridLayout;
class QLabel;
class QPushButton;

class HotkeysPage : public QWidget
{
  Q_OBJECT

public:
  HotkeysPage(AppCore *core, QWidget *parent = 0);

protected:
  void changeEvent(QEvent *event) override;

private slots:
  void edit();
  void onChanged(const QString &id, const QKeySequence &shortcut);
  void onClicked(bool checked);

private:
  struct Row {
    inline Row() : checkBox(0), btn(0) {}

    QCheckBox *checkBox;
    QPushButton *btn;
  };

  QKeySequence shortcut(const QString &id) const;
  QString toHuman(const QKeySequence &seq) const;
  void init(QGridLayout *layout, const QString &id, const QString &name);
  void reload(const QString &id);
  void retranslateUi();

  AppCore *m_core;
  QLabel *m_primaryLabel;
  QMap<QString, Row> m_rows;
};

#endif /* HOTKEYSPAGE_H_ */
