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

#ifndef TRAYWINDOW_H_
#define TRAYWINDOW_H_

#include <QFrame>
#include <QMap>
#include <QPointer>

class AppCore;
class QLabel;
class QMenu;
class QToolButton;
class RecentView;
class SettingsDialog;

class TrayWindow : public QFrame
{
  Q_OBJECT

public:
  TrayWindow(AppCore *core, QWidget *parent = 0);
  inline bool isPin() const             { return m_pin; }
  inline RecentView *recentView() const { return m_recentView; }

signals:
  void grabRect();
  void grabScreen();
  void openFile();

public slots:
  inline void setPin(bool pin) { m_pin = pin; }

protected:
  void changeEvent(QEvent *event) override;
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dropEvent(QDropEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void showEvent(QShowEvent *event) override;

private slots:
  void onLangChanged(QAction *action);
  void onSiteClicked();
  void showSettings();

private:
  enum Buttons {
    RectangleBtn,
    FullscreenBtn,
    FileBtn
  };

  void addLang(const QString &lang);
  void createMenu();
  void retranslateUi();

  bool m_pin;
  AppCore *m_core;                           ///< Указатель на ядро приложения.
  int m_width;                               ///< Ширина кнопок.
  QAction *m_exitAction;
  QAction *m_settingsAction;
  QFrame *m_controlFrame;
  QFrame *m_linksFrame;
  QFrame *m_topFrame;
  QLabel *m_recentLabel;
  QMap<Buttons, QToolButton*> m_buttons;     ///< Кнопки.
  QMap<QString, QAction*> m_langs;           ///< Языки.
  QMenu *m_langMenu;
  QMenu *m_menu;
  QPointer<SettingsDialog> m_settingsDialog; ///< Диалог настроек.
  QToolButton *m_menuBtn;
  QToolButton *m_siteBtn;
  QToolButton *m_pinBtn;
  RecentView *m_recentView;                  ///< Список недавних файлов.
};

#endif /* TRAYWINDOW_H_ */
