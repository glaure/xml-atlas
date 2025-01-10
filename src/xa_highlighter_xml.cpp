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

#include "xa_highlighter_xml.h"
#include "xa_app.h"
#include "xa_theme.h"


XAHighlighter_XML::XAHighlighter_XML(XAApp* app, QTextDocument* parent)
    : QSyntaxHighlighter(parent)
    , m_app(app)
{
    init();
}

void XAHighlighter_XML::highlightBlock(const QString& text)
{
    auto line = text.toStdString();

    // apply the line highlighting rules
    for (const HighlightingRule &rule : std::as_const(m_line_rules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    // apply multi-line rules

    int startIndex = 0;
    if (previousBlockState() != 1) {
        startIndex = text.indexOf("<!--");
    }

    while (startIndex >= 0) {
        int endIndex = text.indexOf("-->", startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else {
            commentLength = endIndex - startIndex + 3;
        }
        setFormat(startIndex, commentLength, m_format_map.at(static_cast<int>(XMLSE::XML_MULTILINE_COMMENT)));
        startIndex = text.indexOf("<!--", startIndex + commentLength);
    }

    // Handle multi-line CDATA sections
    startIndex = 0;
    if (previousBlockState() != 2) {
        startIndex = text.indexOf("<![CDATA[");
    }

    while (startIndex >= 0) {
        int endIndex = text.indexOf("]]>", startIndex);
        int cdataLength;
        if (endIndex == -1) {
            setCurrentBlockState(2);
            cdataLength = text.length() - startIndex;
        }
        else {
            cdataLength = endIndex - startIndex + 3;
        }
        setFormat(startIndex, cdataLength, m_format_map.at(static_cast<int>(XMLSE::XML_CDATA)));
        startIndex = text.indexOf("<![CDATA[", startIndex + cdataLength);
    }

    // Handle XML elements
    {
        // <ABC
        auto elem = m_element_start_pattern.match(text);
        while (elem.hasMatch()) {
            int elementStart = elem.capturedStart();
            int elementEnd = elem.capturedEnd();
            auto debug = text.mid(elementStart, elementEnd - elementStart).toStdString();
            setFormat(elementStart, elementEnd - elementStart, m_format_map.at(static_cast<int>(XMLSE::XML_ELEM)));
            elem = m_element_start_pattern.match(text, elementEnd);

            // <ABC ...  we are ready to handle attributes
            setCurrentBlockState(3);

            // attributes
            {
                auto attr_name = m_attribute_name.match(text, elementEnd);
                while (attr_name.hasMatch())
                {
                    int attrStart = attr_name.capturedStart();
                    int attrEnd = attr_name.capturedEnd();
                    setFormat(attrStart, attrEnd - attrStart, m_format_map.at(static_cast<int>(XMLSE::XML_ATTR)));
                    attr_name = m_attribute_name.match(text, attrEnd);
                    // attribute value
                    auto attr_value = m_attribute_value.match(text, attrEnd);
                    if (attr_value.hasMatch())
                    {
                        int attrValueStart = attr_value.capturedStart();
                        int attrValueEnd = attr_value.capturedEnd();
                        setFormat(attrValueStart, attrValueEnd - attrValueStart, m_format_map.at(static_cast<int>(XMLSE::XML_ATTR_VALUE)));
                    }
                }
            }

            {   // >
                auto cm = m_element_closing_pattern.match(text, elementEnd);
                if (cm.hasMatch()) {
                    int elementStart = cm.capturedStart();
                    int elementEnd = cm.capturedEnd();
                    auto debug2 = text.mid(elementStart, elementEnd - elementStart).toStdString();
                    setFormat(elementStart, elementEnd - elementStart, m_format_map.at(static_cast<int>(XMLSE::XML_ELEM)));
                }
            }
        }

        // </ABC>
        auto elementEndMatch = m_element_end_pattern.match(text);
        while (elementEndMatch.hasMatch()) {
            int elementStart = elementEndMatch.capturedStart();
            int elementEnd = elementEndMatch.capturedEnd();
            setFormat(elementStart, elementEnd - elementStart, m_format_map.at(static_cast<int>(XMLSE::XML_ELEM)));
            elementEndMatch = m_element_end_pattern.match(text, elementEnd);
        }
    }
    
    // Handle XML attributes
    {
        // <ABC ...  we are ready to handle attributes
        if (previousBlockState() == 3)
        {
            // attributes
            {
                auto attr_name = m_attribute_name.match(text);
                while (attr_name.hasMatch())
                {
                    int attrStart = attr_name.capturedStart();
                    int attrEnd = attr_name.capturedEnd();
                    setFormat(attrStart, attrEnd - attrStart, m_format_map.at(static_cast<int>(XMLSE::XML_ATTR)));
                    auto debug = text.mid(attrStart, attrEnd - attrStart).toStdString();

                    // attribute value
                    auto attr_value = m_attribute_value.match(text, attrEnd);
                    int attrValueEnd = 0;
                    if (attr_value.hasMatch())
                    {
                        int attrValueStart = attr_value.capturedStart();
                        attrValueEnd = attr_value.capturedEnd();
                        setFormat(attrValueStart, attrValueEnd - attrValueStart, m_format_map.at(static_cast<int>(XMLSE::XML_ATTR_VALUE)));
                        debug = text.mid(attrValueStart, attrValueEnd - attrValueStart).toStdString();
                    }
                    else
                    {
                        break;
                    }
                    attr_name = m_attribute_name.match(text, attrValueEnd);

                    // >
                    auto cm = m_element_closing_pattern.match(text, attrValueEnd);
                    if (cm.hasMatch())
                    {
                        int elementStart = cm.capturedStart();
                        int elementEnd = cm.capturedEnd();
                        auto debug2 = text.mid(elementStart, elementEnd - elementStart).toStdString();
                        setFormat(elementStart, elementEnd - elementStart, m_format_map.at(static_cast<int>(XMLSE::XML_ELEM)));
                        setCurrentBlockState(0);
                        break;
                    }
                    else
                    {
                        // still in the same element
                        setCurrentBlockState(3);
                    }
                    

                }

            }
        }
    }

}

void XAHighlighter_XML::onThemeChange()
{
    init();
    rehighlight();
}


void XAHighlighter_XML::init()
{
    HighlightingRule rule;

    updateFormatMap();

    m_line_rules.clear();

    // line rules
    {   // Processing instruction
        rule.pattern = QRegularExpression(QStringLiteral("<\\?.*\\?>"));
        rule.format = m_format_map.at(static_cast<int>(XMLSE::XML_PI));
        m_line_rules.append(rule);

        //<?xml version="1.0" encoding="UTF-8"?>
        rule.pattern = QRegularExpression(QStringLiteral("<\\?xml.*>"));
        rule.format = m_format_map.at(static_cast<int>(XMLSE::XML_PROLOG));
        m_line_rules.append(rule);

        //<!-- comment -->
        rule.pattern = QRegularExpression(QStringLiteral("<!--[^>]*-->"));
        rule.format = m_format_map.at(static_cast<int>(XMLSE::XML_COMMENT));
        m_line_rules.append(rule);

        //</ABC>
        //<ABC/>
        //rule.pattern = QRegularExpression(QStringLiteral("<[/]?[\\s]*([a-zA-Z_][\\w\\-\\.]*)[\\s/>]"));
        //rule.format = m_format_map.at(static_cast<int>(XMLSE::XML_ELEM));
        //m_line_rules.append(rule);
    }

    // (potential) multi-line rules
    {   //<!-- multi-line comment -->
        rule.pattern = QRegularExpression(QStringLiteral("<!--"));
        rule.format = m_format_map.at(static_cast<int>(XMLSE::XML_MULTILINE_COMMENT));

        //<![CDATA[ ... ]]>
        rule.pattern = QRegularExpression(QStringLiteral("<!\\[CDATA\\[.*\\]\\]>"));
        rule.format = m_format_map.at(static_cast<int>(XMLSE::XML_CDATA));

        //<ABC></ABC>
        //<ABC/>
        //rule.pattern = QRegularExpression(QStringLiteral("<[/]?[\\s]*([a-zA-Z_][\\w\\-\\.]*)[\\s/>]"));
        //rule.format = m_format_map.at(static_cast<int>(XMLSE::XML_ELEM));

        //<ABC
        m_element_start_pattern = QRegularExpression(QStringLiteral("<([a-zA-Z_][\\w\\-\\.]*)"));
        m_element_closing_pattern = QRegularExpression(QStringLiteral(">(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)"));
        m_element_end_pattern = QRegularExpression(QStringLiteral("</([a-zA-Z_][\\w\\-\\.]*)>")); 

        // XML attribute name
        //rule.pattern = QRegularExpression(QStringLiteral("\\b[a-zA-Z_][\\w\\-\\.]*\\b(?=\\s*=)"));
        //rule.format = m_format_map.at(static_cast<int>(XMLSE::XML_ATTR));
        m_attribute_name = QRegularExpression(QStringLiteral("\\b[a-zA-Z_][\\w\\-\\.]*\\b(?=\\s*=)"));

        // XML Attribute value
        m_attribute_value = QRegularExpression(QStringLiteral("\"[^\"]*\""));
        //rule.pattern = QRegularExpression(QStringLiteral("\"[^\"]*\""));
        //rule.format = m_format_map.at(static_cast<int>(XMLSE::XML_ATTR_VALUE));
    }
}


void XAHighlighter_XML::updateFormatMap()
{
    auto theme = m_app->getTheme();

    if (theme->getColorTheme() == "dark")
    {
        {
            QTextCharFormat fmt;
            fmt.setFontWeight(QFont::Bold);
            fmt.setForeground(QBrush(0x569cd6));    // lightblue
            m_format_map[static_cast<int>(XMLSE::XML_PROLOG)] = fmt;
        }

        {
            QTextCharFormat fmt;
            fmt.setFontWeight(QFont::Bold);
            fmt.setForeground(QBrush(0x569cd6));    // lightblue
            m_format_map[static_cast<int>(XMLSE::XML_ELEM)] = fmt;
        }

        {
            QTextCharFormat fmt;
            fmt.setFontWeight(QFont::Bold);
            fmt.setForeground(QBrush(0x9cdcfe));    // lightlightblue
            m_format_map[static_cast<int>(XMLSE::XML_ATTR)] = fmt;
        }

        {
            QTextCharFormat fmt;
            fmt.setForeground(QBrush(0xce9178));    // orange/red
            m_format_map[static_cast<int>(XMLSE::XML_ATTR_VALUE)] = fmt;
        }

        {
            QTextCharFormat fmt;
            fmt.setFontItalic(true);
            fmt.setForeground(QBrush(0x6a9955));    // green
            m_format_map[static_cast<int>(XMLSE::XML_COMMENT)] = fmt;
        }

        {
            QTextCharFormat fmt;
            fmt.setFontItalic(true);
            fmt.setForeground(QBrush(0x6a9955));    // green
            m_format_map[static_cast<int>(XMLSE::XML_MULTILINE_COMMENT)] = fmt;
        }

        {
            QTextCharFormat fmt;
            fmt.setForeground(QBrush(0xffd700));    // gold
            m_format_map[static_cast<int>(XMLSE::XML_CDATA)] = fmt;
        }

        {
            QTextCharFormat fmt;
            fmt.setForeground(QBrush(0xffa500));    // orange
            m_format_map[static_cast<int>(XMLSE::XML_PI)] = fmt;
        }

    }
    else
    {
        {
            QTextCharFormat fmt;
            fmt.setFontWeight(QFont::Bold);
            fmt.setForeground(QColor(Qt::gray).darker(100));
            m_format_map[static_cast<int>(XMLSE::XML_PROLOG)] = fmt;
        }

        {
            QTextCharFormat fmt;
            fmt.setFontWeight(QFont::Bold);
            fmt.setForeground(Qt::darkBlue);
            m_format_map[static_cast<int>(XMLSE::XML_ELEM)] = fmt;
        }

        {
            QTextCharFormat fmt;
            fmt.setFontWeight(QFont::Bold);
            fmt.setForeground(Qt::gray);
            m_format_map[static_cast<int>(XMLSE::XML_ATTR)] = fmt;
        }

        {
            QTextCharFormat fmt;
            fmt.setForeground(Qt::darkMagenta);
            m_format_map[static_cast<int>(XMLSE::XML_ATTR_VALUE)] = fmt;
        }

        {
            QTextCharFormat fmt;
            fmt.setFontItalic(true);
            fmt.setForeground(Qt::darkGreen);
            m_format_map[static_cast<int>(XMLSE::XML_COMMENT)] = fmt;
        }

        {
            QTextCharFormat fmt;
            fmt.setFontItalic(true);
            fmt.setForeground(Qt::darkGreen);
            m_format_map[static_cast<int>(XMLSE::XML_MULTILINE_COMMENT)] = fmt;
        }

        {
            QTextCharFormat fmt;
            fmt.setForeground(Qt::darkYellow);
            m_format_map[static_cast<int>(XMLSE::XML_CDATA)] = fmt;
        }

        {
            QTextCharFormat fmt;
            fmt.setForeground(QColor(0xffa500).darker(100));
            m_format_map[static_cast<int>(XMLSE::XML_PI)] = fmt;
        }
    }
}
