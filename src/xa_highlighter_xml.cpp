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
    auto debug = text.toStdString();

    for (const HighlightingRule &rule : std::as_const(m_highlighting_rules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

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

    // Handle multi-line attributes
    //startIndex = 0;
    //if (previousBlockState() != 3) {
    //    startIndex = text.indexOf("<");
    //}

    //while (startIndex >= 0) {
    //    int endIndex = text.indexOf(">", startIndex);
    //    int elementLength;
    //    if (endIndex == -1) {
    //        setCurrentBlockState(3);
    //        elementLength = text.length() - startIndex;
    //    }
    //    else {
    //        elementLength = endIndex - startIndex + 1;
    //    }
    //    setFormat(startIndex, elementLength, m_format_map.at(static_cast<int>(XMLSE::XML_ELEM)));
    //    startIndex = text.indexOf("<", startIndex + elementLength);
    //}
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

    m_highlighting_rules.clear();

    {
        //<ABC></ABC>
        //<ABC/>
        rule.pattern = QRegularExpression(QStringLiteral("<[/]?[\\s]*([a-zA-Z_][\\w\\-\\.]*)[\\s/>]"));
        rule.format = m_format_map.at(static_cast<int>(XMLSE::XML_ELEM));
        m_highlighting_rules.append(rule);
    }
    {   // Processing instruction
        rule.pattern = QRegularExpression(QStringLiteral("<\\?.*\\?>"));
        rule.format = m_format_map.at(static_cast<int>(XMLSE::XML_PI));
        m_highlighting_rules.append(rule);
    }
    {   //<?xml version="1.0" encoding="UTF-8"?>
        rule.pattern = QRegularExpression(QStringLiteral("<\\?xml.*>"));
        rule.format = m_format_map.at(static_cast<int>(XMLSE::XML_PROLOG));
        m_highlighting_rules.append(rule);
    }
    {   // XML attribute
        rule.pattern = QRegularExpression(QStringLiteral("\\b[a-zA-Z_][\\w\\-\\.]*\\b(?=\\s*=)"));
        rule.format = m_format_map.at(static_cast<int>(XMLSE::XML_ATTR));
        m_highlighting_rules.append(rule);
    }
    {   // Attribute value
        rule.pattern = QRegularExpression(QStringLiteral("\"[^\"]*\""));
        rule.format = m_format_map.at(static_cast<int>(XMLSE::XML_ATTR_VALUE));
        m_highlighting_rules.append(rule);
    }

    {   //<!-- comment -->
        rule.pattern = QRegularExpression(QStringLiteral("<!--[^>]*-->"));
        rule.format = m_format_map.at(static_cast<int>(XMLSE::XML_COMMENT));
        m_highlighting_rules.append(rule);
    }
    {   //<!-- multi-line comment -->
        rule.pattern = QRegularExpression(QStringLiteral("<!--"));
        rule.format = m_format_map.at(static_cast<int>(XMLSE::XML_MULTILINE_COMMENT));
        m_highlighting_rules.append(rule);
    }
    {   //<![CDATA[ ... ]]>
        rule.pattern = QRegularExpression(QStringLiteral("<!\\[CDATA\\[.*\\]\\]>"));
        rule.format = m_format_map.at(static_cast<int>(XMLSE::XML_CDATA));
        m_highlighting_rules.append(rule);
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
