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
#include <QWidget>
#include <pugixml.hpp>

class QVBoxLayout;
class QLabel;
class QTableWidget;
class QScrollArea;

class XATableView : public QWidget
{
    Q_OBJECT

public:
    XATableView(QWidget* parent = nullptr);

    void setTableRootNode(pugi::xml_node node);

private:
    void setupLayout();
    void populateAttributeTable(pugi::xml_node node);
    void populateTable(pugi::xml_node node);
    void adjustHeight(QTableWidget* table);
    void adjustWidth();

    QVBoxLayout*  m_layout;
    QLabel*       m_tableattribute_title;
    QTableWidget* m_tableattributes;
    QLabel*       m_tablechildren_title;
    QTableWidget* m_tablechildren;
    QScrollArea*  m_scrollArea;
};
