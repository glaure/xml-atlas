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

#include "xa_tableview.h"
#include <QLabel>

XATableView::XATableView(QWidget* parent)
    : QWidget(parent)
    , m_layout(nullptr)
{
    m_layout = new QVBoxLayout(this);
}

void XATableView::setTableRootNode(pugi::xml_node node)
{
    if (m_layout) delete m_layout;
    m_layout = new QVBoxLayout(this);
    m_layout->setSizeConstraint(QLayout::SetMinimumSize);
    setLayout(m_layout);

    populateAttributeTable(node);
    populateTable(node);

}

void XATableView::populateAttributeTable(pugi::xml_node node)
{
    m_tableattributes = new QTableWidget(this);
    m_tableattributes->clear();

    auto attributes = node.attributes();
    auto num_attr = std::distance(attributes.begin(), attributes.end());

    // Add attributes
    if (num_attr > 0)
    {
        auto title = new QLabel(QString("%1 Attributes:").arg(num_attr));
        m_layout->addWidget(title);
        //m_layout->setStretchFactor(title, 1);
        title->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

        int row = 0;
        QStringList headers;
        headers << "Name";
        headers << "Value";

        m_tableattributes->setRowCount(num_attr);
        m_tableattributes->setColumnCount(2);

        for (pugi::xml_attribute attr : attributes)
        {
            QString attrName = attr.name();
            QString attrValue = attr.value();

            QTableWidgetItem* attrNameItem = new QTableWidgetItem(attrName);
            m_tableattributes->setItem(row, 0, attrNameItem);
            title->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

            QTableWidgetItem* attrItem = new QTableWidgetItem(attrValue);
            m_tableattributes->setItem(row, 1, attrItem);
            row++;
        }

        m_tableattributes->resizeRowsToContents();

        m_tableattributes->resizeColumnsToContents();
        m_tableattributes->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        //m_layout->setStretchFactor(m_tableattributes, 3);
        m_tableattributes->setHorizontalHeaderLabels(headers);
        m_layout->addWidget(m_tableattributes);
    }
}

void XATableView::populateTable(pugi::xml_node node)
{
    m_tablechildren = new QTableWidget(this);
    m_tablechildren->clear();

    m_tablechildren->setRowCount(0);
    m_tablechildren->setColumnCount(1);

    QStringList headers;
    headers << "Tag";

    auto children = node.children();
    auto num_children = std::distance(children.begin(), children.end());

    if (num_children > 0)
    {
        auto title = new QLabel(QString("%2 Subtags:").arg(num_children));
        m_layout->addWidget(title);
        //m_layout->setStretchFactor(title, 1);
        title->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    }

    // Traverse the XML nodes to extract tag names, attributes, and child elements
    int row = 0;
    for (pugi::xml_node child : node.children())
    {
        int col = 0;
        m_tablechildren->insertRow(row);
        std::string name = child.name();

        if (!name.empty())
        {
            // Add tag name
            QTableWidgetItem* tagItem = new QTableWidgetItem(name.c_str());
            m_tablechildren->setItem(row, col, tagItem);
            col++;
        }

        // Add attributes
        for (pugi::xml_attribute attr : child.attributes())
        {
            QString attrName = attr.name();
            QString attrValue = attr.value();

            if (!headers.contains(attrName))
            {
                headers << attrName;
                m_tablechildren->setColumnCount(headers.size());
            }

            int attrCol = headers.indexOf(attrName);
            QTableWidgetItem* attrItem = new QTableWidgetItem(attrValue);
            m_tablechildren->setItem(row, attrCol, attrItem);
        }

        // Text
        switch (child.type())
        {
        case pugi::node_pcdata:
        {
            QString grandChildName = "Text";
            QString grandChildValue = child.text().as_string();
            if (row == 0)
            {
                headers.clear();
                headers << grandChildName;
                m_tablechildren->setColumnCount(headers.size());
            }
            int grandChildCol = headers.indexOf(grandChildName);
            QTableWidgetItem* grandChildItem = new QTableWidgetItem(grandChildValue);
            m_tablechildren->setItem(row, grandChildCol, grandChildItem);
        } break;
        default: break;
        }

        // Add child elements
        for (pugi::xml_node grandChild : child.children())
        {
            switch (grandChild.type())
            {
            case pugi::node_pcdata:
            {
                QString grandChildName = "Text";
                QString grandChildValue = grandChild.text().as_string();
                if (row == 0)
                {
                    headers << grandChildName;
                    m_tablechildren->setColumnCount(headers.size());
                }
                int grandChildCol = headers.indexOf(grandChildName);
                QTableWidgetItem* grandChildItem = new QTableWidgetItem(grandChildValue);
                m_tablechildren->setItem(row, grandChildCol, grandChildItem);
            } break;
            
            case pugi::node_element:
            {
                QString grandChildName = grandChild.name();
                QString grandChildValue = grandChild.child_value();

                if (!headers.contains(grandChildName))
                {
                    headers << grandChildName;
                    m_tablechildren->setColumnCount(headers.size());
                }

                int grandChildCol = headers.indexOf(grandChildName);
                QTableWidgetItem* grandChildItem = new QTableWidgetItem(grandChildValue);
                m_tablechildren->setItem(row, grandChildCol, grandChildItem);
            } break;

            default:
                break;
            }

        }

        row++;
    }
    m_layout->addWidget(m_tablechildren);
    //m_layout->setStretchFactor(m_tablechildren, 5);
    m_tablechildren->setHorizontalHeaderLabels(headers);
}
