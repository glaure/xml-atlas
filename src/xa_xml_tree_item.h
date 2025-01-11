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

#include <QIcon>
#include <QVariant>
#include <QVector>
#include <pugixml.hpp>

enum class XAXMLTreeItemType
{
    ELEMENT,
    ATTRIBUTE
};



class XAXMLTreeItem
{
public:
    explicit XAXMLTreeItem(const std::string& value);
    explicit XAXMLTreeItem(const std::string& value,
        const pugi::xml_node& node,
        XAXMLTreeItemType item_type,
        XAXMLTreeItem* parent_item = nullptr);
    ~XAXMLTreeItem();

    XAXMLTreeItem* appendChild(XAXMLTreeItem* child);
    XAXMLTreeItem* child(int row);
    int childCount() const;
    int columnCount() const;
    
    QVariant data(int column) const;
    
    int row() const;
    XAXMLTreeItem* parentItem();

    uint64_t getOffset() const;

    pugi::xml_node getNode();
private:

private:
    QVector<XAXMLTreeItem*> m_child_items;
    XAXMLTreeItem* m_parent_item;
    std::string m_value;
    pugi::xml_node m_node;
    XAXMLTreeItemType m_item_type;
};