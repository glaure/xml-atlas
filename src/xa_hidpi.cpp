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

#include "xa_hidpi.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QtGlobal>

HighDPIUtil::HighDPIUtil()
    : m_screen_scale_factor(1.0)
    , m_pixel_ratio(1.0)
{
    update();
}

HighDPIUtil::~HighDPIUtil()
{

}

void HighDPIUtil::applyHighDpiAppSettings()
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#if QT_VERSION >= 0x051400
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
}

qreal HighDPIUtil::screenScaleFactor() const
{
    return m_screen_scale_factor;
}

qreal HighDPIUtil::fontScaleFactor() const
{
    return m_pixel_ratio;
}

void HighDPIUtil::update()
{
    auto desktop = qApp->desktop();
    auto dpi_x = desktop->logicalDpiX();
    auto dpi_y = desktop->logicalDpiY();

    auto phy_dpi_x = desktop->physicalDpiX();

    m_pixel_ratio = desktop->devicePixelRatioF();
    m_screen_scale_factor = dpi_x / 96.0;  // 96 == default dpi
}

void HighDPIUtil::fixAppFont()
{
    auto platform_name = QGuiApplication::platformName();
    if (platform_name == "wayland") return;

    // Windows and xcb seem to need this font fix
    auto font = qApp->font();
    int font_size = font.pointSize();
    font_size = font_size * fontScaleFactor();
    if (font_size & 0x1) font_size += 1; // even the size for better rendering
    font.setPointSize(font_size);
    font.setStyleStrategy(QFont::PreferAntialias);
    qApp->setFont(font);
}
