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

#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QPalette>
#include <QStyleHints>

XAApp::XAApp(int& argc, char** argv)
    : QObject(nullptr)
    , m_app(argc, argv)
    , m_app_data(nullptr)
{
    HighDPIUtil hidpi;
    hidpi.fixAppFont();

    //selectColorTheme("dark");
    //selectColorTheme("light");
    if (isDarkMode())
    {
        selectColorTheme("dark");
    }
    else {
        selectColorTheme("light");
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
    m_window = new XAMainWindow(m_app_data);

    if (!initGui()) return false;

    return true;
}

bool XAApp::initGui()
{
    m_window->show();
    return true;
}

bool XAApp::isDarkMode() const
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    const auto scheme = QGuiApplication::styleHints()->colorScheme();
    return scheme == Qt::ColorScheme::Dark;
#else
    const QPalette defaultPalette;
    const auto text = defaultPalette.color(QPalette::WindowText);
    return text.lightness() > window.lightness();
#endif // QT_VERSION
}

void XAApp::selectColorTheme(const QString& color_theme)
{
    QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);

    //auto app = qobject_cast<QApplication*>(QApplication::instance());
    //if (app)
    //{
    //    //auto old_theme = m_color_theme.toStdString();
    //    auto new_theme = color_theme.toStdString();

    //    QFile file(QString(":/%1.qss").arg(color_theme));
    //    //QFile file(":/dark.qss");
    //    //QFile file(":/light.qss");
    //    file.open(QFile::ReadOnly | QFile::Text);
    //    if (file.isOpen()) {
    //        QTextStream stream(&file);
    //        app->setStyleSheet(stream.readAll());
    //    }
    //    else {
    //        app->setStyleSheet({});
    //    }
    //    //m_color_theme = color_theme;
    //}
}

int XAApp::run()
{
    int ret = 0;

    ret = m_app.exec();

    return ret;
}
