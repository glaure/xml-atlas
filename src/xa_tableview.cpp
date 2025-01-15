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
#include <QDebug>
#include <QHeaderView>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
#include <QTableWidget>
#include <QVBoxLayout>


XATableView::XATableView(QWidget* parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout(this))
    , m_table_title(new QLabel("root", this))
    , m_tableattribute_title(new QLabel("Attributes:", this))
    , m_tableattributes(new QTableWidget(this))
    , m_tablechildren_title(new QLabel("Subtags:", this))
    , m_tablechildren(new QTableWidget(this))
    , m_scrollArea(new QScrollArea(this))
{
    setupLayout();
}

void XATableView::setupLayout()
{
    // Disable scrollbars for the tables
    m_tableattributes->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_tableattributes->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_tablechildren->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_tablechildren->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Set size policies to adjust size to content
    m_table_title->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    m_tableattributes->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_tablechildren->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_tablechildren_title->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    // Set a smaller font for the tables
    QFont smallFont = m_tableattributes->font();
    smallFont.setPointSize(8);
    m_tableattributes->setFont(smallFont);
    m_tablechildren->setFont(smallFont);

    QWidget* container = new QWidget(this);
    QVBoxLayout* containerLayout = new QVBoxLayout(container);

    // Center the m_table_title label
    QHBoxLayout* titleLayout = new QHBoxLayout();
    titleLayout->addWidget(m_table_title);
    titleLayout->setAlignment(m_table_title, Qt::AlignCenter);

    containerLayout->addLayout(titleLayout);
    containerLayout->addWidget(m_tableattribute_title);
    containerLayout->addWidget(m_tableattributes);
    containerLayout->addWidget(m_tablechildren_title);
    containerLayout->addWidget(m_tablechildren);
    containerLayout->setAlignment(Qt::AlignTop);
    containerLayout->setSizeConstraint(QLayout::SetMinimumSize);
    m_layout->setSizeConstraint(QLayout::SetMinimumSize);

    container->setLayout(containerLayout);
    m_scrollArea->setWidget(container);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    //m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    m_layout->addWidget(m_scrollArea);
    setLayout(m_layout);

    m_tableattribute_title->hide();
    m_tablechildren_title->hide();
    adjustHeight(m_tableattributes);
}

void XATableView::setTableRootNode(pugi::xml_node node)
{
    m_table_title->setText(QString("%1").arg(node.name()));

    populateAttributeTable(node);
    populateTable(node);
}

void XATableView::populateAttributeTable(const pugi::xml_node& node)
{
    m_tableattributes->clear();

    auto attributes = node.attributes();
    auto num_attr = std::distance(attributes.begin(), attributes.end());

    // Add attributes
    if (num_attr > 0)
    {
        int row = 0;
        QStringList headers;
        headers << "Name";
        headers << "Value";

        m_tableattribute_title->setText(QString("%1 Attributes:").arg(num_attr));
        m_tableattributes->setRowCount(num_attr);
        m_tableattributes->setColumnCount(2);

        QFontMetrics metrics(m_tableattributes->font());

        for (pugi::xml_attribute attr : attributes)
        {
            QString attrName = attr.name();
            QString attrValue = attr.value();

            // Shorten the text if it is too long to fit
            QString elidedAttrName = metrics.elidedText(attrName, Qt::ElideRight, m_tableattributes->columnWidth(0));
            QString elidedAttrValue = metrics.elidedText(attrValue, Qt::ElideRight, m_tableattributes->columnWidth(1));

            QTableWidgetItem* attrNameItem = new QTableWidgetItem(elidedAttrName);
            m_tableattributes->setItem(row, 0, attrNameItem);

            QTableWidgetItem* attrItem = new QTableWidgetItem(elidedAttrValue);
            m_tableattributes->setItem(row, 1, attrItem);
            row++;
        }

        m_tableattributes->setHorizontalHeaderLabels(headers);
        m_tableattributes->resizeRowsToContents();
        m_tableattributes->resizeColumnsToContents();
        m_tableattributes->adjustSize();

        // Adjust the height of the table to fit the content
        adjustHeight(m_tableattributes);

        m_tableattribute_title->show();
        m_tableattributes->show();
    }
    else
    {
        m_tableattribute_title->hide();
        m_tableattributes->hide();
    }
}

