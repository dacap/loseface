// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "CreateUserPanel.h"
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>

CreateUserPanel::CreateUserPanel(QWidget* parent)
  : QWidget(parent)
{
  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);

  QLabel* titleLabel = new QLabel(tr("Create User"));
  titleLabel->setFont(QFont("Tahoma", 12, QFont::Bold));

  QFont helpFont("Tahoma", 8);
  helpFont.setItalic(true);

  QLabel* userNameLabel = new QLabel(tr("User Name:"));
  QLineEdit* userNameEdit = new QLineEdit(tr(""));
  QLabel* userNameHelp = new QLabel(tr("Used to identify the user uniquely in login mode."));
  userNameHelp->setFont(helpFont);

  QLabel* firstNameLabel = new QLabel(tr("First Name:"));
  QLineEdit* firstNameEdit = new QLineEdit(tr(""));

  QLabel* lastNameLabel = new QLabel(tr("Last Name:"));
  QLineEdit* lastNameEdit = new QLineEdit(tr(""));

  QLabel* emailLabel = new QLabel(tr("E-mail:"));
  QLineEdit* emailEdit = new QLineEdit(tr(""));

  QLabel* ssnLabel = new QLabel(tr("Social Security Number:"));
  QLineEdit* ssnEdit = new QLineEdit(tr(""));

  QGridLayout* layout = new QGridLayout();
  layout->addWidget(titleLabel, 0, 0);
  layout->addWidget(userNameLabel, 1, 0);
  layout->addWidget(userNameEdit, 1, 1);
  layout->addWidget(firstNameLabel, 2, 0);
  layout->addWidget(firstNameEdit, 2, 1);
  layout->addWidget(lastNameLabel, 3, 0);
  layout->addWidget(lastNameEdit, 3, 1);
  layout->addWidget(emailLabel, 4, 0);
  layout->addWidget(emailEdit, 4, 1);
  layout->addWidget(ssnLabel, 5, 0);
  layout->addWidget(ssnEdit, 5, 1);
  layout->setColumnStretch(1, 100);
  setLayout(layout);
}
