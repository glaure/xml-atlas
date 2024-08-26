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


XAHighlighter_XML::XAHighlighter_XML(QObject* parent) :
    QSyntaxHighlighter(parent)
{
    // setRegexes();
    // setFormats();
    init();
}

XAHighlighter_XML::XAHighlighter_XML(QTextDocument* parent) :
    QSyntaxHighlighter(parent)
{
    // setRegexes();
    // setFormats();
    init();
}

//XAHighlighter_XML::XAHighlighter_XML(QTextEdit* parent) :
//    QSyntaxHighlighter(parent)
//{
//    setRegexes();
//    setFormats();
//}

void XAHighlighter_XML::highlightBlock(const QString& text)
{
    // Special treatment for xml element regex as we use captured text to emulate lookbehind
    // int xmlElementIndex = m_xmlElementRegex.indexIn(text);
    // while (xmlElementIndex >= 0)
    // {
    //     int matchedPos = m_xmlElementRegex.pos(1);
    //     int matchedLength = m_xmlElementRegex.cap(1).length();
    //     setFormat(matchedPos, matchedLength, m_xmlElementFormat);

    //     xmlElementIndex = m_xmlElementRegex.indexIn(text, matchedPos + matchedLength);
    // }

    // // Highlight xml keywords *after* xml elements to fix any occasional / captured into the enclosing element
    // typedef QList<QRegExp>::const_iterator Iter;
    // Iter xmlKeywordRegexesEnd = m_xmlKeywordRegexes.end();
    // for (Iter it = m_xmlKeywordRegexes.begin(); it != xmlKeywordRegexesEnd; ++it) {
    //     const QRegExp& regex = *it;
    //     highlightByRegex(m_xmlKeywordFormat, regex, text);
    // }

    // highlightByRegex(m_xmlAttributeFormat, m_xmlAttributeRegex, text);
    // highlightByRegex(m_xmlCommentFormat, m_xmlCommentRegex, text);
    // highlightByRegex(m_xmlValueFormat, m_xmlValueRegex, text);


    for (const HighlightingRule &rule : std::as_const(m_highlighting_rules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    // int startIndex = 0;
    // if (previousBlockState() != 1)
    //     startIndex = text.indexOf(commentStartExpression);


    // while (startIndex >= 0) {
    //     QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
    //     int endIndex = match.capturedStart();
    //     int commentLength = 0;
    //     if (endIndex == -1) {
    //         setCurrentBlockState(1);
    //         commentLength = text.length() - startIndex;
    //     } else {
    //         commentLength = endIndex - startIndex
    //                         + match.capturedLength();
    //     }
    //     setFormat(startIndex, commentLength, multiLineCommentFormat);
    //     startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    // }
}

void XAHighlighter_XML::highlightByRegex(const QTextCharFormat& format,
    const QRegularExpression& regex, const QString& text)
{
    // int index = regex.indexIn(text);

    // while (index >= 0)
    // {
    //     int matchedLength = regex.matchedLength();
    //     setFormat(index, matchedLength, format);

    //     index = regex.indexIn(text, index + matchedLength);
    // }
}

// void XAHighlighter_XML::setRegexes()
// {
//     m_xmlElementRegex.setPattern("<[?\\s]*[/]?[\\s]*([^\\n][^>]*)(?=[\\s/>])");
//     m_xmlAttributeRegex.setPattern("\\w+\\s*(?=\\=)\\s*");
//     m_xmlValueRegex.setPattern("\"[^\\n\"]+\"(?=[?\\s/>])");
//     m_xmlCommentRegex.setPattern("<!--[^\\n]*-->");

//     // m_xmlKeywordRegexes = QList<QRegExp>() << QRegExp("<\\?") << QRegExp("/>")
//     //     << QRegExp(">") << QRegExp("<") << QRegExp("</")
//     //     << QRegExp("\\?>");
// }

// void XAHighlighter_XML::setFormats()
// {
//     m_xmlKeywordFormat.setForeground(Qt::blue);
//     m_xmlKeywordFormat.setFontWeight(QFont::Bold);

//     m_xmlElementFormat.setForeground(Qt::darkBlue);
//     m_xmlElementFormat.setFontWeight(QFont::Bold);

//     m_xmlAttributeFormat.setForeground(Qt::darkGreen);
//     m_xmlAttributeFormat.setFontWeight(QFont::Bold);
//     m_xmlAttributeFormat.setFontItalic(true);

//     m_xmlValueFormat.setForeground(Qt::darkRed);

//     m_xmlCommentFormat.setForeground(Qt::gray);
// }


void XAHighlighter_XML::init()
{
    HighlightingRule rule;


    {   //<?xml version="1.0" encoding="UTF-8"?>
        QTextCharFormat prolog_fmt;
        prolog_fmt.setFontWeight(QFont::Bold);
        prolog_fmt.setForeground(Qt::red);
        rule.pattern = QRegularExpression(QStringLiteral("<?xml.*>"));
        rule.format = prolog_fmt;
        m_highlighting_rules.append(rule);
    }
    {   
        //<ABC></ABC>
        //<ABC/>
        QTextCharFormat elem_fmt;
        elem_fmt.setFontWeight(QFont::Bold);
        elem_fmt.setForeground(Qt::darkBlue);
        rule.pattern = QRegularExpression(QStringLiteral("<[/]?[\\s]*([^\\n][^>]*)(?=[\\s/>])"));
        rule.format = elem_fmt;
        //m_highlighting_rules.append(rule);
    }

}
