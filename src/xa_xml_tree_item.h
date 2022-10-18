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

#pragma once

#include <QVariant>
#include <QVector>


enum class XAXMLTreeItemType
{
    ELEMENT,
    ATTRIBUTE
};



class XAXMLTreeItem
{
public:
    explicit XAXMLTreeItem(XAXMLTreeItem* parent_item = nullptr);
    explicit XAXMLTreeItem(const std::string& value, XAXMLTreeItemType item_type, XAXMLTreeItem* parent_item = nullptr);
    ~XAXMLTreeItem();

    XAXMLTreeItem* appendChild(XAXMLTreeItem* child);
    XAXMLTreeItem* child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    XAXMLTreeItem* parentItem();

private:
    QVector<XAXMLTreeItem*> m_child_items;
    XAXMLTreeItem* m_parent_item;
    std::string m_value;
    XAXMLTreeItemType m_item_type;
};