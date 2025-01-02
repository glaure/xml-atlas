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
#include "xa_hidpi.h"
#include "xa_theme.h"

#include <QFile>
#include <QFileInfo>


XAApp::XAApp(int& argc, char** argv)
    : QObject(nullptr)
    , m_app(argc, argv)
    , m_app_data(nullptr)
    , m_theme(std::make_unique<XATheme>())
{
    HighDPIUtil hidpi;
    hidpi.fixAppFont();

    if (m_theme->isSystemDarkMode())
    {
        m_theme->selectColorTheme("dark");
    }
    else 
    {
        m_theme->selectColorTheme("light");
    }
}

XAApp::~XAApp()
{
    delete m_app_data;
    delete m_window;
}

bool XAApp::init()
{
    m_app_data = new XAData;
    m_window = new XAMainWindow(this, m_app_data);

    if (!initGui()) return false;

    return true;
}

bool XAApp::initGui()
{
    m_window->show();
    return true;
}

int XAApp::run()
{
    int ret = 0;

    ret = m_app.exec();

    return ret;
}

XATheme* XAApp::getTheme() 
{ 
    return m_theme.get(); 
}
