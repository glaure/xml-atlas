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

#include "xa_data.h"
#include "xa_xml_tree_model.h"
#include "xa_xml_tree_item.h"
#include "xa_xml_writer.h"
#include <sstream>
#include <vector>

namespace
{

    void appendAttributes(XAXMLTreeItem* item, const pugi::xml_node& node)
    {
        const auto& attributes = node.attributes();
        for (const auto& attr : attributes)
        {
            item->appendChild(new XAXMLTreeItem{ attr.name(), 
                node, 
                XAXMLTreeItemType::ATTRIBUTE, 
                item});
        }
    }

    class XmlTreeBuilder : public pugi::xml_tree_walker
    {
    public:
        XmlTreeBuilder(XAXMLTreeModel* model)
            : m_model(model)
            , m_tree_root(nullptr)
            , m_current_parent(nullptr)
            , m_last_node(nullptr)
            , m_last_depth(-1)
        {
            m_tree_root = m_model->rootItem();
            m_current_parent = m_tree_root;
            m_last_node = m_current_parent;
        }

        // Callback that is called when traversal begins
        bool begin(pugi::xml_node& node) override
        {
            return true;
        }

        // Callback that is called for each node traversed
        bool for_each(pugi::xml_node& node) override
        {
            switch (node.type())
            {
            case pugi::node_element: {
                auto current_depth = depth();
                auto name = node.name();
                if (m_last_depth < current_depth) {
                    // node is the first child of a parent node
                    // m_last_node is the parent for this node
                    m_current_parent = m_last_node;
                    auto new_node = new XAXMLTreeItem(node.name(), 
                        node, 
                        XAXMLTreeItemType::ELEMENT, 
                        m_current_parent);
                    appendAttributes(new_node, node);
                    m_current_parent->appendChild(new_node);
                    m_last_node = new_node;
                }
                else if (m_last_depth == current_depth) {
                    // node is a sibling to the previous node
                    auto new_node = new XAXMLTreeItem(node.name(), 
                        node, 
                        XAXMLTreeItemType::ELEMENT, 
                        m_current_parent);
                    appendAttributes(new_node, node);
                    m_current_parent->appendChild(new_node);
                    m_last_node = new_node;
                }
                else {
                    // node is child on another branch
                    auto last_depth = m_last_depth;
                    while (current_depth < last_depth) {
                        m_current_parent = m_current_parent->parentItem();
                        --last_depth;
                    }
                    auto new_node = new XAXMLTreeItem(node.name(), 
                        node, 
                        XAXMLTreeItemType::ELEMENT, 
                        m_current_parent);
                    appendAttributes(new_node, node);
                    m_current_parent->appendChild(new_node);
                    m_last_node = new_node;

                }
                m_last_depth = current_depth;
                break;
            }
            }

            

            return true;
        }

        // Callback that is called when traversal ends
        bool end(pugi::xml_node& node) override
        {
            return true;
        }
    private:
        XAXMLTreeModel* m_model;
        XAXMLTreeItem* m_tree_root;
        XAXMLTreeItem* m_current_parent;
        XAXMLTreeItem* m_last_node;
        int m_last_depth;
    };
}


XAData::XAData(XATheme* theme)
{
    m_xml_tree_model = new XAXMLTreeModel(theme, this);
}

XAData::~XAData()
{
}


XAXMLTreeModel* XAData::getXMLTreeModel() const
{
    return m_xml_tree_model;
}

void XAData::setContent(const QString& content)
{
    m_content = content;
    m_xml_tree_model->clear();
    buildTreeModelFromContent();
}

QString XAData::indentDocument(int indent_size, int max_attr_per_line, bool use_spaces)
{
    std::stringstream sbuff;

    XAXMLWriter xw;
    xw.setIndentation(indent_size);
    xw.setAttributesPerLine(max_attr_per_line);
    xw.setUseSpaces(use_spaces);

    xw.write(m_doc, sbuff);

    return QString::fromStdString(sbuff.str());
}

pugi::xml_document& XAData::getDocument()
{
    return m_doc;
}

void XAData::buildTreeModelFromContent()
{
    auto parse_result = m_doc.load_buffer(m_content.toLatin1(), m_content.size(), pugi::parse_full);
    if (parse_result.status == pugi::status_ok)
    {
        XmlTreeBuilder tb(m_xml_tree_model);

        m_xml_tree_model->beginFillModel();
        m_doc.traverse(tb);
        m_xml_tree_model->endFillModel();
        
        //m_xml_tree_model->updateAll();
    }
}
