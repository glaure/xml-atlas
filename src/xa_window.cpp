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
#include "xa_editor.h"
#include "xa_tree_dock.h"
#include "xa_data.h"
#include "xa_xml_tree_model.h"
#include "xa_xml_tree_item.h"
#include <QtWidgets>


XAMainWindow::XAMainWindow(XAData* app_data, QWidget *parent)
    : QMainWindow(parent)
    , m_app_data(app_data)
    , m_editor(nullptr)
    , m_xml_highlighter(nullptr)
    , m_tree_dock(nullptr)
    , m_tree_view(nullptr)
{
    setupFileMenu();
    setupHelpMenu();
    setupEditor();

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

void XAMainWindow::setupEditor()
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    m_editor = new XAEditor;
    m_editor->setFont(font);

    m_xml_highlighter = new XAHighlighter_XML(m_editor->document());

    m_tree_view = new QTreeView;
    m_tree_view->setModel(m_app_data->getXMLTreeModel());
    m_tree_view->setHeaderHidden(true);

    bool ok = connect(m_tree_view, &QAbstractItemView::clicked, this, &XAMainWindow::onTreeItemClicked);

    m_tree_dock = new XATreeDock("XML Tree");
    m_tree_dock->setWidget(m_tree_view);
    addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, m_tree_dock);
}


void XAMainWindow::setupFileMenu()
{
    QMenu *fileMenu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(fileMenu);

    QToolBar* fileToolBar = addToolBar(tr("File"));   
    {
        const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
        QAction* newAct = new QAction(newIcon, tr("&New"), this);
        newAct->setShortcuts(QKeySequence::New);
        newAct->setStatusTip(tr("Create a new file"));
        connect(newAct, &QAction::triggered, this, &XAMainWindow::newFile);
        fileMenu->addAction(newAct);
        fileToolBar->addAction(newAct);
    }


    {
        const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
        QAction* openAct = new QAction(openIcon, tr("&Open..."), this);
        openAct->setShortcuts(QKeySequence::Open);
        openAct->setStatusTip(tr("Open an existing file"));
        //connect(openAct, &QAction::triggered, this, &XAMainWindow::open);
        connect(openAct, &QAction::triggered, this, [this]() { openFile(); });
        fileMenu->addAction(openAct);
        fileToolBar->addAction(openAct);
    }

    {
        const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
        QAction* saveAct = new QAction(saveIcon, tr("&Save"), this);
        saveAct->setShortcuts(QKeySequence::Save);
        saveAct->setStatusTip(tr("Save the document to disk"));
        //connect(saveAct, &QAction::triggered, this, &XAMainWindow::save);
        connect(saveAct, &QAction::triggered, this, [this]() { saveFile(); });
        fileMenu->addAction(saveAct);
        fileToolBar->addAction(saveAct);
    }

    //fileMenu->addAction(tr("&New"), this, &XAMainWindow::newFile, QKeySequence::New);
    //fileMenu->addAction(tr("&Open..."), this, [this]() { openFile(); },QKeySequence::Open);
    fileMenu->addAction(tr("E&xit"), qApp, &QApplication::quit, QKeySequence::Quit);

}

void XAMainWindow::setupHelpMenu()
{
    QMenu *helpMenu = new QMenu(tr("&Help"), this);
    menuBar()->addMenu(helpMenu);

    helpMenu->addAction(tr("&About"), this, &XAMainWindow::about);
    helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
}


void XAMainWindow::onTreeItemClicked(const QModelIndex& index)
{
    auto item = index.internalPointer();
    if (item)
    {
        auto tree_item = reinterpret_cast<XAXMLTreeItem*>(item);

        m_editor->markSelectedRange(tree_item->getOffset(), 20);
    }
}
