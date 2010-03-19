// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "PreferencesPanel.h"
#include <QLabel>
#include <QGridLayout>

PreferencesPanel::PreferencesPanel(QWidget* parent)
  : QWidget(parent)
{
  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);

  QLabel* titleLabel = new QLabel(tr("Preferences"));
  titleLabel->setFont(QFont("Tahoma", 12));

  QGridLayout* layout = new QGridLayout();
  layout->addWidget(titleLabel, 0, 0);
  setLayout(layout);
}
