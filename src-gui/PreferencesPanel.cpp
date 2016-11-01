// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "PreferencesPanel.h"
#include <QLabel>
#include <QGridLayout>

PreferencesPanel::PreferencesPanel(QWidget* parent)
  : QWidget(parent)
{
  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);

  QLabel* titleLabel = new QLabel(tr("Preferences"));
  titleLabel->setFont(QFont("Tahoma", 12, QFont::Bold));

  QGridLayout* layout = new QGridLayout();
  layout->addWidget(titleLabel, 0, 0);
  layout->setRowStretch(1, 100);
  setLayout(layout);
}
