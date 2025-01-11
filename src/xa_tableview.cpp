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

XATableView::XATableView(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    m_tableWidget = new QTableWidget(this);
    layout->addWidget(m_tableWidget);
    setLayout(layout);
}

void XATableView::setTableRootNode(pugi::xml_node node)
{
    populateTable(node);
}

void XATableView::populateTable(pugi::xml_node node)
{
    m_tableWidget->clear();
    m_tableWidget->setRowCount(1);
    m_tableWidget->setColumnCount(1);

    QStringList headers;
    headers << "Tag";

    // Traverse the XML nodes to extract tag names, attributes, and child elements
    int row = 0;
    for (pugi::xml_node child : node.children())
    {
        int col = 0;
        m_tableWidget->insertRow(row);
        std::string name = child.name();

        // Add tag name
        QTableWidgetItem* tagItem = new QTableWidgetItem(name.c_str());
        m_tableWidget->setItem(row, col, tagItem);
        col++;

        // Add attributes
        for (pugi::xml_attribute attr : child.attributes())
        {
            QString attrName = attr.name();
            QString attrValue = attr.value();

            if (!headers.contains(attrName))
            {
                headers << attrName;
                m_tableWidget->setColumnCount(headers.size());
            }

            int attrCol = headers.indexOf(attrName);
            QTableWidgetItem* attrItem = new QTableWidgetItem(attrValue);
            m_tableWidget->setItem(row, attrCol, attrItem);
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
                    m_tableWidget->setColumnCount(headers.size());
                }
                int grandChildCol = headers.indexOf(grandChildName);
                QTableWidgetItem* grandChildItem = new QTableWidgetItem(grandChildValue);
                m_tableWidget->setItem(row, grandChildCol, grandChildItem);
            } break;
            
            case pugi::node_element:
            {
                QString grandChildName = grandChild.name();
                QString grandChildValue = grandChild.child_value();

                if (!headers.contains(grandChildName))
                {
                    headers << grandChildName;
                    m_tableWidget->setColumnCount(headers.size());
                }

                int grandChildCol = headers.indexOf(grandChildName);
                QTableWidgetItem* grandChildItem = new QTableWidgetItem(grandChildValue);
                m_tableWidget->setItem(row, grandChildCol, grandChildItem);
            } break;

            default:
                break;
            }

        }

        row++;
    }

    m_tableWidget->setHorizontalHeaderLabels(headers);
}
