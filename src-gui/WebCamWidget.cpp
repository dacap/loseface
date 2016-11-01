// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "WebCamWidget.h"
#include "captu/VideoCapture.h"
#include <QtGui>

#define TIMER_FREQ 100

using namespace captu;
using namespace cimg_library;

WebCamWidget::WebCamWidget(QWidget* parent)
  : QWidget(parent)
{
  m_timer = new QTimer(this);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(timerTick()));
  // m_timer->setInterval(TIMER_FREQ)
  // m_timer->start(TIMER_FREQ);

  m_driverInitialized = false;

  m_video = NULL;
  setBackgroundRole(QPalette::NoRole);
}

WebCamWidget::~WebCamWidget()
{
  if (m_video) {
    if (m_driverInitialized)
      m_video->exitDriver();

    if (m_video->isPreviewWindowAvailable())
      m_video->destroyPreviewWindow();

    delete m_video;
  }
}

QSize WebCamWidget::sizeHint() const
{
  return QSize(320, 240);	// TODO
}

bool WebCamWidget::startPreview()
{
  if (!initDriver())
    return false;

  if (m_video->isPreviewWindowAvailable())
    m_video->startPreviewInWindow();
  else
    m_timer->start(TIMER_FREQ);

  return true;
}

void WebCamWidget::stopPreview()
{
  if (!m_driverInitialized)
    return;

  if (m_video->isPreviewWindowAvailable())
    m_video->stopPreviewInWindow();
  else
    m_timer->stop();
}

// cimg_library::CImg<unsigned char>* WebCamWidget::snapshot()
// {
//   if (!initDriver())
//     return false;

//   if (m_video->getImage(m_lastImage))
//     return new cimg_library::CImg<unsigned char>(m_lastImage);
//   else
//     return NULL;
// }

void WebCamWidget::showFormatDialog()
{
  if (m_video->hasFormatDialog()) {
    m_video->showFormatDialog();
  }
  else {
    // showMsg(_T("No implementado"));
  }
}

void WebCamWidget::showParamsDialog()
{
  if (m_video->hasParamsDialog()) {
    m_video->showParamsDialog();
  }
  else if (VideoControls* controls = m_video->getVideoControls()) {
    // TODO
    // CaptureParamsDialog dlg(this, tr("Capture parameters"),
    // 			    controls);
    // dlg.exec();
  }
}

void WebCamWidget::paintEvent(QPaintEvent* event)
{
  QPainter painter(this);

  if (!m_driverInitialized) {
    painter.fillRect(event->rect(), QBrush(QColor(0, 0, 0)));
    return;
  }

  if (!m_video->isPreviewWindowAvailable()) {
    m_video->getImage(m_lastImage);

    // wxImage image;
    // if (CImg2wxImage(img, image)) {
    //   wxBitmap bitmap(image);
    //   if (bitmap.IsOk())
    // 	dc.DrawBitmap(bitmap, 0, 0, false);
    // }
  }
}

bool WebCamWidget::initDriver()
{
  if (!m_driverInitialized) {
    // showMsg(_T("Initializing camera driver"));

    m_video = captu::VideoCapture::create();
    if (m_video->isPreviewWindowAvailable()) {
      bool result = m_video->createPreviewWindow(reinterpret_cast<void*>(winId()));

      // TODO handle "result" = false, show an error message
    }

    if (m_video->initDriver()) {
      // showMsg(_T("Camera initialized"));
      m_driverInitialized = true;
    }
    // else
    //   showMsg(_T("Error iniciando driver de la cámara"));
  }
  return m_driverInitialized;
}

void WebCamWidget::timerTick()
{
  m_video->grabFrame();

  update();

  m_timer->start(TIMER_FREQ);
}
