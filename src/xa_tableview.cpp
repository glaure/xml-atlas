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
#include <algorithm>


XATableView::XATableView(QWidget* parent)
    : QWidget(parent)
    , m_table_root()
    , m_num_unique_col(0)
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

void XATableView::setTableRootNode(pugi::xml_node node, int num_unique_col)
{
    m_table_root = node;
    m_num_unique_col = num_unique_col;

    m_table_title->setText(QString("%1").arg(node.name()));
    populateAttributeTable(node);
    populateElementTable(node);
}

void XATableView::setUniqueConsolidation(int num_unique_col)
{
    m_num_unique_col = num_unique_col;

    m_table_title->setText(QString("%1").arg(m_table_root.name()));
    populateAttributeTable(m_table_root);
    populateElementTable(m_table_root);
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

void XATableView::populateElementTable(const pugi::xml_node& node)
{
    // preprocess stage: Count unique columns
    auto [count_tags, count_attrs] = countUniqueItems(node);
    // count all elements with an occurrence of 1
    auto unique_elem = std::count_if(count_tags.cbegin(), count_tags.cend(), [](const auto& p) { return p.second == 1; });
    auto unique_attr = std::count_if(count_attrs.cbegin(), count_attrs.cend(), [](const auto& p) { return p.second == 1; });
    bool unique_cons = m_num_unique_col <= unique_elem || m_num_unique_col <= unique_attr;
    const QString header_unique_attr = "Unique Attributes";
    const QString header_unique_subtags = "Unique Subtags";

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

            switch (node_type)
            {
            case pugi::node_element:
            {
                m_tablechildren->insertRow(row);
                
                {
                    // Add tag name row
                    QString tagName = QString::fromStdString(name);
                    //if (!headers.contains(tagName))
                    //{
                    //    headers << tagName;
                    //    m_tablechildren->setColumnCount(headers.size());
                    //}
                    QTableWidgetItem* tagItem = new QTableWidgetItem(tagName);
                    setItemWrapper(m_tablechildren, row, col, tagItem);
                    col++;
                }

                // Add attributes 
                {
                    int count_unique_attr = 0;
                    for (const pugi::xml_attribute& attr : child.attributes())
                    {
                        std::string attr_name = attr.name();
                        if (unique_cons && count_attrs[attr_name] == 1)
                        {
                            ++count_unique_attr;
                        }
                        else
                        {
                            QString qattr_name = attr.name();
                            if (!headers.contains(qattr_name))
                            {
                                headers << qattr_name;
                                m_tablechildren->setColumnCount(headers.size());
                            }
                            int attrCol = headers.indexOf(qattr_name);
                            QString attrValue = attr.value();
                            QTableWidgetItem* attrItem = new QTableWidgetItem(attrValue);
                            setItemWrapper(m_tablechildren, row, attrCol, attrItem);
                        }
                    }
                    if (count_unique_attr > 0)
                    {
                        if (!headers.contains(header_unique_attr))
                        {
                            headers << header_unique_attr;
                            m_tablechildren->setColumnCount(headers.size());
                        }
                        int attrCol = headers.indexOf(header_unique_attr);
                        if (count_unique_attr > 1)
                        {
                            QString attrValue = QString("%1 unique attributes")
                                .arg(count_unique_attr);
                            QTableWidgetItem* attrItem = new QTableWidgetItem(attrValue);
                            setItemWrapper(m_tablechildren, row, attrCol, attrItem);
                        }
                        else
                        {
                            QString attrValue = QString("%1 = \"%2\"")
                                .arg(child.first_attribute().name()).arg(child.first_attribute().as_string());
                            QTableWidgetItem* attrItem = new QTableWidgetItem(attrValue);
                            setItemWrapper(m_tablechildren, row, attrCol, attrItem);
                        }
                    }
                }

                // iterate grand children
                int count_unique_tags = 0;
                for (const pugi::xml_node& grand_child : child.children())
                {
                    std::string grand_name = grand_child.name();
                    auto grand_node_type = grand_child.type();

                    switch (grand_node_type)
                    {
                    case pugi::node_element:
                    {
                        // Extend header row
                        QString tagName = QString::fromStdString(grand_name);
                        if (unique_cons && count_tags[grand_name] == 1)
                        {
                            ++count_unique_tags;
                        }
                        else
                        {
                            if (!headers.contains(tagName))
                            {
                                headers << tagName;
                                m_tablechildren->setColumnCount(headers.size());
                            }
                            int tagCol = headers.indexOf(tagName);
                            auto item = new QTableWidgetItem(getCellContent(grand_child, count_tags[grand_name]));
                            setItemWrapper(m_tablechildren, row, tagCol, item);
                        }

                    } break;

                    default:
                        break;
                    }
                }

                if (count_unique_tags > 0)
                {
                    if (!headers.contains(header_unique_subtags))
                    {
                        headers << header_unique_subtags;
                        m_tablechildren->setColumnCount(headers.size());
                    }
                    int col = headers.indexOf(header_unique_subtags);
                    if (count_unique_tags > 1)
                    {
                        QString tagValue = QString("%1 unique subtags")
                            .arg(count_unique_tags);
                        QTableWidgetItem* item = new QTableWidgetItem(tagValue);
                        setItemWrapper(m_tablechildren, row, col, item);
                    }
                    else
                    {
                        QString tagValue = QString("%1")
                            .arg(QString::fromLatin1(child.first_child().name()));
                        QTableWidgetItem* item = new QTableWidgetItem(tagValue);
                        setItemWrapper(m_tablechildren, row, col, item);
                    }

                }

                //addChildElements(m_tablechildren, headers, child, row);
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
            case pugi::node_comment:
                break;
            case pugi::node_pi:
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

std::tuple<ItemOccurenceMap, ItemOccurenceMap> XATableView::countUniqueItems(const pugi::xml_node& node)
{
    ItemOccurenceMap count_tags;
    ItemOccurenceMap count_attrs;

    // iterate direct children
    for (const pugi::xml_node& child : node.children())
    {
        std::string name = child.name();
        auto node_type = child.type();

        switch (node_type)
        {
        case pugi::node_element:
        {
            // count tag occurrence
            count_tags[name]++;

            // count attribute occurrence
            for (const pugi::xml_attribute& attr : child.attributes())
            {
                count_attrs[attr.name()]++;
            }

            // iterate grand children
            for (const pugi::xml_node& grand_child : child.children())
            {
                std::string gc_name = grand_child.name();
                auto gc_node_type = grand_child.type();

                switch (gc_node_type)
                {
                case pugi::node_element:
                {
                    // count gran children tag occurrence
                    count_tags[gc_name]++;

                    // count attribute occurrence
                    for (const pugi::xml_attribute& attr : grand_child.attributes())
                    {
                        count_attrs[attr.name()]++;
                    }

                } break;

                default:
                    break;
                }
            }
        } break;

        case pugi::node_pcdata:
            break;
        case pugi::node_cdata:
            break;
        case pugi::node_comment:
            break;
        case pugi::node_pi:
            break;
        default:
            break;
        }
    }


    return { count_tags, count_attrs };
}

QString XATableView::getCellContent(const pugi::xml_node& node, int occurrence)
{
    auto count_children = countChildren(node);

    switch (node.type())
    {
    case pugi::node_cdata:
        return node.value();
    case pugi::node_pcdata:
        return node.value();
    case pugi::node_element:
    {
        if (count_children == 1)
        {
            auto child = node.first_child();
            switch (child.type())
            {
            case pugi::node_cdata:
                return child.text().as_string();
            case pugi::node_pcdata:
                return child.text().as_string();
            case pugi::node_element:
                return child.name();
            }
        }
        else if (count_children == 0)
        {
            if (occurrence > 1)
            {
                return QString("%1 (%2 occurrences)").arg(node.name()).arg(occurrence);
            }
            return node.text().as_string();
        }
        else
        {
            return node.name();
        }
        
    } break;
        
    default:
        break;
    }
    return {};
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

size_t XATableView::countElements(const pugi::xml_node& node) const
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

size_t XATableView::countChildren(const pugi::xml_node& node) const
{
    size_t num = 0;

    for (const auto& child : node.children())
    {
        switch (child.type())
        {
        case pugi::node_element:    
        case pugi::node_pcdata:
        case pugi::node_cdata:
            ++num; 
            break;
        default: break;

        }
    }

    return num;
}