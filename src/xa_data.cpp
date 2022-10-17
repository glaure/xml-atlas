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
#include "pugixml.hpp"
#include <vector>

namespace
{
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
            m_tree_root = m_model->getRoot();
            m_current_parent = m_tree_root;
            m_last_node = m_current_parent;
        }

        // Callback that is called when traversal begins
        virtual bool begin(pugi::xml_node& node) override
        {
            return true;
        }

        // Callback that is called for each node traversed
        virtual bool for_each(pugi::xml_node& node) override
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
                    auto new_node = new XAXMLTreeItem(node.name(), m_current_parent);
                    m_current_parent->appendChild(new_node);
                    m_last_node = new_node;
                }
                else if (m_last_depth == current_depth) {
                    // node is a sibling to the previous node
                    auto new_node = new XAXMLTreeItem(node.name(), m_current_parent);
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
                    auto new_node = new XAXMLTreeItem(node.name(), m_current_parent);
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
        virtual bool end(pugi::xml_node& node) override
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


XAData::XAData()
{
    m_xml_tree_model = new XAXMLTreeModel;
}

XAData::~XAData()
{
    delete m_xml_tree_model;
}


XAXMLTreeModel* XAData::getXMLTreeModel() const
{
    return m_xml_tree_model;
}

void XAData::setContent(const QString& content)
{
    m_content = content;

    buildTreeModelFromContent();
}

void XAData::buildTreeModelFromContent()
{
    pugi::xml_document doc;
    auto parse_result = doc.load_buffer(m_content.toLatin1(), m_content.size());
    if (parse_result.status == pugi::status_ok)
    {
        XmlTreeBuilder tb(m_xml_tree_model);

        m_xml_tree_model->beginFillModel();
        doc.traverse(tb);
        m_xml_tree_model->endFillModel();
        
        //m_xml_tree_model->updateAll();
    }
}
