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

#include "xa_find_dialog.h"


FindDialog::FindDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Find");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    m_searchLineEdit = new QLineEdit(this);
    mainLayout->addWidget(m_searchLineEdit);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_findButton = new QPushButton("Find", this);
    m_findNextButton = new QPushButton("Find Next", this);
    m_cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addWidget(m_findButton);
    buttonLayout->addWidget(m_findNextButton);
    buttonLayout->addWidget(m_cancelButton);

    mainLayout->addLayout(buttonLayout);

    connect(m_findButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_findNextButton, &QPushButton::clicked, this, &FindDialog::findNext);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

QString FindDialog::getSearchTerm() const
{
    return m_searchLineEdit->text();
}
