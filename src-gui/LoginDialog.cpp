// Lose Face - An open source face recognition project
// Copyright (C) 2008-2009 David Capello
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the authors nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

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
