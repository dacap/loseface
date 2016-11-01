// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "CreateUserPanel.h"
#include <QtGui>

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

  QPushButton* okButton = new QPushButton(tr("&OK"));
  QPushButton* cancelButton = new QPushButton(tr("&Cancel"));
  connect(okButton, SIGNAL(clicked()), this, SLOT(onOK()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(onCancel()));

  QHBoxLayout* buttonslayout = new QHBoxLayout();
  buttonslayout->addWidget(okButton);
  buttonslayout->addWidget(cancelButton);

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
  layout->addLayout(buttonslayout, 6, 1);
  layout->setColumnStretch(1, 100);
  layout->setColumnStretch(2, 100);
  layout->setRowStretch(7, 100);
  setLayout(layout);
}

void CreateUserPanel::onOK()
{
  emit ok();
}

void CreateUserPanel::onCancel()
{
  emit cancel();
}
