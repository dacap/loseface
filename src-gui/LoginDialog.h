// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

class WebCamWidget;

class LoginDialog : public QDialog
{
  Q_OBJECT

  WebCamWidget* m_webCam;

public:
  enum {
    LOGIN_RESULT = 1,
    ENTER_ADMIN_RESULT = 2,
  };
  LoginDialog();

private slots:
  void login();
  void enterAdmin();

};

#endif
