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

#include "qml_reloader.h"

#include <QDirIterator>
#include <QQmlEngine>
#include <QQuickView>


QmlReloader::QmlReloader()
    : QObject(nullptr)
{
    connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, &QmlReloader::onFileChanged);
}

bool QmlReloader::addDirectoryToWatch(const QString& dir_path, const QStringList& filters, bool recursive)
{
    QStringList watcher_failures;

    QDirIterator dir_iter(dir_path, filters, QDir::Files,
        recursive ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);
    QStringList files;

    while (dir_iter.hasNext())
    {
        dir_iter.next();
        files << dir_iter.filePath();
    }

    watcher_failures.append(m_watcher.addPaths(files));

    if (!watcher_failures.empty())
    {
        return false;
    }
    return true;
}

void QmlReloader::reload()
{
    std::vector<QQmlEngine*> engines;
    for (auto view : m_views)
    {
        auto view_engine = view->engine();
        auto match = std::find_if(engines.begin(), engines.end(),
            [&view_engine](QQmlEngine* engine)
        {
            return view_engine == engine;
        });

        if (match == engines.end())
        {
            view->engine()->clearComponentCache();
            engines.push_back(view_engine);
            view->setSource(view->source());
        }
    }

    Q_EMIT reloaded();
}

void QmlReloader::onFileChanged(const QString& path)
{
    reload();
}

void QmlReloader::addView(QQuickView* view)
{
    auto match = std::find_if(m_views.begin(), m_views.end(),
        [&view](QQuickView* it_view)
    {
        return view == it_view;
    });

    if (match != m_views.end())
    {
        return;
    }

    m_views.push_back(view);
    connect(view, &QObject::destroyed, this, &QmlReloader::removeView);
}

void QmlReloader::removeView(QObject* obj)
{
    auto match = std::find_if(m_views.begin(), m_views.end(),
        [&obj](QQuickView* view)
    {
        return obj == view;
    });

    if (match != m_views.end())
    {
        m_views.erase(match);
        disconnect(obj, 0, this, 0);
    }
}
