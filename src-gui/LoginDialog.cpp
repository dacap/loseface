// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "LoginDialog.h"
#include "WebCamWidget.h"
#include <QtGui>

LoginDialog::LoginDialog()
{
  // Create widgets

  m_webCam = new WebCamWidget();
  QPushButton* loginButton = new QPushButton(tr("Login"));
  QPushButton* adminButton = new QPushButton(tr("Admin"));

  // Create layouts

  QHBoxLayout* midLayout = new QHBoxLayout();
  midLayout->addWidget(new QLabel("User Name:"), 0);
  midLayout->addWidget(new QLineEdit(), 1);

  QHBoxLayout* bottomLayout2 = new QHBoxLayout;
  bottomLayout2->addStretch();
  bottomLayout2->addWidget(adminButton);

  QHBoxLayout* bottomLayout = new QHBoxLayout;
  bottomLayout->addStretch();
  bottomLayout->addWidget(loginButton);
  bottomLayout->addLayout(bottomLayout2);

  QVBoxLayout* mainLayout = new QVBoxLayout;
  mainLayout->addWidget(m_webCam, 2);
  mainLayout->addLayout(midLayout, 0);
  mainLayout->addLayout(bottomLayout, 0);

  // mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  setLayout(mainLayout);

  // Connect signals

  connect(loginButton, SIGNAL(clicked()), this, SLOT(login()));
  connect(adminButton, SIGNAL(clicked()), this, SLOT(enterAdmin()));
}

void LoginDialog::login()
{
  done(LOGIN_RESULT);
}

void LoginDialog::enterAdmin()
{
  done(ENTER_ADMIN_RESULT);
}
