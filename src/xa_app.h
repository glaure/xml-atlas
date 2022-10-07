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

#include <QApplication>
#include <QObject>

class XAData;
class XAEditor;
class XAMainWindow;

/**
 * Initializes the application and manages the GUI
 */
class XAApp : public QObject
{
    Q_OBJECT;

public:
    XAApp(int& argc, char** argv);
    ~XAApp();

    bool init();
    bool initGui();

    /**
     * Starts the GUI application and only returns (with an exit code) when the application should terminate
     */
    int run();


private:


private:
    QApplication      m_app;
    XAData*           m_app_data;
    //XAEditor*         m_editor;
    XAMainWindow*     m_window;
};