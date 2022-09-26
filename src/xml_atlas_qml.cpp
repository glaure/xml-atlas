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

#include "xml_atlas_qml.h"

#include <QCoreApplication>
#include <QMenu>
#include <QMessageBox>

XmlAtlasQml::XmlAtlasQml()
{
    createActions();
    createTrayIcon();

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &XmlAtlasQml::iconActivated);
    connect(m_trayIcon, &QSystemTrayIcon::messageClicked, this, &XmlAtlasQml::messageClicked);

    m_trayIcon->show();

    setTitle(tr("DEWETRON AppUpdate"));
}


bool XmlAtlasQml::event(QEvent* event)
{
    if (event->type() == QEvent::Close) {
        if (m_trayIcon->isVisible())
        {
            QMessageBox::information(nullptr, tr("DEWETRON AppUpdate"),
                tr("The program will keep running in the "
                    "system tray. To terminate the program, "
                    "choose <b>Quit</b> in the context menu "
                    "of the system tray entry."));
            hide();
            return true;
        }
        else
        {
            return QQuickView::event(event);
        }
    }
    return QQuickView::event(event);
}

void XmlAtlasQml::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        //iconComboBox->setCurrentIndex((iconComboBox->currentIndex() + 1) % iconComboBox->count());
        this->showNormal();
        break;
    case QSystemTrayIcon::MiddleClick:
        //showMessage();
        break;
    default:
        ;
    }
}

void XmlAtlasQml::resetAlertIcon()
{
    auto icon = QIcon(":/res/dewetron.ico");
    m_trayIcon->setIcon(icon);
    //this->setIcon(icon);
}

void XmlAtlasQml::showNotification(const QString& title, const QString& body)
{
    auto icon = QIcon(":/res/dewetron_alert.ico");
    m_trayIcon->setIcon(icon);
    //this->setIcon(icon);

    m_trayIcon->showMessage(title, body);
}

void XmlAtlasQml::messageClicked()
{
    this->raise();
    this->showNormal();
}

void XmlAtlasQml::createActions()
{
    m_minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(m_minimizeAction, &QAction::triggered, this, &QQuickView::hide);

    m_maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(m_maximizeAction, &QAction::triggered, this, &QQuickView::showMaximized);

    m_restoreAction = new QAction(tr("&Restore"), this);
    connect(m_restoreAction, &QAction::triggered, this, &QQuickView::showNormal);

    m_quitAction = new QAction(tr("&Quit"), this);
    connect(m_quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}


void XmlAtlasQml::createTrayIcon()
{
    m_trayIconMenu = new QMenu();
    m_trayIconMenu->addAction(m_minimizeAction);
    m_trayIconMenu->addAction(m_maximizeAction);
    m_trayIconMenu->addAction(m_restoreAction);
    m_trayIconMenu->addSeparator();
    m_trayIconMenu->addAction(m_quitAction);

    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setContextMenu(m_trayIconMenu);

    auto icon = QIcon(":/res/dewetron.ico");
    m_trayIcon->setIcon(icon);
    this->setIcon(icon);

    m_trayIcon->setToolTip("DEWETRON AppUpdate");
}