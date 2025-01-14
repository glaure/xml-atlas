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

class XAApp;
class XAEditor;
class XATableView;
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
    XAMainWindow(XAApp* app, XAData* app_data, QWidget *parent = nullptr);

public slots:
    void about();
    void newFile();
    void openFile(const QString &path = QString());
    void saveFile(const QString& path = QString());

    virtual QSize sizeHint() const;

    void indentDocument(bool force_option);

protected slots:
    void onSelectionChanged(const QModelIndex& index, const QModelIndex& previous);
    void onThemeChange();
    void onFind();
    void onFindNext();

private:
    void setupEditor();
    void setupShortCuts();
    void setupTableView();
    void setupFileMenu();
    void setupHelpMenu();
    void setupTheme();
    void setupFont();
    void setupDefaults();
    void changeTheme(const QString& selected_theme);
    void updateRecentFileActions();
    void openRecentFile();
    void addRecentFile(const QString& file_path);
    void onEditorTextChanged();
    void findInEditor(const QString& searchTerm);

private:
    Ui::MainWindow*     m_main_window;
    XAApp*              m_app;
    XAData*             m_app_data;
    XAEditor*           m_editor;
    QTextCursor         m_searchCursor;
    XATableView*        m_tableView;
    XAHighlighter_XML*  m_xml_highlighter;
    XATreeDock*         m_tree_dock;
    QTreeView*          m_tree_view;
    QFont               m_font;

    enum { MaxRecentFiles = 10 };
    QAction* m_recent_file_acts[MaxRecentFiles];
    QAction* m_recent_file_separator;
    QAction* m_recent_file_submenuact;
};
