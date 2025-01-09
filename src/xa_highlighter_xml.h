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

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <map>

class QTextDocument;
class QTextEdit;
class XAApp;

enum class XMLSE
{
    XML_PROLOG,
    XML_ELEM,
    XML_ATTR,
    XML_ATTR_VALUE,
    XML_COMMENT,
    XML_MULTILINE_COMMENT,
    XML_CDATA,
    XML_PI,
};


class XAHighlighter_XML : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    XAHighlighter_XML(XAApp* app, QTextDocument* parent);

    void onThemeChange();

protected:
    virtual void highlightBlock(const QString& text);


private:
    void highlightByRegex(const QTextCharFormat& format,
        const QRegularExpression& regex, const QString& text);

    void init();
    void updateFormatMap();

private:
    XAApp* m_app;
    std::map<int, QTextCharFormat>  m_format_map;

    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QList<HighlightingRule> m_highlighting_rules;

};
