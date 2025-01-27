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

#include "xa_editor.h"
#include <QPainter>
#include <QTextBlock>
#include <QRegularExpression>
#include "xa_app.h"
#include "xa_theme.h"
#include "xa_xml_tree_item.h"
#include "pugixml.hpp"


XAEditor::XAEditor(XAApp* app, QWidget *parent) 
    : QPlainTextEdit(parent)
    , m_theme(app->getTheme())
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, &XAEditor::blockCountChanged, this, &XAEditor::updateLineNumberAreaWidth);
    connect(this, &XAEditor::updateRequest, this, &XAEditor::updateLineNumberArea);
    connect(this, &XAEditor::cursorPositionChanged, this, &XAEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    setUndoRedoEnabled(true);
}

int XAEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

void XAEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void XAEditor::markSelectedRange(const XAXMLTreeItem* item)
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = m_theme->getColorTheme() == "dark" ? QColor(0x264f78) : QColor(0xbbd5fd);

        selection.format.setBackground(lineColor);

        // highlight selection
        auto offset = findFirstElementPos(item);
        auto last_offset = findEndElementPos(item);
        auto length = last_offset - offset;

        QTextCursor cursor = textCursor();
        cursor.setPosition(static_cast<int>(offset), QTextCursor::MoveAnchor);
        cursor.setPosition(static_cast<int>(offset + length), QTextCursor::KeepAnchor);
        selection.cursor = cursor;
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
    if (!extraSelections.isEmpty()) {
        QTextCursor cursor = extraSelections.first().cursor;
        cursor.setPosition(cursor.selectionStart(), QTextCursor::MoveAnchor);
        setTextCursor(cursor);
        ensureCursorVisible();
    }
}

size_t XAEditor::findFirstElementPos(const XAXMLTreeItem* item)
{
    auto node = item->getNode();
    auto offset = node.offset_debug();

    switch (item->getItemType())
    {
    case XAXMLTreeItemType::ELEMENT:
    {
        return offset - 1;
    } break;

    case XAXMLTreeItemType::ATTRIBUTE:
    {
        auto node = item->getNode();
        auto attribute_name = QString::fromStdString(item->getValue());
        QString pattern = QStringLiteral("\\b%1\\b").arg(QRegularExpression::escape(attribute_name));
        QRegularExpression regex(pattern);

        QTextDocument* document = this->document();
        auto cursor = document->find(regex, offset);
        if (cursor.isNull()) {
            return 0; // Attribute not found
        }
        return cursor.selectionStart(); // Return the start of the match

    } break;

    default:
        break;
    }

    return 0;
}


class CountElementVisitor : public pugi::xml_tree_walker
{
public:
    CountElementVisitor(const std::string& elem_name) 
        : m_element_name(elem_name)
        , m_count(0) 
    {}

    virtual bool for_each(pugi::xml_node& node)
    {
        if (node.type() == pugi::node_element)
        {
            if (node.name() == m_element_name)
            {
                m_count++;
            }
        }
        return true;
    }

    size_t getCount() const 
    { 
        return m_count; 
    }

private:
    std::string m_element_name;
    int m_count;
};


size_t XAEditor::findEndElementPos(const XAXMLTreeItem* item)
{
    auto node = item->getNode();
    auto offset = node.offset_debug();

    switch (item->getItemType())
    {
    case XAXMLTreeItemType::ELEMENT:
    {
        auto end_offset = offset;
        QTextDocument* document = this->document();

        auto child_count = std::distance(node.begin(), node.end());
        if (child_count == 0)
        {
            QString pattern = QStringLiteral("/>").arg(QRegularExpression::escape(node.name()));
            QRegularExpression regex(pattern);
            auto cursor = document->find(regex, end_offset);
            if (cursor.isNull()) {
                return 0; // End element not found
            }
            end_offset = cursor.selectionEnd(); // Return the end of the match
        }
        else
        {
            // Count nested child elements with the same name
            CountElementVisitor visitor(node.name());
            node.traverse(visitor);

            QString pattern = QStringLiteral("</%1>").arg(QRegularExpression::escape(node.name()));
            QRegularExpression regex(pattern);

            for (int i = 0; i <= visitor.getCount(); i++)
            {
                auto cursor = document->find(regex, end_offset);
                if (cursor.isNull()) {
                    return 0; // End element not found
                }
                end_offset = cursor.selectionEnd(); // Return the end of the match
            }
        }
        return end_offset;
    } break;

    case XAXMLTreeItemType::ATTRIBUTE:
    {
        auto attribute_name = QString::fromStdString(item->getValue());
        QString pattern = QStringLiteral("\\b%1\\b\\s*=\\s*\"([^\"]*)\"").arg(QRegularExpression::escape(attribute_name));
        QRegularExpression regex(pattern);

        QTextDocument* document = this->document();
        auto cursor = document->find(regex, offset);
        if (cursor.isNull()) {
            return 0; // Attribute not found
        }

        // Get the matched text
        auto match = regex.match(document->toPlainText(), cursor.selectionStart());
        if (match.hasMatch()) {
            auto attribute_value = match.captured(1);
            return cursor.selectionStart() + match.capturedLength(0); // End of the attribute value
        }
    } break;

    default:
        break;
    }

    return 0;
}

void XAEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void XAEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void XAEditor::highlightCurrentLine()
{
    //QList<QTextEdit::ExtraSelection> extraSelections;

    //if (!isReadOnly()) {
    //    QTextEdit::ExtraSelection selection;

    //    QColor lineColor = QColor(Qt::yellow).lighter(160);

    //    selection.format.setBackground(lineColor);
    //    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    //    selection.cursor = textCursor();
    //    selection.cursor.clearSelection();
    //    extraSelections.append(selection);
    //}

    //setExtraSelections(extraSelections);
}

void XAEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);



    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());



    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

