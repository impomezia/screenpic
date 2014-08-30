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

#include <QGraphicsItem>

#include "EditorScene.h"
#include "items/EditorItem.h"
#include "items/ItemCommand.h"

ItemCommand::ItemCommand(EditorScene *scene, const QPointF &point, EditorItem *creator)
  : IItemCommand()
  , m_removed(false)
  , m_point(point)
  , m_creator(creator)
  , m_scene(scene)
  , m_item(0)
{
}


ItemCommand::~ItemCommand()
{
  if (m_removed && m_item)
    delete m_item;
}


bool ItemCommand::validate()
{
  return true;
}


void ItemCommand::redo()
{
  m_removed = false;

  if (m_item)
    m_scene->addItem(m_item);
  else
    create();
}


void ItemCommand::undo()
{
  m_removed = true;

  if (m_item)
    m_scene->removeItem(m_item);
}


void ItemCommand::create()
{
  if (!m_creator)
    return;

  m_item = m_creator->create(m_scene, m_point);
}
