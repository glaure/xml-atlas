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

#include "xa_window.h"
#include "ui_xa_window.h"
#include "ui_xa_indent_options.h"
#include "ui_xa_theme_options.h"
#include "xa_app.h"
#include "xa_editor.h"
#include "xa_tableview.h"
#include "xa_tree_dock.h"
#include "xa_data.h"
#include "xa_theme.h"
#include "xa_xml_tree_model.h"
#include "xa_xml_tree_item.h"
#include <QtWidgets>
#include <QFontDialog>


XAMainWindow::XAMainWindow(XAApp* app, XAData* app_data, QWidget* parent)
    : QMainWindow(parent)
    , m_main_window(new Ui::MainWindow)
    , m_app(app)
    , m_app_data(app_data)
    , m_editor(nullptr)
    , m_xml_highlighter(nullptr)
    , m_tree_dock(nullptr)
    , m_tree_view(nullptr)
    , m_font()
    , m_recent_file_acts()
    , m_recent_file_separator(nullptr)
    , m_recent_file_submenuact(nullptr)
{
    m_main_window->setupUi(this);

    setupFileMenu();
    setupHelpMenu();

    // setup UI default
    setupDefaults();

    setupEditor();
    setupTableView();

    connect(m_main_window->actionUI_Theme, &QAction::triggered, [this]() { setupTheme(); });
    connect(m_main_window->actionFont, &QAction::triggered, [this]() { setupFont(); });
    connect(m_main_window->actionIndent, &QAction::triggered, [this]() { bool force_option = false; indentDocument(force_option); });
    connect(m_main_window->actionIndent_Options, &QAction::triggered, [this]() { bool force_option = true; indentDocument(force_option); });

    setCentralWidget(m_editor);
    setWindowTitle(tr("XML Atlas"));
    QIcon icon(":/icons/images/xmlatlas.ico");
    setWindowIcon(icon);

    onThemeChange();
    updateRecentFileActions();
}


QSize XAMainWindow::sizeHint() const
{
    return QSize{ 1024, 800 };
}

void XAMainWindow::about()
{
    QMessageBox::about(this, tr("About XML Atlas"),
        tr("<p>The <b>XML Atlas</b> is an XML editor with " \
            "tree navigation support. " \
            "</p>"));
}

void XAMainWindow::newFile()
{
    m_editor->clear();
    m_app_data->getXMLTreeModel()->clear();
    m_tree_view->reset();
}

void XAMainWindow::openFile(const QString& path)
{
    QString fileName = path;

    if (fileName.isNull())
        fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", "XML Files (*.xml *.XML)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            auto content = file.readAll();
            m_app_data->setContent(content);
            m_editor->setPlainText(content);
            m_tree_view->expandAll();

            m_tree_view->resizeColumnToContents(0);

            //{
            //    auto cw = m_tree_view->columnWidth(0);
            //    m_tree_view->setColumnWidth(0, cw + 50);
            //}

            //{
            //    auto cw = m_tree_view->columnWidth(1);
            //    m_tree_view->setColumnWidth(1, 20);
            //}

            m_tree_view->collapseAll();
            m_tree_view->expand(m_app_data->getXMLTreeModel()->index(0, 0));

            addRecentFile(fileName);
        }
    }
}

void XAMainWindow::saveFile(const QString& path)
{
    QString fileName = path;

    if (fileName.isNull())
        fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", "XML Files (*.xml *.XML)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QFile::WriteOnly | QFile::Text)) {
            QTextStream out(&file);
            out << m_editor->toPlainText();
            file.close();
            addRecentFile(fileName);
        }
        else {
            QMessageBox::warning(this, tr("Error"), tr("Cannot save file %1:\n%2.").arg(fileName, file.errorString()));
        }
    }
}


void XAMainWindow::setupEditor()
{
    m_editor = new XAEditor(m_app, this);
    m_editor->setFont(m_font);

    m_xml_highlighter = new XAHighlighter_XML(m_app, m_editor->document());

    m_tree_view = new QTreeView(this);
    m_tree_view->setModel(m_app_data->getXMLTreeModel());
    m_tree_view->setHeaderHidden(true);

    bool ok = connect(m_tree_view->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &XAMainWindow::onSelectionChanged);

    m_tree_dock = new XATreeDock("XML Tree", this);
    m_tree_dock->setWidget(m_tree_view);
    addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, m_tree_dock);
    
    //connect(m_editor, &XAEditor::textChanged, this, &XAMainWindow::onEditorTextChanged);
}

void XAMainWindow::setupTableView()
{
    m_tableView = new XATableView(this);
    QDockWidget* tableDockWidget = new QDockWidget(tr("Table View"), this);
    tableDockWidget->setWidget(m_tableView);
    addDockWidget(Qt::BottomDockWidgetArea, tableDockWidget);
}

void XAMainWindow::onEditorTextChanged()
{
    //QString xmlContent = m_editor->toPlainText();
    //m_tableView->setXmlContent(xmlContent);
    //auto& doc = m_app_data->getDocument();
}

