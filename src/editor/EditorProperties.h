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

#ifndef EDITORPROPERTIES_H_
#define EDITORPROPERTIES_H_

#include "ui/DialogFrame.h"

class QCheckBox;
class IScreenpic;

class EditorProperties : public DialogFrame
{
  Q_OBJECT

public:
  EditorProperties(IScreenpic *screenpic, QWidget *parent = 0);
  void retranslateUi() override;

private slots:
  void reload();

private:
  IScreenpic *m_screenpic;
  QCheckBox *m_publishOnCloseBtn;
};

#endif // EDITORPROPERTIES_H_
