/* 
 * This file is part of XMLAtlas (https://github.com/glaure/xml-atlas)
 * Copyright (c) 2022 Gunther Laure <gunther.laure@gmail.com>.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "xa_xml_tree_item.h"


XAXMLTreeItem::XAXMLTreeItem(const std::string& value)
    : m_parent_item(nullptr)
    , m_value()
    , m_offset(0)
    , m_item_type(XAXMLTreeItemType::ELEMENT)
{
}

XAXMLTreeItem::XAXMLTreeItem(const std::string& value, uint64_t offset, XAXMLTreeItemType item_type, XAXMLTreeItem* parent_item)
    : m_parent_item(parent_item)
    , m_value(value)
    , m_offset(offset)
    , m_item_type(item_type)
{
}

XAXMLTreeItem::~XAXMLTreeItem()
{
    qDeleteAll(m_child_items);
}

XAXMLTreeItem* XAXMLTreeItem::appendChild(XAXMLTreeItem* item)
{
    m_child_items.append(item);
    return item;
}

XAXMLTreeItem* XAXMLTreeItem::child(int row)
{
    if (row < 0 || row >= m_child_items.size())
        return nullptr;
    return m_child_items.at(row);
}

int XAXMLTreeItem::childCount() const
{
    return m_child_items.count();
}

int XAXMLTreeItem::columnCount() const
{
    //return m_itemData.count();
    return 2;
}

QVariant XAXMLTreeItem::data(int column) const
{
    if (column < 0 || column >= columnCount())
        return QVariant();
    //return m_itemData.at(column);
    //return "Test";
    switch (column) {
    case 0:     return QString::fromStdString(m_value);
    case 1:     return static_cast<qulonglong>(m_offset);
    default:    return "Error: column";
    }
    return QString::fromStdString(m_value);
}

int XAXMLTreeItem::row() const
{
    if (m_parent_item)
        return m_parent_item->m_child_items.indexOf(const_cast<XAXMLTreeItem*>(this));

    return 0;
}

XAXMLTreeItem* XAXMLTreeItem::parentItem()
{
    return m_parent_item;
}

uint64_t XAXMLTreeItem::getOffset() const
{
    return m_offset;
}
