// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef IDENTIFYDIALOG_H
#define IDENTIFYDIALOG_H

#include <QDialog>

class WebCamWidget;

class IdentifyDialog : public QDialog
{
  Q_OBJECT

  WebCamWidget* m_webCam;

public:
  IdentifyDialog();

};

#endif
