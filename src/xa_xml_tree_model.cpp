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


XAXMLTreeItem::XAXMLTreeItem(XAXMLTreeItem* parent_item)
{

}


XAXMLTreeModel::XAXMLTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{

}

XAXMLTreeModel::~XAXMLTreeModel()
{

}

QVariant XAXMLTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    //TreeItem* item = static_cast<TreeItem*>(index.internalPointer());

    //return item->data(index.column());
    return {};
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

    //TreeItem* parentItem;

    //if (!parent.isValid())
    //    parentItem = rootItem;
    //else
    //    parentItem = static_cast<TreeItem*>(parent.internalPointer());

    //TreeItem* childItem = parentItem->child(row);
    //if (childItem)
    //    return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex XAXMLTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    return {};
}

int XAXMLTreeModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0)
        return 0;

    return 0;
}

int XAXMLTreeModel::columnCount(const QModelIndex& parent) const
{
    return 0;
}