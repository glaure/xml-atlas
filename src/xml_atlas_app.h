/* 
 * This file is part of the AppUpdate (https://github.com/DEWETRON/AppUpdate)
 * Copyright (c) DEWETRON GmbH 2020.
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

#include <QApplication>
#include <QObject>
#include "qml_reloader.h"

class XmlAtlasData;
class XmlAtlasQml;

/**
 * Initializes the application and manages the GUI
 */
class XmlAtlasApp : public QObject
{
    Q_OBJECT;

public:
    XmlAtlasApp(int& argc, char** argv);
    ~XmlAtlasApp();

    bool init();
    bool initGui();

    /**
     * Starts the GUI application and only returns (with an exit code) when the application should terminate
     */
    int run();


private:


private:
    QApplication            m_app;
    XmlAtlasData*           m_app_data;
    XmlAtlasQml*            m_main_window;
    QString                 m_qml_main_file;
    QmlReloader*            m_qml_reloader;
};