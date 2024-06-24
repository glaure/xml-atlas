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
#include "xa_editor.h"
#include "xa_tree_dock.h"
#include "xa_data.h"
#include "xa_xml_tree_model.h"
#include "xa_xml_tree_item.h"
#include <QtWidgets>
#include <QFontDialog>


XAMainWindow::XAMainWindow(XAData* app_data, QWidget *parent)
    : QMainWindow(parent)
    , m_main_window(new Ui::MainWindow)
    , m_app_data(app_data)
    , m_editor(nullptr)
    , m_xml_highlighter(nullptr)
    , m_tree_dock(nullptr)
    , m_tree_view(nullptr)
{
    m_main_window->setupUi(this);

    setupFileMenu();
    setupHelpMenu();

    // setup UI default
    setupDefaults();

    setupEditor();

    connect(m_main_window->actionUI_Theme, &QAction::triggered, [this]() { setupTheme(); });
    connect(m_main_window->actionFont, &QAction::triggered, [this]() { setupFont(); });
    connect(m_main_window->actionIndent, &QAction::triggered, [this]() { bool force_option = false; indentDocument(force_option); });
    connect(m_main_window->actionIndent_Options, &QAction::triggered, [this]() { bool force_option = true;indentDocument(force_option); });

    setCentralWidget(m_editor);
    setWindowTitle(tr("XML Atlas"));
}


QSize XAMainWindow::sizeHint() const
{
    return QSize{1024, 800};
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
}

void XAMainWindow::openFile(const QString &path)
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
            //m_tree_view->resizeColumnToContents(1);

        }
    }
}

void XAMainWindow::saveFile(const QString& path)
{
    //QString fileName = path;

    //if (fileName.isNull())
    //    fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", "C++ Files (*.xml *.XML)");

    //if (!fileName.isEmpty()) {
    //    QFile file(fileName);
    //    if (file.open(QFile::ReadOnly | QFile::Text))
    //        editor->setPlainText(file.readAll());
    //}
}


void XAMainWindow::indentDocument(bool force_option)
{
    if (force_option)
    {
        auto indent_ui = new Ui::IndentOptions();
        auto dlg = new QDialog();
        indent_ui->setupUi(dlg);
        dlg->exec();

        delete dlg;
    }

    auto content = m_app_data->indentDocument();
    m_editor->setPlainText(content);
}


void XAMainWindow::setupEditor()
{
    m_editor = new XAEditor;
    m_editor->setFont(m_font);

    m_xml_highlighter = new XAHighlighter_XML(m_editor->document());

    m_tree_view = new QTreeView(this);
    m_tree_view->setModel(m_app_data->getXMLTreeModel());
    m_tree_view->setHeaderHidden(true);

    bool ok = connect(m_tree_view->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &XAMainWindow::onSelectionChanged);

    m_tree_dock = new XATreeDock("XML Tree", this);
    m_tree_dock->setWidget(m_tree_view);
    addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, m_tree_dock);
}


void XAMainWindow::setupFileMenu()
{
    connect(m_main_window->actionNew, &QAction::triggered, this, [this]() { newFile(); });
    connect(m_main_window->actionOpen, &QAction::triggered, this, [this]() { openFile(); });
    connect(m_main_window->actionSave, &QAction::triggered, this, [this]() { saveFile(); });
    connect(m_main_window->actionExit, &QAction::triggered, qApp, &QApplication::quit);
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

        m_editor->markSelectedRange(tree_item->getOffset(), 20);
    }
}

void XAMainWindow::setupTheme()
{
    auto theme_ui = new Ui::ThemeOptions;
    auto dlg = new QDialog;
    theme_ui->setupUi(dlg);
    dlg->exec();


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
    } else {
        // the user canceled the dialog; font is set to the initial
        // value, in this case Helvetica [Cronyx], 10
    }
}

void XAMainWindow::setupDefaults()
{
    QFont font;
    font.setFamily("Monospace");
    font.setFixedPitch(true);
    font.setPointSize(9);

    m_font = font;
}
