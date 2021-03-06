// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef WEBCAMWIDGET_H
#define WEBCAMWIDGET_H

#include <QWidget>
#include "captu/VideoCapture.h"

class QTimer;

class WebCamWidget : public QWidget
{
  Q_OBJECT

  bool m_driverInitialized;
  QTimer* m_timer;
  captu::VideoCapture* m_video;
  cimg_library::CImg<unsigned char> m_lastImage;

public:
  WebCamWidget(QWidget* parent = 0);
  ~WebCamWidget();

  bool startPreview();
  void stopPreview();
  // cimg_library::CImg<unsigned char>* snapshot();

  void showFormatDialog();
  void showParamsDialog();

  QSize sizeHint() const;

protected:
  void paintEvent(QPaintEvent* event);

private:
  bool initDriver();

private slots:
  void timerTick();

};

#endif
