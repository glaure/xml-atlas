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

#include <pugixml.hpp>
#include <sstream>

class XAXMLWriter
{
public:
    XAXMLWriter();

    void setIndentation(int indent_size);
    void setAttributesPerLine(int max_attr_per_line);
    void setUseSpaces(bool use_spaces);

    bool write(const pugi::xml_document& doc, std::stringstream& buff);

private:
    int m_indent_size;
    int m_max_attr_per_line;
    bool m_use_spaces;
};
