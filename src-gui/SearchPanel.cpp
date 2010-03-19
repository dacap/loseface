// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "SearchPanel.h"
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>
#include <QListWidget>

SearchPanel::SearchPanel(QWidget* parent)
  : QWidget(parent)
{
  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);

  QLabel* titleLabel = new QLabel(tr("Search for registered users:"));
  titleLabel->setFont(QFont("Tahoma", 12));

  QLineEdit* nameLabel = new QLineEdit(tr(""));

  QPushButton* searchButton = new QPushButton(tr("Search"));
  connect(searchButton, SIGNAL(clicked()), this, SLOT(onSearchClicked()));

  QPushButton* newUser = new QPushButton(tr("New User"));
  connect(newUser, SIGNAL(clicked()), this, SLOT(onNewUserClicked()));

  m_usersList = new QListWidget();

  QGridLayout* layout = new QGridLayout();
  layout->addWidget(titleLabel, 0, 0);
  layout->addWidget(nameLabel, 1, 0);
  layout->addWidget(searchButton, 1, 1);
  layout->addWidget(newUser, 1, 2);
  layout->addWidget(m_usersList, 2, 0, 1, 3);
  layout->setColumnStretch(0, 100);
  layout->setColumnStretch(3, 100);
  layout->setRowStretch(2, 100);

  setLayout(layout);
}

void SearchPanel::onSearchClicked()
{
  m_usersList->addItem(tr("Item 1"));
  m_usersList->addItem(tr("Item 2"));
  m_usersList->addItem(tr("Item 3"));
}

void SearchPanel::onNewUserClicked()
{
  emit createUser();
}
