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

#include "xa_xml_writer.h"

namespace
{
    struct xml_writer_custom : pugi::xml_writer
    {
        std::stringstream& result;
        int indent_size;
        int max_attr_per_line;
        bool use_spaces;
        int current_indent;

        xml_writer_custom(std::stringstream& buff, int indent_size, int max_attr_per_line, bool use_spaces)
            : result(buff), indent_size(indent_size), max_attr_per_line(max_attr_per_line), use_spaces(use_spaces), current_indent(0) {
        }

        void write(const void* data, size_t size) override
        {
            result.write(static_cast<const char*>(data), size);
        }

        void write_indent()
        {
            if (use_spaces)
            {
                result << std::string(current_indent * indent_size, ' ');
            }
            else
            {
                result << std::string(current_indent, '\t');
            }
        }

        void write_attributes(const pugi::xml_node& node)
        {
            int attr_count = 0;
            for (auto attr = node.first_attribute(); attr; attr = attr.next_attribute())
            {
                if (attr_count > 0)
                {
                    result << ' ';
                }
                result << attr.name() << "=\"" << attr.value() << "\"";
                if (++attr_count >= max_attr_per_line)
                {
                    result << '\n';
                    write_indent();
                    attr_count = 0;
                }
            }
        }

        void write_node(const pugi::xml_node& node)
        {
            auto node_type = node.type();

            switch (node_type)
            {
            case pugi::node_document:
            {
                // has declaration node? -> check first element
                if (node.first_child().type() == pugi::node_element)
                {
                    result << PUGIXML_TEXT("<?xml version=\"1.0\"");
                    // encoding ?
                    result << "?>\n";
                }
                for (auto child = node.first_child(); child; child = child.next_sibling())
                {
                    write_node(child);
                }
            }
                break;

            case pugi::node_element:
                write_indent();
                result << '<' << node.name();
                write_attributes(node);

                if (node.first_child())
                {
                    result << '>' << '\n';
                    ++current_indent;
                    for (auto child = node.first_child(); child; child = child.next_sibling())
                    {
                        write_node(child);
                    }
                    --current_indent;
                    write_indent();
                    result << "</" << node.name() << '>' << '\n';
                }
                else
                {
                    result << "/>" << '\n';
                }
                break;

            case pugi::node_pcdata:
                write_indent();
                result << node.value() << '\n';
                break;

            case pugi::node_declaration:
                result << "<?xml";
                write_attributes(node);
                result << "?>\n";
                break;

            default:
                break;
            }
        }

    };

}


XAXMLWriter::XAXMLWriter()
    : m_indent_size(4)
    , m_max_attr_per_line(20)
    , m_use_spaces(true)
{
}

void XAXMLWriter::setIndentation(int indent_size)
{
    m_indent_size = indent_size;
}

void XAXMLWriter::setAttributesPerLine(int max_attr_per_line)
{
    m_max_attr_per_line = max_attr_per_line;
}

void XAXMLWriter::setUseSpaces(bool use_spaces)
{
    m_use_spaces = use_spaces;
}

bool XAXMLWriter::write(const pugi::xml_document& doc, std::stringstream& buff)
{
#if 0
    doc.save(buff);
    return true;
#else
    xml_writer_custom writer(buff, m_indent_size, m_max_attr_per_line, m_use_spaces);
    writer.write_node(doc);
    return true;
#endif

}
