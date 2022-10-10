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
#include <QtWidgets>


XAMainWindow::XAMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupFileMenu();
    setupHelpMenu();
    setupEditor();

    setCentralWidget(editor);
    setWindowTitle(tr("XML Atlas"));


}


QSize XAMainWindow::sizeHint() const
{
    return QSize{640, 480};
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
    editor->clear();
}

void XAMainWindow::openFile(const QString &path)
{
    QString fileName = path;

    if (fileName.isNull())
        fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", "C++ Files (*.xml *.XML)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly | QFile::Text))
            editor->setPlainText(file.readAll());
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

    //editor = new QPlainTextEdit;
    editor = new XAEditor;
    editor->setFont(font);

    m_xml_highlighter = new XAHighlighter_XML(editor->document());

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