void XATableView::populateTable(const pugi::xml_node& node)
{
    m_tablechildren->clear();

    m_tablechildren->setRowCount(0);
    m_tablechildren->setColumnCount(1);

    QStringList headers;
    headers << "Tag/Text";

    auto children = node.children();
    auto num_children = countElements(node);

    if (num_children > 0)
    {
        m_tablechildren_title->setText(QString("%1 Subtags:").arg(num_children));
        m_tablechildren_title->show();
        m_tablechildren->show();

        QFontMetrics metrics(m_tablechildren->font());

        // Traverse the XML nodes to extract tag names, attributes, and child elements
        int row = 0;
        for (const pugi::xml_node& child : children)
        {
            int col = 0;
            std::string name = child.name();
            auto node_type = child.type();

            // XML elements to ignore
            if (node_type == pugi::node_comment
                || node_type == pugi::node_pi) continue;

            switch (node_type)
            {
            case pugi::node_element:
            {
                m_tablechildren->insertRow(row);
                {
                    // Add tag name
                    QString tagName = QString::fromStdString(name);
                    QTableWidgetItem* tagItem = new QTableWidgetItem(tagName);
                    setItemWrapper(m_tablechildren, row, col, tagItem);
                    col++;
                }

                // Add attributes as separate rows
                for (const pugi::xml_attribute& attr : child.attributes())
                {
                    addAttributeRow(m_tablechildren, headers, attr, row);
                }

                addChildElements(m_tablechildren, headers, child, row);
                row++;
            } break;
            case pugi::node_pcdata:
                m_tablechildren->insertRow(row);
                addTextRow(m_tablechildren, headers, child, row);
                row++;
                break;
            case pugi::node_cdata:
                m_tablechildren->insertRow(row);
                addTextRow(m_tablechildren, headers, child, row);
                row++;
                break;
            default:
                break;
            }


        }
    }
    else
    {
        m_tablechildren_title->hide();
        m_tablechildren->hide();
    }

    m_tablechildren->setHorizontalHeaderLabels(headers);
    //m_tablechildren->resizeRowsToContents();
    m_tablechildren->resizeColumnsToContents();
    m_tablechildren->adjustSize();

    // Adjust the height of the table to fit the content
    adjustHeight(m_tablechildren);

    // Adjust the width of the container to fit the content
    adjustWidth();
}

void XATableView::addAttributeRow(QTableWidget* table, QStringList& headers, const pugi::xml_attribute& attr, int row)
{
    QString attrName = attr.name();
    QString attrValue = attr.value();

    if (!headers.contains(attrName))
    {
        headers << attrName;
        table->setColumnCount(headers.size());
    }

    int attrCol = headers.indexOf(attrName);

    QTableWidgetItem* attrItem = new QTableWidgetItem(attrValue);
    setItemWrapper(table, row, attrCol, attrItem);
}

void XATableView::addTextRow(QTableWidget* table, QStringList& headers, const pugi::xml_node& node, int row)
{
    QString childName = "Text";
    QString childValue = node.text().as_string();
    childValue.remove('\n');
    childValue.remove('\r');
    QFontMetrics metrics(table->font());

    if (row == 0)
    {
        headers.clear();
        headers << childName;
        table->setColumnCount(headers.size());
    }
    int grandChildCol = headers.indexOf(childName);
    auto col_size = 2 * table->columnWidth(grandChildCol);
    QString elidedChildValue = metrics.elidedText(childValue, Qt::ElideRight, col_size);
    QTableWidgetItem* grandChildItem = new QTableWidgetItem(elidedChildValue);
    setItemWrapper(table, row, grandChildCol, grandChildItem);
}

/**
 * These are the children of "node" therefore the grandchildren of the selected node
 */
