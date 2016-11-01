// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

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
  LoginDialog(QWidget* parent = 0);
  ~LoginDialog();

  bool initCamera();

private slots:
  void login();
  void enterAdmin();

};

#endif
