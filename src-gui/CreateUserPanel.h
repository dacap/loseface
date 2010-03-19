// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

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