void XATableView::addChildElements(QTableWidget* table, QStringList& headers, const pugi::xml_node& node, int row)
{
    QFontMetrics metrics(table->font());
    QString unique_subtags_title("Unique Subtags");

    auto children = node.children();

    enum {
        _ONE_CDATA,
        _ONE_PCDATA,
        _ONE_ELEM,
        _MULTI_EQUAL_ELEM,
        _MULTI_ELEM,
    } layout_state;

    std::map<std::string, int> element_count;

    // Analyze the children
    struct ElemCount {
        int cdata_count = 0;
        int pcdata_count = 0;
        int elem_count = 0;
    } elem_count;

    for (const pugi::xml_node& child : children)
    {
        switch (child.type())
        {
        case pugi::node_cdata:
            elem_count.cdata_count++;
            break;
        case pugi::node_pcdata:
            elem_count.pcdata_count++;
            break;
        case pugi::node_element:
            elem_count.elem_count++;
            element_count[child.name()]++;
            break;
        }
    }

    if (elem_count.cdata_count > 0 && elem_count.pcdata_count == 0 && elem_count.elem_count == 0)
    {
        layout_state = _ONE_CDATA;
    }
    else if (elem_count.cdata_count == 0 && elem_count.pcdata_count > 0 && elem_count.elem_count == 0)
    {
        layout_state = _ONE_PCDATA;
    }
    else if (elem_count.cdata_count == 0 && elem_count.pcdata_count == 0 && elem_count.elem_count == 1)
    {
        layout_state = _ONE_ELEM;
    }
    else if (elem_count.elem_count > 1 && element_count.size() == 1)
    {
        layout_state = _MULTI_EQUAL_ELEM;
    }
    else if (elem_count.elem_count > 1 && element_count.size() > 1)
    {
        layout_state = _MULTI_ELEM;
    }
    else
    {
        return;
    }

    // Layout rules
    // ------------
    // 1. If there is only 1 element with text, its content is shown in a separate row
    // 2. If there is only 1 element with one "child elemnt", the child element's name is shown in a separate row
    // 3. If there are multiple identical elements, its name and the number of occurence is shown:
    //      "Name (x occurrences)"
    // 4. If there are multiple non-identical elements, their unique number of occurence is shown:
    //      "x unique subtags"


    int grandChildCol = 0;
    QString elidedChildValue;

    switch (layout_state)
    {
    case _ONE_CDATA:
    {
        QString childName = "Text";
        QString childValue = QString("%1").arg(node.first_child().text().as_string()).simplified();
        grandChildCol = addTableHeader(table, headers, childName, row);
        auto col_size = 2 * table->columnWidth(grandChildCol);
        elidedChildValue = QString("<![CDATA[%1]]").arg(metrics.elidedText(childValue, Qt::ElideRight, col_size));
    } break;

    case _ONE_PCDATA:
    {
        QString childName = "Text";
        QString childValue = QString("%1").arg(node.first_child().text().as_string()).simplified();
        grandChildCol = addTableHeader(table, headers, childName, row);
        auto col_size = 2 * table->columnWidth(grandChildCol);
        elidedChildValue = metrics.elidedText(childValue, Qt::ElideRight, col_size);
    } break;

    case _ONE_ELEM:
    {
        QString childName = unique_subtags_title;
        QString childValue = QString("%1").arg(node.first_child().name());
        grandChildCol = addTableHeader(table, headers, childName, row);
        auto col_size = 2 * table->columnWidth(grandChildCol);
        elidedChildValue = metrics.elidedText(childValue, Qt::ElideRight, col_size);

    } break;

    case _MULTI_EQUAL_ELEM:
    {
        QString childName = unique_subtags_title;
        QString childValue = QString("%1 (%2 occurrences)")
            .arg(node.first_child().name())
            .arg(element_count[node.first_child().name()]);
        if (!headers.contains(childName))
        {
            headers << childName;
            table->setColumnCount(headers.size());
        }
        grandChildCol = headers.indexOf(childName);
        auto col_size = 2 * table->columnWidth(grandChildCol);
        elidedChildValue = metrics.elidedText(childValue, Qt::ElideRight, col_size);
    } break;

    case _MULTI_ELEM:
    {
        QString childName = unique_subtags_title;
        QString childValue = QString("%1 unique subtags").arg(element_count.size());
        if (!headers.contains(childName))
        {
            headers << childName;
            table->setColumnCount(headers.size());
        }
        grandChildCol = headers.indexOf(childName);
        auto col_size = 2 * table->columnWidth(grandChildCol);
        elidedChildValue = metrics.elidedText(childValue, Qt::ElideRight, col_size);
    } break;

    default:
        return;
    }

    QTableWidgetItem* grandChildItem = new QTableWidgetItem(elidedChildValue);
    setItemWrapper(table, row, grandChildCol, grandChildItem);

}

int XATableView::addTableHeader(QTableWidget* table, QStringList& headers, const QString& childName, int row)
{
    if (row == 0)
    {
        headers << childName;
        table->setColumnCount(headers.size());
    }
    return headers.indexOf(childName);
}

void XATableView::adjustHeight(QTableWidget* table)
{
    // Adjust the height of the table to fit the content
    int totalHeight = table->horizontalHeader()->height();
    for (int i = 0; i < table->rowCount(); ++i)
    {
        totalHeight += table->rowHeight(i);
    }
    table->setFixedHeight(totalHeight);
}

void XATableView::adjustWidth()
{
    // Adjust the width of the container to fit the content
    int maxWidth = std::max(m_tableattributes->horizontalHeader()->length(),
        m_tablechildren->horizontalHeader()->length());
    
    m_tablechildren->setFixedWidth(maxWidth + m_scrollArea->verticalScrollBar()->width() + 20);
}

void XATableView::setItemWrapper(QTableWidget* table, int row, int column, QTableWidgetItem* item)
{
    qDebug() << "row: " << row << "  col: " << column << " : " << item->text();
    table->setItem(row, column, item);
}

size_t XATableView::countElements(const pugi::xml_node& node)
{
    size_t num = 0;

    for (const auto& child : node.children())
    {
        switch (child.type())
        {
        case pugi::node_element:    ++num; break;
        default: break;

        }
    }

    return num;
}
