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

#include "xa_theme.h"
#include <QGuiApplication>
#include <QPalette>
#include <QStyleHints>


namespace
{

#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
    QPalette qt_fusionPalette_legacy(bool darkAppearance)
    {

        const QColor windowText = darkAppearance ? QColor(240, 240, 240) : Qt::black;
        const QColor backGround = darkAppearance ? QColor(50, 50, 50) : QColor(239, 239, 239);
        const QColor light = backGround.lighter(150);
        const QColor mid = (backGround.darker(130));
        const QColor midLight = mid.lighter(110);
        const QColor base = darkAppearance ? backGround.darker(140) : Qt::white;
        const QColor disabledBase(backGround);
        const QColor dark = backGround.darker(150);
        const QColor darkDisabled = QColor(209, 209, 209).darker(110);
        const QColor text = darkAppearance ? windowText : Qt::black;
        const QColor highlight = QColor(48, 140, 198);
        const QColor hightlightedText = darkAppearance ? windowText : Qt::white;
        const QColor disabledText = darkAppearance ? QColor(130, 130, 130) : QColor(190, 190, 190);
        const QColor button = backGround;
        const QColor shadow = dark.darker(135);
        const QColor disabledShadow = shadow.lighter(150);
        const QColor disabledHighlight(145, 145, 145);
        QColor placeholder = text;
        placeholder.setAlpha(128);

        QPalette fusionPalette(windowText, backGround, light, dark, mid, text, base);
        fusionPalette.setBrush(QPalette::Midlight, midLight);
        fusionPalette.setBrush(QPalette::Button, button);
        fusionPalette.setBrush(QPalette::Shadow, shadow);
        fusionPalette.setBrush(QPalette::HighlightedText, hightlightedText);

        fusionPalette.setBrush(QPalette::Disabled, QPalette::Text, disabledText);
        fusionPalette.setBrush(QPalette::Disabled, QPalette::WindowText, disabledText);
        fusionPalette.setBrush(QPalette::Disabled, QPalette::ButtonText, disabledText);
        fusionPalette.setBrush(QPalette::Disabled, QPalette::Base, disabledBase);
        fusionPalette.setBrush(QPalette::Disabled, QPalette::Dark, darkDisabled);
        fusionPalette.setBrush(QPalette::Disabled, QPalette::Shadow, disabledShadow);

        fusionPalette.setBrush(QPalette::Active, QPalette::Highlight, highlight);
        fusionPalette.setBrush(QPalette::Inactive, QPalette::Highlight, highlight);
        fusionPalette.setBrush(QPalette::Disabled, QPalette::Highlight, disabledHighlight);

        // fusionPalette.setBrush(QPalette::Active, QPalette::Accent, highlight);
        // fusionPalette.setBrush(QPalette::Inactive, QPalette::Accent, highlight);
        // fusionPalette.setBrush(QPalette::Disabled, QPalette::Accent, disabledHighlight);

        fusionPalette.setBrush(QPalette::PlaceholderText, placeholder);

        // Use a more legible light blue on dark backgrounds than the default Qt::blue.
        if (darkAppearance)
            fusionPalette.setBrush(QPalette::Link, highlight);

        return fusionPalette;
    }
#endif

}



XATheme::XATheme()
    : m_color_theme()
{
}

XATheme::~XATheme()
{
}

bool XATheme::isSystemDarkMode() const
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    const auto scheme = QGuiApplication::styleHints()->colorScheme();
    return scheme == Qt::ColorScheme::Dark;
#else
    const QPalette defaultPalette;
    const auto text = defaultPalette.color(QPalette::WindowText);
    const auto window = defaultPalette.color(QPalette::Window);
    return text.lightness() > window.lightness();
#endif // QT_VERSION
}


void XATheme::selectColorTheme(const QString& color_theme)
{
    m_color_theme = color_theme;

#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    if (m_color_theme == "dark")
        QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Dark);
    else
        QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);
#else
     qApp->setPalette(qt_fusionPalette_legacy(m_color_theme == "dark"));
#endif // QT_VERSION
}

QString XATheme::getColorTheme() const
{
    return m_color_theme;
}

QIcon XATheme::getIcon(QString icon_name) const
{
    return QIcon(QString(":/icons/images/%1/%2").arg(m_color_theme).arg(icon_name));
}