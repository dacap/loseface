// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "captu/VideoCapture.h"
#include <vector>
#include <tchar.h>
#include <windows.h>
#include <vfw.h>

using namespace captu;
using namespace cimg_library;

// MinGW no tiene definidas estas constantes
#ifndef WM_CAP_DRIVER_CONNECT
  #define WM_CAP_DRIVER_CONNECT		(WM_USER+10)
  #define WM_CAP_DRIVER_DISCONNECT	(WM_USER+11)
  #define WM_CAP_DRIVER_GET_CAPS	(WM_USER+14)
  #define WM_CAP_EDIT_COPY		(WM_USER+30)
  #define WM_CAP_SET_PREVIEW		(WM_USER+50)
  #define WM_CAP_SET_PREVIEWRATE	(WM_USER+52)
  #define WM_CAP_SET_SCALE		(WM_USER+53)
  #define WM_CAP_GET_STATUS		(WM_USER+54)
  #define WM_CAP_GRAB_FRAME		(WM_USER+60)
  #define WM_CAP_GRAB_FRAME_NOSTOP	(WM_USER+61)
  #define WM_CAP_SEQUENCE_NOFILE	(WM_USER+63)

  #define WM_CAP_DLG_VIDEOFORMAT	(WM_USER+41)
  #define WM_CAP_DLG_VIDEOSOURCE	(WM_USER+42)
  #define WM_CAP_DLG_VIDEODISPLAY	(WM_USER+43)
  #define WM_CAP_GET_VIDEOFORMAT	(WM_USER+44)
  #define WM_CAP_SET_VIDEOFORMAT	(WM_USER+45)
  #define WM_CAP_DLG_VIDEOCOMPRESSION	(WM_USER+46)

  typedef struct {
    UINT uiImageWidth;
    UINT uiImageHeight;
    BOOL fLiveWindow;
    BOOL fOverlayWindow;
    BOOL fScale;
    POINT ptScroll;
    BOOL fUsingDefaultPalette;
    BOOL fAudioHardware;
    BOOL fCapFileExists;
    DWORD dwCurrentVideoFrame;
    DWORD dwCurrentVideoFramesDropped;
    DWORD dwCurrentWaveSamples;
    DWORD dwCurrentTimeElapsedMS;
    HPALETTE hPalCurrent;
    BOOL fCapturingNow;
    DWORD dwReturn;
    UINT wNumVideoAllocated;
    UINT wNumAudioAllocated;
  } CAPSTATUS;
#endif

class VideoCaptureWin32 : public captu::VideoCapture
{
  int m_driverIndex;
  HWND m_hwndPreview;
  std::vector<TCHAR*> m_driversList;
  CImg<unsigned char> m_img;

public:
  VideoCaptureWin32();
  virtual ~VideoCaptureWin32();

  virtual bool initDriver();
  virtual void exitDriver();

  virtual bool isPreviewWindowAvailable();
  virtual bool createPreviewWindow(void* parentWindowHandle);
  virtual void destroyPreviewWindow();
  virtual void startPreviewInWindow();
  virtual void stopPreviewInWindow();

  virtual void grabFrame();
  virtual bool getImage(CImg<unsigned char>& img);

  virtual bool hasFormatDialog() { return true; }
  virtual bool hasParamsDialog() { return true; }

  virtual void showFormatDialog();
  virtual void showParamsDialog();

  virtual VideoControls* getVideoControls()
  {
    return NULL;
  }
};

VideoCaptureWin32::VideoCaptureWin32()
{
  m_driverIndex = 0;
  m_hwndPreview = NULL;
}

VideoCaptureWin32::~VideoCaptureWin32()
{
}

bool VideoCaptureWin32::initDriver()
{
  if (::SendMessage(m_hwndPreview, WM_CAP_DRIVER_CONNECT,
		    m_driverIndex, 0L)) {
    // setup the format to 320x240 size
    LPBITMAPINFO lpbi;
    DWORD dwSize = ::SendMessage(m_hwndPreview,
				 WM_CAP_GET_VIDEOFORMAT,
				 0, 0);

    lpbi = reinterpret_cast<LPBITMAPINFO>(::GlobalAlloc(GHND, dwSize));
    ::SendMessage(m_hwndPreview,
		  WM_CAP_GET_VIDEOFORMAT,
		  dwSize, reinterpret_cast<LPARAM>(lpbi));

    lpbi->bmiHeader.biWidth = 320;
    lpbi->bmiHeader.biHeight = 240;
    ::SendMessage(m_hwndPreview,
		  WM_CAP_SET_VIDEOFORMAT,
		  dwSize, reinterpret_cast<LPARAM>(lpbi));
    ::GlobalFree(lpbi);

    // get the status (to get the real size of the preview HWND)
    CAPSTATUS cs;
    if (::SendMessage(m_hwndPreview, WM_CAP_GET_STATUS,
		      sizeof(CAPSTATUS), reinterpret_cast<LPARAM>(&cs))) {
      // set the size of the preview window to the image size
      ::MoveWindow(m_hwndPreview, 0, 0, cs.uiImageWidth, cs.uiImageHeight, TRUE);
      return true;
    }
  }
  else {
    // TODO error
  }
  return false;
}

