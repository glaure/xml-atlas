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

#include <QObject>

/**
 * Utilities to handle HighDPI displays
 */
class HighDPIUtil : public QObject
{
    Q_OBJECT
public:
    HighDPIUtil();
    ~HighDPIUtil();

    static void applyHighDpiAppSettings();

    /**
     * Refresh HighDPI Settings
     */
    void update();

    void fixAppFont();

    qreal screenScaleFactor() const;
    qreal fontScaleFactor() const;
private:
    qreal m_screen_scale_factor;
    qreal m_pixel_ratio;
};
