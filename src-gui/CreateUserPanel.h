// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef CREATEUSERPANEL_H
#define CREATEUSERPANEL_H

#include <QWidget>

class CreateUserPanel : public QWidget
{
  Q_OBJECT

public:
  CreateUserPanel(QWidget* parent = 0);

signals:
  void ok();
  void cancel();

private slots:
  void onOK();
  void onCancel();
};

#endif
