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

#include "pugixml.hpp"
#include <QObject>


class XAXMLTreeModel;
class XATheme;

 /**
  * Application main model
  */
class XAData : public QObject
{
    Q_OBJECT

public:
    XAData(XATheme* theme);
    ~XAData();

    XAXMLTreeModel* getXMLTreeModel() const;

    void setContent(const QString& content);

    QString indentDocument(int indent_size, int max_attr_per_line, bool use_spaces);

private:
    void buildTreeModelFromContent();

private:
    XAXMLTreeModel*     m_xml_tree_model;
    QString             m_content;
    pugi::xml_document  m_doc;
};