void VideoCaptureWin32::exitDriver()
{
  ::SendMessage(m_hwndPreview, WM_CAP_DRIVER_DISCONNECT, 0, 0L);
}

bool VideoCaptureWin32::isPreviewWindowAvailable()
{
  return true;
}

bool VideoCaptureWin32::createPreviewWindow(void* parentWindowHandle)
{
  // Here we create the HWND to show the camera preview
  m_hwndPreview =
    capCreateCaptureWindow(_T("WebCam"),
			   WS_VISIBLE | WS_CHILD,
			   0, 0, 0, 0,
			   reinterpret_cast<HWND>(parentWindowHandle), 0);

#if 0
  // Get the list of available camera drivers
  for (int i=0; i<10; i++) { // driver index (from 0 to 9 according to MSDN)
    TCHAR name[256];
    TCHAR ver[256];
    if (capGetDriverDescription(i, name, sizeof(name), ver, sizeof(ver))) {
      // TCHAR buf[512];
      // _stprintf(buf, "%s %s", name, ver);
      // driversList.push_back(_tcsdup(buf));
      printf("%s %s\n", name, ver);
    }
    else
      break;
  }
#endif
  
  return m_hwndPreview ? true: false;
}

void VideoCaptureWin32::destroyPreviewWindow()
{
  ::DestroyWindow(m_hwndPreview);
}

void VideoCaptureWin32::startPreviewInWindow()
{
  ::SendMessage(m_hwndPreview, WM_CAP_SET_PREVIEWRATE, 10, 0L);
  ::SendMessage(m_hwndPreview, WM_CAP_SET_PREVIEW, TRUE, 0L);
}

void VideoCaptureWin32::stopPreviewInWindow()
{
  ::SendMessage(m_hwndPreview, WM_CAP_SET_PREVIEW, FALSE, 0L);
}

void VideoCaptureWin32::grabFrame()
{
  // Do nothing
}

bool VideoCaptureWin32::getImage(CImg<unsigned char>& img)
{
  bool ret = false;

  ::SendMessage(m_hwndPreview, WM_CAP_GRAB_FRAME_NOSTOP, 0, 0L);
  ::SendMessage(m_hwndPreview, WM_CAP_EDIT_COPY, 0, 0L);

  ::OpenClipboard(m_hwndPreview); 
  if (HBITMAP hbmp = reinterpret_cast<HBITMAP>(::GetClipboardData(CF_BITMAP))) {
    HDC hdc = ::GetDC(m_hwndPreview);

    BITMAPINFO bi;
    memset(&bi, 0, sizeof(BITMAPINFO));
    bi.bmiHeader.biSize = sizeof(BITMAPINFO);
    bi.bmiHeader.biPlanes = 1;
    ::GetDIBits(hdc, hbmp, 0, 0, NULL, &bi, DIB_RGB_COLORS);

    int width = bi.bmiHeader.biWidth;
    int height = bi.bmiHeader.biHeight;

    if (width > 0 && height > 0 && width <= 1024 && height <= 768) {
      unsigned char* data = new unsigned char[width*height*3];
      bi.bmiHeader.biBitCount = 24;
      bi.bmiHeader.biCompression = BI_RGB;
      ::GetDIBits(hdc, hbmp, 0, height, data, &bi, DIB_RGB_COLORS);

      // assign de CImg ya chequea que el tamaño de la imagen sea el mismo
      // (así no tiene que hacer un delete[] y new cada vez)
      img.assign(width, height, 1, 3, 0);

      unsigned char* ptr = data;
      for (int y=0; y<height; ++y) {
	for (int x=0; x<width; ++x) {
	  int b = *(ptr++);
	  int g = *(ptr++);
	  int r = *(ptr++);
	  img(x, height-1-y, 0, 0) = r;
	  img(x, height-1-y, 0, 1) = g;
	  img(x, height-1-y, 0, 2) = b;
	}
      }

      delete data;
      ret = true;
    }

    ::ReleaseDC(m_hwndPreview, hdc);
  }
  ::CloseClipboard(); 

  return ret;
}

void VideoCaptureWin32::showFormatDialog()
{
  ::SendMessage(m_hwndPreview, WM_CAP_DLG_VIDEOFORMAT, 0, 0L);
  
  CAPSTATUS cs;
  if (::SendMessage(m_hwndPreview, WM_CAP_GET_STATUS,
		    sizeof(CAPSTATUS), reinterpret_cast<LPARAM>(&cs))) {
    ::MoveWindow(m_hwndPreview, 0, 0, cs.uiImageWidth, cs.uiImageHeight, TRUE);
  }
}

void VideoCaptureWin32::showParamsDialog()
{
  ::SendMessage(m_hwndPreview, WM_CAP_DLG_VIDEOSOURCE, 0, 0L);
  
  CAPSTATUS cs;
  if (::SendMessage(m_hwndPreview, WM_CAP_GET_STATUS,
		    sizeof(CAPSTATUS), reinterpret_cast<LPARAM>(&cs))) {
    ::MoveWindow(m_hwndPreview, 0, 0, cs.uiImageWidth, cs.uiImageHeight, TRUE);
  }
}

VideoCapture* VideoCapture::create()
{
  return new VideoCaptureWin32();
}
