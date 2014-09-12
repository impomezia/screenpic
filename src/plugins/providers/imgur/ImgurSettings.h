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

#ifndef IMGURSETTINGS_H_
#define IMGURSETTINGS_H_

#include <QWidget>

class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QToolButton;

class ImgurSettings : public QWidget
{
  Q_OBJECT

public:
  ImgurSettings(QWidget *parent = 0);

signals:
  void pinReady(const QString &pin);
  void pinRequest();

protected:
  void changeEvent(QEvent *event) override;

private slots:
  void onCompleteClicked();
  void onPaste();

private:
  void retranslateUi();

  QGroupBox *m_accountGroup;
  QLabel *m_firstStepLabel;
  QLabel *m_secondStepLabel;
  QLabel *m_thirdStepLabel;
  QLineEdit *m_pinEdit;
  QPushButton *m_completeBtn;
  QPushButton *m_openPageBtn;
  QToolButton *m_pasteBtn;
};

#endif // IMGURSETTINGS_H_
