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

#ifndef ITEMTEXTBUTTON_H_
#define ITEMTEXTBUTTON_H_

#include "interfaces/ISettingsListener.h"
#include "ToolBarItem.h"

class IScreenpic;
class QToolButton;

class ItemTextButton : public ToolBarItem, public ISettingsListener
{
  Q_OBJECT

public:
  ItemTextButton(IScreenpic *screenpic, QWidget *parent = 0);
  ~ItemTextButton();
  void onSettingsChanged(const QString &key, const QVariant &value) override;
  void setChecked(bool checked) override;

protected:
  void retranslateUi() override;

private slots:
  void reload();

private:
  IScreenpic *m_screenpic;
  QToolButton *m_button;
  QAction *m_borderAction;
};

#endif // ITEMTEXTBUTTON_H_
