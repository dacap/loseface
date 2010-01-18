// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef WEBCAMWIDGET_H
#define WEBCAMWIDGET_H

#include <QWidget>

class WebCamWidget : public QWidget
{
  Q_OBJECT

public:
  WebCamWidget(QWidget* parent = 0);

  QSize sizeHint() const;
  
};

#endif
