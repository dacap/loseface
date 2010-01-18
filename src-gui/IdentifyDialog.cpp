// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "IdentifyDialog.h"
#include "WebCamWidget.h"
#include <QtGui>

IdentifyDialog::IdentifyDialog()
{
  m_webCam = new WebCamWidget();

  QPushButton* identifyButton = new QPushButton(tr("Identify"));
  QPushButton* adminButton = new QPushButton(tr("Admin"));

  QHBoxLayout* bottomLayout2 = new QHBoxLayout;
  bottomLayout2->addStretch();
  bottomLayout2->addWidget(adminButton);

  QHBoxLayout* bottomLayout = new QHBoxLayout;
  bottomLayout->addStretch();
  bottomLayout->addWidget(identifyButton);
  bottomLayout->addLayout(bottomLayout2);

  QVBoxLayout* mainLayout = new QVBoxLayout;
  mainLayout->addWidget(m_webCam, 2);
  mainLayout->addLayout(bottomLayout);

  // mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  setLayout(mainLayout);

  // connect(loginButton, SIGNAL(clicked()), this, SLOT(login()));
  // connect(adminButton, SIGNAL(clicked()), this, SLOT(enterAdmin()));
}
