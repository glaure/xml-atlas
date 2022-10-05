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

#include "xa_app.h"
#include "xa_data.h"
#include "xa_editor.h"
#include "xa_window.h"

#include "QFile"
#include <QFileInfo>
#include <QQmlContext>
#include <QQmlEngine>


XAApp::XAApp(int& argc, char** argv)
    : QObject(nullptr)
    , m_app(argc, argv)
    , m_app_data(nullptr)
{

}

XAApp::~XAApp()
{
    delete m_app_data;
    delete m_editor;
    delete m_window;
}

bool XAApp::init()
{
    m_app_data = new XAData;
    m_editor = new XAEditor;
    m_window = new XAMainWindow;

    if (!initGui()) return false;

    return true;
}

bool XAApp::initGui()
{
    // m_main_window = new XmlAtlasQml;
    // m_main_window->connect(m_main_window->engine(), &QQmlEngine::quit, &m_app, &QCoreApplication::quit);
    // m_main_window->setSource(QUrl(m_qml_main_file));

    // // connect(m_app_data, &XAData::doShowNotification, m_main_window, &XmlAtlasQml::showNotification);
    // // connect(m_app_data, &XAData::resetAlertIcon, m_main_window, &XmlAtlasQml::resetAlertIcon);


    // if (m_main_window->status() == QQuickView::Error)
    //     return false;

    // m_main_window->setResizeMode(QQuickView::SizeRootObjectToView);
    
    // m_main_window->rootContext()->setContextProperty("app", m_app_data);


    // m_main_window->hide();
    // if (m_qml_reloader) 
    // {
    //     m_qml_reloader->addView(m_main_window);
    // }

    m_editor->show();
    m_window->show();

    return true;
}

int XAApp::run()
{
    int ret = 0;

    ret = m_app.exec();

    return ret;
}
