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


#include "xa_xml_tree_model.h"
#include "xa_xml_tree_item.h"
#include "xa_theme.h"

#include <QIcon>


XAXMLTreeModel::XAXMLTreeModel(XATheme* theme, QObject* parent)
    : QAbstractItemModel(parent)
    , m_theme(theme)
{
    m_root_item = new XAXMLTreeItem{ "ROOT"};
}

XAXMLTreeModel::~XAXMLTreeModel()
{
    delete m_root_item;
}

QVariant XAXMLTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
    {
        auto item = static_cast<XAXMLTreeItem*>(index.internalPointer());
        auto data_value = item->data(index.column());
        return data_value;
    }

    case Qt::DecorationRole:
    {
        auto item = static_cast<XAXMLTreeItem*>(index.internalPointer());
        static QIcon ic_light_element_children = QIcon(":/xml/images/light/element-children.png");
        static QIcon ic_light_element_empty = QIcon(":/xml/images/light/element-empty.png");
        static QIcon ic_light_element_text = QIcon(":/xml/images/light/element-text.png");
        static QIcon ic_light_error_mark = QIcon(":/xml/images/light/mark.png");
        static QIcon ic_dark_element_children = QIcon(":/xml/images/dark/element-children.png");
        static QIcon ic_dark_element_empty = QIcon(":/xml/images/dark/element-empty.png");
        static QIcon ic_dark_element_text = QIcon(":/xml/images/dark/element-text.png");
        static QIcon ic_dark_error_mark = QIcon(":/xml/images/light/mark.png");
        
        if (m_theme->getColorTheme() == "dark")
        {
            if (item->getItemType() == XAXMLTreeItemType::ERROR)
            {
                return ic_dark_error_mark;
            }

            switch (item->childCount())
            {
            case 0:  return ic_dark_element_empty;
            case 1:  return ic_dark_element_text;
            default: return ic_dark_element_children;
            }
        }
        else
        {
            if (item->getItemType() == XAXMLTreeItemType::ERROR)
            {
                return ic_light_error_mark;
            }

            switch (item->childCount())
            {
            case 0:  return ic_light_element_empty;
            case 1:  return ic_light_element_text;
            default: return ic_light_element_children;
            }
        }
    }
    
    default:
        break;
    }

    return QVariant();
}

Qt::ItemFlags XAXMLTreeModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QModelIndex XAXMLTreeModel::index(int row, int column,
    const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    XAXMLTreeItem* parent_item;

    if (!parent.isValid()) 
    {
        parent_item = m_root_item;
    }
    else 
    {
        parent_item = static_cast<XAXMLTreeItem*>(parent.internalPointer());
    }

    auto* child_item = parent_item->child(row);
    if (child_item)
        return createIndex(row, column, child_item);
    return QModelIndex();
}

QModelIndex XAXMLTreeModel::indexFromItem(XAXMLTreeItem* item) const
{
    if (!item || item == m_root_item)
        return QModelIndex();

    XAXMLTreeItem* parent_item = item->parent();
    if (!parent_item)
        return QModelIndex();

    int row = parent_item->children().indexOf(item);
    return createIndex(row, 0, item);
}

QModelIndex XAXMLTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    auto child_item = static_cast<XAXMLTreeItem*>(index.internalPointer());
    if (!child_item)
        return QModelIndex();

    auto parent_item = child_item->parentItem();

    if (parent_item == m_root_item)
        return QModelIndex();

    return createIndex(parent_item->row(), 0, parent_item);
}

int XAXMLTreeModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0)
        return 0;

    XAXMLTreeItem* parent_item;

    if (!parent.isValid())
        parent_item = m_root_item;
    else
        parent_item = static_cast<XAXMLTreeItem*>(parent.internalPointer());

    return parent_item->childCount();
}

int XAXMLTreeModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return static_cast<XAXMLTreeItem*>(parent.internalPointer())->columnCount();
    return m_root_item->columnCount();
}

XAXMLTreeItem* XAXMLTreeModel::rootItem() const
{
    return m_root_item;
}

void XAXMLTreeModel::updateAll()
{
    //QModelIndex topLeft = createIndex(1, 0);
    //QModelIndex bottomRight = createIndex(3, 0);
    //emit dataChanged(topLeft, bottomRight);
}

void XAXMLTreeModel::beginFillModel()
{
    beginResetModel();
}

void XAXMLTreeModel::endFillModel()
{
    endResetModel();
}

void XAXMLTreeModel::clear()
{
    beginResetModel();
    delete m_root_item;
    m_root_item = new XAXMLTreeItem{ "ROOT" };
    endResetModel();
}