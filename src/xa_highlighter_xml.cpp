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
    for (const HighlightingRule &rule : std::as_const(m_highlighting_rules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

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
        rule.pattern = QRegularExpression(QStringLiteral("<[/]?[\\s]*([^\\n][^>]*)(?=[\\s/>])"));
        rule.format = m_format_map.at(static_cast<int>(XMLSE::XML_ELEM));
        m_highlighting_rules.append(rule);
    }
    {   
        rule.pattern = QRegularExpression(QStringLiteral("\\w+\\s*(?=\\=)\\s*"));
        rule.format = m_format_map.at(static_cast<int>(XMLSE::XML_ATTR));
        m_highlighting_rules.append(rule);
    }
    {   //<?xml version="1.0" encoding="UTF-8"?>
        rule.pattern = QRegularExpression(QStringLiteral("<\\?xml.*>"));
        rule.format = m_format_map.at(static_cast<int>(XMLSE::XML_PROLOG));
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
            // Attribute value: orange/red 0xce9178
        }

        {
            // XML </> characters: grey 0x808080
        }

        {
            // XML = character: lightgrey 0xcccccc
        }
        {
            // Comment color: 0x6a9955
        }
    }
    else
    {
        {
            QTextCharFormat fmt;
            fmt.setFontWeight(QFont::Bold);
            fmt.setForeground(Qt::red);
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
            fmt.setForeground(Qt::red);
            m_format_map[static_cast<int>(XMLSE::XML_ATTR)] = fmt;
        }
    }
}