void XAMainWindow::setupFileMenu()
{
    connect(m_main_window->actionNew, &QAction::triggered, this, [this]() { newFile(); });
    connect(m_main_window->actionOpen, &QAction::triggered, this, [this]() { openFile(); });
    connect(m_main_window->actionSave, &QAction::triggered, this, [this]() { saveFile(); });
    connect(m_main_window->actionSave_as, &QAction::triggered, this, [this]() { saveFile(); });
    connect(m_main_window->actionExit, &QAction::triggered, qApp, &QApplication::quit);

    m_recent_file_separator = m_main_window->menuFile->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i) {
        m_recent_file_acts[i] = new QAction(this);
        m_recent_file_acts[i]->setVisible(false);
        connect(m_recent_file_acts[i], &QAction::triggered, this, &XAMainWindow::openRecentFile);
        m_main_window->menuFile->addAction(m_recent_file_acts[i]);
    }
}

void XAMainWindow::setupHelpMenu()
{
    connect(m_main_window->actionAbout_XML_Atlas, &QAction::triggered, this, &XAMainWindow::about);
    connect(m_main_window->actionAbout_Qt, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void XAMainWindow::onSelectionChanged(const QModelIndex& index, const QModelIndex& previous)
{
    auto item = index.internalPointer();
    if (item)
    {
        auto tree_item = reinterpret_cast<XAXMLTreeItem*>(item);

        // mark
        m_editor->markSelectedRange(tree_item->getOffset(), 20);

        // update table view
        //auto& doc = m_app_data->getDocument();
        m_tableView->setTableRootNode(tree_item->getNode());
    }
}

void XAMainWindow::onThemeChange()
{
    auto theme = m_app->getTheme();

    m_main_window->actionNew->setIcon(theme->getIcon("new-doc.png"));
    m_main_window->actionOpen->setIcon(theme->getIcon("open-doc.png"));
    m_main_window->actionSave->setIcon(theme->getIcon("save-doc.png"));

    m_main_window->actionCut->setIcon(theme->getIcon("cut.png"));
    m_main_window->actionCopy->setIcon(theme->getIcon("copy.png"));
    m_main_window->actionPaste->setIcon(theme->getIcon("paste.png"));
    m_main_window->actionUndo->setIcon(theme->getIcon("undo.png"));
    m_main_window->actionRedo->setIcon(theme->getIcon("redo.png"));

    m_main_window->actionIndent->setIcon(theme->getIcon("indent.png"));
    m_main_window->actionFind->setIcon(theme->getIcon("search.png"));
    m_main_window->actionLocate_in_tree->setIcon(theme->getIcon("location.png"));

    m_main_window->actionDrill_down->setIcon(theme->getIcon("down-arrow.png"));
    m_main_window->actionDrill_up->setIcon(theme->getIcon("up-arrow.png"));

}

void XAMainWindow::setupTheme()
{
    auto theme_ui = new Ui::ThemeOptions;
    auto dlg = new QDialog;
    theme_ui->setupUi(dlg);

    auto theme = m_app->getTheme();
    QString current_theme = m_app->retrieveColorTheme();

    if (current_theme == "dark")
    {
        theme_ui->m_style->setCurrentIndex(1);
    }
    else
    {
        theme_ui->m_style->setCurrentIndex(0);
    }


    connect(theme_ui->m_style, QOverload<int>::of(&QComboBox::currentIndexChanged), [theme, this](int index) {
        QString selected_theme = (index == 0) ? "light" : "dark";
        changeTheme(selected_theme);
        });

    auto ret = dlg->exec();
    if (ret != QDialog::Accepted)
    {
        changeTheme(current_theme);
    }

    delete dlg;
}

void XAMainWindow::setupFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(
        &ok, m_font, this);
    if (ok) {
        m_font = font;
        m_editor->setFont(m_font);
    }
    else {
        // the user canceled the dialog; font is set to the initial
        // value, in this case Helvetica [Cronyx], 10
    }
}

void XAMainWindow::indentDocument(bool force_option)
{
    int indent_size = 4;
    int max_attr_per_line = 6;
    bool use_spaces = true;

    if (force_option)
    {
        auto indent_ui = new Ui::IndentOptions();
        auto dlg = new QDialog();
        indent_ui->setupUi(dlg);
        dlg->exec();

        delete dlg;
    }
    //m_app_data->setContent(m_editor->toPlainText());
    auto content = m_app_data->indentDocument(indent_size, max_attr_per_line, use_spaces);
    m_editor->setPlainText(content);
}


void XAMainWindow::setupDefaults()
{
    QFont font;
    font.setFamily("Monospace");
    font.setFixedPitch(true);
    font.setPointSize(9);

    m_font = font;
}

void XAMainWindow::changeTheme(const QString& selected_theme)
{
    auto theme = m_app->getTheme();
    theme->selectColorTheme(selected_theme);
    m_app->storeColorTheme(selected_theme);

    this->onThemeChange();
    if (m_xml_highlighter)
    {
        m_xml_highlighter->onThemeChange();
    }
}

void XAMainWindow::updateRecentFileActions()
{
    auto& settings = m_app->getSettings();
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
        m_recent_file_acts[i]->setText(text);
        m_recent_file_acts[i]->setData(files[i]);
        m_recent_file_acts[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        m_recent_file_acts[j]->setVisible(false);

    m_recent_file_separator->setVisible(numRecentFiles > 0);
}

void XAMainWindow::openRecentFile()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action)
        openFile(action->data().toString());
}

void XAMainWindow::addRecentFile(const QString& filePath)
{
    auto& settings = m_app->getSettings();
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(filePath);
    files.prepend(filePath);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("recentFileList", files);

    updateRecentFileActions();
}