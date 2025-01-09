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
#include <string>
#include <algorithm>
#include <cctype>
#include <locale>

namespace
{
    std::string trim(const std::string& str)
    {
        auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) {
            return std::isspace(ch);
            });

        auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {
            return std::isspace(ch);
            }).base();

        if (start < end)
        {
            return std::string(start, end);
        }
        else
        {
            return std::string();
        }
    }

    /**
     * Adaptable xml pretty printer
     * XMLMarker style
     */
    struct xml_marker_writer : pugi::xml_writer
    {
        std::stringstream& result;
        int indent_size;
        int max_attr_per_line;
        bool use_spaces;
        int current_indent;

        xml_marker_writer(std::stringstream& buff, int indent_size, int max_attr_per_line, bool use_spaces)
            : result(buff)
            , indent_size(indent_size)
            , max_attr_per_line(max_attr_per_line)
            , use_spaces(use_spaces)
            , current_indent(0) 
        {
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
            auto attributes = node.attributes();
            if (std::distance(attributes.begin(), attributes.end()) > max_attr_per_line)
            {
                ++current_indent;
                for (const auto& attr : attributes)
                {
                    result << '\n';
                    write_indent();
                    result << ' ' << attr.name() << " = \"" << attr.value() << "\"";
                }
                --current_indent;
            }
            else
            {
                for (auto attr = node.first_attribute(); attr; attr = attr.next_attribute())
                {
                    result << ' ' << attr.name() << " = \"" << attr.value() << "\"";
                    if (++attr_count >= max_attr_per_line)
                    {
                        result << '\n';
                        write_indent();
                        attr_count = 0;
                    }
                }
            }
        }

        bool write_pcdata(const pugi::xml_node& node)
        {
            std::string value = trim(node.value());
            // is it a single line?
            if (value.find('\n') == std::string::npos)
            {
                result << value;
            }
            else
            {
                // multiline
                std::stringstream ss(value);
                std::string line;
                result << '\n';
                ++current_indent;
                while (std::getline(ss, line))
                {
                    write_indent();
                    result << trim(line) << '\n';
                }
                --current_indent;
                return true;
            }
            return false;
        }

        void write_cdata(const pugi::xml_node& node)
        {
            write_indent();
            result << "<![CDATA[" << node.value() << "]]>" << '\n';
        }

        void write_element(const pugi::xml_node& node)
        {
            write_indent();
            result << '<' << node.name();
            write_attributes(node);

            // has children?
            if (node.first_child())
            {
                auto num_childs = std::distance(node.children().begin(), node.children().end());
                if ((num_childs == 1) && (node.first_child().type() == pugi::node_pcdata))
                {
                    result << '>';
                    if (write_pcdata(node.first_child()))
                    {
                        write_indent();
                    }
                    result << "</" << node.name() << '>' << '\n';
                }
                else
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
            }
            else
            {
                // empty element
                result << "/>" << '\n';
            }
        }

        void write_processing_instruction(const pugi::xml_node& node)
        {
            result << "<?" << node.name() << " " << node.value() << "?>" << '\n';
        }

        void write_comment(const pugi::xml_node& node)
        {
            write_indent();
            result << "<!--" << node.value() << "-->" << '\n';
        }

        void write_doctype(const pugi::xml_node& node)
        {
            result << "<!DOCTYPE " << node.name() << node.value() << ">" << '\n';
        }

        void write_node(const pugi::xml_node& node)
        {
            auto node_type = node.type();

            switch (node_type)
            {
            // Empty (null) node handle
            case pugi::node_null:
                break;

            // A document tree's absolute root
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
            } break;

            // Element tag, i.e. '<node/>'
            case pugi::node_element:
                write_element(node);
                break;

            // Plain character data, i.e. 'text'
            case pugi::node_pcdata:
                write_pcdata(node);
                break;

            // Character data, i.e. '<![CDATA[text]]>'
            case pugi::node_cdata:
                write_cdata(node);
                break;

            // Comment tag, i.e. '<!-- text -->'
            case pugi::node_comment:
                write_comment(node);
                break;

            // Processing instruction, i.e. '<?name?>'
            case pugi::node_pi:
                write_processing_instruction(node);
                break;

            // Document declaration, i.e. '<?xml version="1.0"?>'
            case pugi::node_declaration:
                result << "<?xml";
                write_attributes(node);
                result << "?>\n";
                break;

            // Document type declaration, i.e. '<!DOCTYPE doc>'
            case pugi::node_doctype:
                write_doctype(node);
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
    xml_marker_writer writer(buff, m_indent_size, m_max_attr_per_line, m_use_spaces);
    writer.write_node(doc);
    return true;
#endif

}
