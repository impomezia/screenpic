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
class Spinner;

class ImgurSettings : public QWidget
{
  Q_OBJECT

public:
  ImgurSettings(QWidget *parent = 0);
  void setError(const QString &errorText);
  void setSuccess(const QString &username);

signals:
  void logout();
  void pinReady(const QString &pin);
  void pinRequest();

protected:
  void changeEvent(QEvent *event) override;

private slots:
  void onCompleteClicked();
  void onLogout();
  void onPaste();
  void onTextChanged(const QString &text);
  void queuedAdjustSize();

private:
  void retranslateUi();

  QGroupBox *m_accountGroup;
  QGroupBox *m_loginGroup;
  QLabel *m_errorLabel;
  QLabel *m_firstStepLabel;
  QLabel *m_secondStepLabel;
  QLabel *m_thirdStepLabel;
  QLabel *m_userNameLabel;
  QLabel *m_userNameValueLabel;
  QLineEdit *m_pinEdit;
  QPushButton *m_completeBtn;
  QPushButton *m_logoutBtn;
  QPushButton *m_openPageBtn;
  QToolButton *m_pasteBtn;
  Spinner *m_spinner;
};

#endif // IMGURSETTINGS_H_
