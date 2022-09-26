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

#include <QFileSystemWatcher>
#include <QObject>
#include <vector>

class QQuickView;

class QmlReloader : public QObject
{
    Q_OBJECT
public:
    QmlReloader();

    bool addDirectoryToWatch(const QString& dir_path, const QStringList& filters = {}, bool recursive = false);

    void addView(QQuickView* view);

Q_SIGNALS:
    void reloaded();

public Q_SLOTS:
    void reload();

private Q_SLOTS:
    void onFileChanged(const QString& path);
    void removeView(QObject* obj);

private:
    QFileSystemWatcher m_watcher;
    std::vector<QQuickView*> m_views;
};
