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

#include "xa_highlighter_xml.h"
#include <QMainWindow>


class XAEditor;
class XATreeDock;
class XAData;
class QTreeView;


namespace Ui
{
    class MainWindow;
}


class XAMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    XAMainWindow(XAData* app_data, QWidget *parent = nullptr);

public slots:
    void about();
    void newFile();
    void openFile(const QString &path = QString());
    void saveFile(const QString& path = QString());

    virtual QSize sizeHint() const;

    void indentDocument(bool force_option);

protected slots:
    void onTreeItemClicked(const QModelIndex& index);

private:
    void setupEditor();
    void setupFileMenu();
    void setupHelpMenu();
    void setupTheme();
    void setupFont();
    void setupDefaults();


private:
    Ui::MainWindow*     m_main_window;
    XAData*             m_app_data;
    XAEditor*           m_editor;
    XAHighlighter_XML*  m_xml_highlighter;
    XATreeDock*         m_tree_dock;
    QTreeView*          m_tree_view;
    QFont               m_font;
};
