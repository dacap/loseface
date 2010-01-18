// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "WebCamWidget.h"

WebCamWidget::WebCamWidget(QWidget* parent)
  : QWidget(parent)
{
}

QSize WebCamWidget::sizeHint() const
{
  return QSize(320, 240);
}

