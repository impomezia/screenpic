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

#ifndef SERVERSPAGE_H_
#define SERVERSPAGE_H_

#include <QWidget>

class AppCore;
class QComboBox;
class QLabel;
class QStackedWidget;

class ServersPage : public QWidget
{
  Q_OBJECT

public:
  ServersPage(AppCore *core, QWidget *parent = 0);

signals:
  void adjustSizeRequest();

protected:
  void changeEvent(QEvent *event) override;

private slots:
  void onFailbackChanged(int index);
  void onIndexChanged(int index);

private:
  void retranslateUi();

  AppCore *m_core;
  QComboBox *m_failbackCombo;
  QComboBox *m_serversCombo;
  QLabel *m_failbackLabel;
  QLabel *m_primaryLabel;
  QStackedWidget *m_stackedWidget;
};

#endif /* SERVERSPAGE_H_ */
