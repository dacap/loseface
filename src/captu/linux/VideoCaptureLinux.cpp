// Lose Face - An open source face recognition project
// Copyright (C) 2008-2009 David Capello
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the authors nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

// Based on code of Michel Xhaard

#include "captu/VideoCapture.h"
#include "captu/linux/spcav4l.h"
#include "captu/linux/spcadecoder.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;
using namespace captu;
using namespace cimg_library;

//////////////////////////////////////////////////////////////////////

/// Almacena y recupera la configuración de captura de video en Linux.
/// 
class Configuration
{
  string cfgFileName;
  string devName;
  string format;
  string resolution;
  bool flipRB;
  unsigned char brightness;
  unsigned char contrast;
  unsigned char colors;
  int width;
  int height;

public:
  Configuration();
  virtual ~Configuration();

  // getters y setters
  string getDevName() const { return devName; }
  string getFormat() const { return format; }
  string getResolution() const { return resolution; }
  bool getFlipRB() const { return flipRB; }
  unsigned char getBrightness() const { return brightness; }
  unsigned char getContrast() const { return contrast; }
  unsigned char getColors() const { return colors; }
  int getWidth() const { return width; }
  int getHeight() const { return height; }

  void setDevName(const string& v) { devName = v; }
  void setFormat(const string& v) { format = v; }
  void setResolution(const string& v) { resolution = v; }
  void setFlipRB(bool v) { flipRB = v; }
  void setBrightness(unsigned char v) { brightness = v; }
  void setContrast(unsigned char v) { contrast = v; }
  void setColors(unsigned char v) { colors = v; }
  void setWidth(int v) { width = v; }
  void setHeight(int v) { height = v; }

private:
  bool save();
  bool load();
};

//////////////////////////////////////////////////////////////////////

class VideoControlsLinux : public captu::VideoControls
{
  Configuration& cfg;
  vdIn* vd;

public:
  VideoControlsLinux(Configuration& cfg, vdIn* vd);
  virtual ~VideoControlsLinux();

  virtual void getAvailableFormats(vector<string>& lst);
  virtual void getAvailableResolutions(vector<string>& lst);

  virtual void setFormat(const string& fmt);
  virtual void setResolution(const string& res);

  virtual unsigned char getBrightness();
  virtual unsigned char getContrast();
  virtual unsigned char getColors();

  virtual void setBrightness(unsigned char value);
  virtual void setContrast(unsigned char value);
  virtual void setColors(unsigned char value);

  virtual bool getFlipRB();
  virtual void setFlipRB(bool flip);
};

//////////////////////////////////////////////////////////////////////

class VideoCaptureLinux : public captu::VideoCapture
{
  Configuration cfg;
  vdIn* vd;
  spca5xx_frame* frm;
  VideoControlsLinux* controls;

public:
  VideoCaptureLinux();
  virtual ~VideoCaptureLinux();

  virtual bool initDriver();
  virtual void exitDriver();

  virtual bool isPreviewWindowAvailable() {
    return false;
  }
  virtual bool createPreviewWindow(void* parentWindowHandle) {
    return false;
  }
  virtual void destroyPreviewWindow() { }
  virtual void startPreviewInWindow() { }
  virtual void stopPreviewInWindow() { }

  virtual void grabFrame();
  virtual bool getImage(CImg<unsigned char>& img);

  virtual bool hasFormatDialog() { return false; }
  virtual bool hasParamsDialog() { return false; }

  virtual void showFormatDialog() { }
  virtual void showParamsDialog() { }

  virtual VideoControls* getVideoControls()
  {
    return controls;
  }
};

static void YUV420toRGB(unsigned char *src,
			unsigned char *dst,
			int width, int height,
			int flipUV, int ColSpace);

//////////////////////////////////////////////////////////////////////
// Implementación de Configuration

Configuration::Configuration()
{
  cfgFileName = getenv("HOME");
  cfgFileName += "/.loseface-v4l";

  // valores por defecto
  devName = "/dev/video0";
  format = "";
  resolution = "";
  flipRB = true;
  brightness = 100;
  contrast = 100;
  colors = 100;
  width = 320;
  height = 240;

  load();
}

Configuration::~Configuration()
{
  save();
}

bool Configuration::save()
{
  FILE* file = fopen(cfgFileName.c_str(), "w");
  if (!file)
    return false;

  fprintf(file, "devName=%s\n", devName.c_str());
  fprintf(file, "format=%s\n", format.c_str());
  fprintf(file, "resolution=%s\n", resolution.c_str());
  fprintf(file, "flipRB=%d\n", flipRB ? 1: 0);
  fprintf(file, "brightness=%d\n", brightness);
  fprintf(file, "contrast=%d\n", contrast);
  fprintf(file, "colors=%d\n", colors);
  fprintf(file, "width=%d\n", width);
  fprintf(file, "height=%d\n", height);
  fclose(file);
  return true;
}

bool Configuration::load()
{
  FILE* file = fopen(cfgFileName.c_str(), "r");
  if (!file)
    return false;

  char buf[256];
  while (fgets(buf, sizeof(buf), file)) {
    string varName;
    string varValue;
    char* ini = strstr(buf, "="); // inicio del valor
    char* end = strstr(buf, "\n"); // fin del valor

    if (ini) {
      if (!end)
	end = buf+strlen(buf);

      copy(buf, ini, back_inserter(varName));
      copy(ini+1, end, back_inserter(varValue));

      int intValue = strtol(varValue.c_str(), NULL, 10);

      if (varName == "devName")	        setDevName(varValue);
      else if (varName == "format")	setFormat(varValue);
      else if (varName == "resolution")	setResolution(varValue);
      else if (varName == "flipRB")	setFlipRB(intValue != 0);
      else if (varName == "brightness")	setBrightness(intValue);
      else if (varName == "contrast")	setContrast(intValue);
      else if (varName == "colors")	setColors(intValue);
      else if (varName == "width")	setWidth(intValue);
      else if (varName == "height")	setHeight(intValue);
    }
  }
  fclose(file);
  return true;
}

//////////////////////////////////////////////////////////////////////
// Implementación de VideoCaptureLinux

VideoCaptureLinux::VideoCaptureLinux()
{
  vd = NULL;
  frm = NULL;
  controls = NULL;
}

VideoCaptureLinux::~VideoCaptureLinux()
{
}

bool VideoCaptureLinux::initDriver()
{
  vd = new vdIn;
  memset(vd, 0, sizeof(vdIn));

  vd->videodevice = new char[16];
  vd->cameraname = new char[32];
  vd->bridge = new char[9];
  vd->grabMethod = 1;
  vd->formatIn = VIDEO_PALETTE_RGB24;

  strcpy(vd->videodevice, cfg.getDevName().c_str());

  if (init_v4l(vd) != 0)
    return false;

  vd->hdrwidth = cfg.getWidth();
  vd->hdrheight = cfg.getHeight();
  if (changeSize(vd) != 0) {
    close_v4l(vd);
    delete vd;
    return false;
  }

  frm = new spca5xx_frame;
  memset(frm, 0, sizeof(spca5xx_frame));

  frm->cameratype = vd->cameratype;
  frm->width = vd->hdrwidth;
  frm->height = vd->hdrheight;
  frm->format = VIDEO_PALETTE_RGB24;

  unsigned size = frm->width * frm->height * 3;
  frm->data = new unsigned char[size];

  if (vd->cameratype == JPEG) {
    init_jpeg_decoder();

    frm->pictsetting.change = 01;
    frm->pictsetting.gamma = 3;
    frm->pictsetting.OffRed = -16;
    frm->pictsetting.OffBlue = -16;
    frm->pictsetting.OffGreen = -16;
    frm->pictsetting.GRed = 256;
    frm->pictsetting.GBlue = 256;
    frm->pictsetting.GGreen = 256;
    frm->pictsetting.force_rgb = 0;
    frm->cropx1 = frm->cropx2 = frm->cropy1 = frm->cropy2 = 0;

    frm->method = 0;
    vd->formatIn = VIDEO_PALETTE_RAW_JPEG;
  }

  controls = new VideoControlsLinux(cfg, vd);
  return true;
}

void VideoCaptureLinux::exitDriver()
{
  close_v4l(vd);
  if (frm) {
    delete frm->data;
    delete frm;
    frm = NULL;
  }
  delete controls;
  delete vd;
  vd = NULL;
}

void VideoCaptureLinux::grabFrame()
{
  SpcaGrab(vd);

  if ((frm->hdrwidth != vd->hdrwidth) || (frm->depth != vd->bppIn)) {
    frm->hdrwidth = frm->width = vd->hdrwidth;
    frm->hdrheight = frm->height = vd->hdrheight;
    frm->depth = vd->bppIn;
    frm->format = VIDEO_PALETTE_RGB24;

    delete frm->data;

    unsigned size = frm->width * frm->height * 3;
    frm->data = new unsigned char[size];
  }

  switch (vd->formatIn) {

    case VIDEO_PALETTE_RAW_JPEG:
      spca50x_outpicture(frm);
      break;

    case VIDEO_PALETTE_YUV420P:
      YUV420toRGB(vd->pixTmp,
		  frm->data, frm->width, frm->height, 0, 0);
      break;

    case VIDEO_PALETTE_RGB565: {
      __u16* src = reinterpret_cast<__u16*>(vd->pixTmp);
      __u8* dst = frm->data;
      for (int y=0; y<frm->height; ++y) {
	for (int x=0; x<frm->width; ++x, ++src) {
	  *(dst++) = (*src & 0xf800) >> 8; // R
	  *(dst++) = (*src & 0x07e0) >> 3; // G
	  *(dst++) = (*src & 0x001f) << 3; // B
	}
      }
      break;
    }

    case VIDEO_PALETTE_RGB24: {
#if 0
      __u8* src = vd->pixTmp;
      __u8* dst = frm->data;
      for (int y=0; y<frm->height; ++y) {
	for (int x=0; x<frm->width; ++x) {
	  *(dst++) = *(src++);
	  *(dst++) = *(src++);
	  *(dst++) = *(src++);
	}
      }
#else
      memcpy(frm->data, vd->pixTmp, frm->width*frm->height*3);
#endif
      break;
    }

    case VIDEO_PALETTE_RGB32: {
      __u32* src = reinterpret_cast<__u32*>(vd->pixTmp);
      __u8* dst = frm->data;
      for (int y=0; y<frm->height; ++y) {
	for (int x=0; x<frm->width; ++x, ++src) {
	  *(dst++) = (*src & 0x00ff0000) >> 16;	// R
	  *(dst++) = (*src & 0x0000ff00) >> 8;  // G
	  *(dst++) = (*src & 0x000000ff);       // B
	}
      }
      break;
    }

    default:
      break;
  }
}

bool VideoCaptureLinux::getImage(CImg<unsigned char>& img)
{
  // assign de CImg ya chequea que el tamaño de la imagen sea el mismo
  // (así no tiene que hacer un delete[] y new cada vez)
  img.assign(frm->width, frm->height, 1, 3, 0);

  // 'data' is in VIDEO_PALETTE_RGB24 format
  unsigned char* ptr = frm->data;
  for (int y=0; y<frm->height; ++y) {
    for (int x=0; x<frm->width; ++x) {
      img(x, y, 0, 0) = *(ptr++); // R
      img(x, y, 0, 1) = *(ptr++); // G
      img(x, y, 0, 2) = *(ptr++); // B
    }
  }

  return true;
}

VideoCapture* VideoCapture::create()
{
  return new VideoCaptureLinux();
}

//////////////////////////////////////////////////////////////////////
// Implementación de VideoControlsLinux

VideoControlsLinux::VideoControlsLinux(Configuration& cfg, vdIn* vd)
  : cfg(cfg)
  , vd(vd)
{
  vd->flipUV = cfg.getFlipRB();

  setPalette(vd);

  SpcaSetBrightness(vd, cfg.getBrightness());
  SpcaSetContrast(vd, cfg.getContrast());
  SpcaSetColors(vd, cfg.getColors());
}

VideoControlsLinux::~VideoControlsLinux()
{
}

void VideoControlsLinux::getAvailableFormats(vector<string>& lst)
{
  // TODO
}

void VideoControlsLinux::getAvailableResolutions(vector<string>& lst)
{
  // TODO
}

void VideoControlsLinux::setFormat(const string& fmt)
{
  // TODO
}

void VideoControlsLinux::setResolution(const string& res)
{
  // TODO
}

unsigned char VideoControlsLinux::getBrightness()
{
  return cfg.getBrightness();
}

unsigned char VideoControlsLinux::getContrast()
{
  return cfg.getContrast();
}

unsigned char VideoControlsLinux::getColors()
{
  return cfg.getColors();
}

void VideoControlsLinux::setBrightness(unsigned char value)
{
  cfg.setBrightness(value);
  SpcaSetBrightness(vd, value);
}

void VideoControlsLinux::setContrast(unsigned char value)
{
  cfg.setContrast(value);
  SpcaSetContrast(vd, value);
}

void VideoControlsLinux::setColors(unsigned char value)
{
  cfg.setColors(value);
  SpcaSetColors(vd, value);
}

bool VideoControlsLinux::getFlipRB()
{
  return vd->flipUV == 1;
}

void VideoControlsLinux::setFlipRB(bool flip)
{
  cfg.setFlipRB(flip);
  vd->flipUV = flip ? 1: 0;
}

//////////////////////////////////////////////////////////////////////
// Funciones auxiliares

#define CLIP(color) (unsigned char)((color>0xFF)?0xff:((color<0)?0:color))

static void YUV420toRGB(unsigned char *src,
			unsigned char *dst,
			int width, int height,
			int flipUV, int ColSpace)
{
  unsigned char *Y;
  unsigned char *V;
  unsigned char *U;
  int y1, y2, u, v;
  int v1, v2, u1, u2;
  unsigned char *pty1, *pty2;
  int i, j;
  unsigned char *RGB1, *RGB2;
  int r, g, b;

  //Initialization
  Y = src;
  V = Y + width * height;
  U = Y + width * height + width * height / 4;

  pty1 = Y;
  pty2 = pty1 + width;
  RGB1 = dst;
  RGB2 = RGB1 + 3 * width;
  for (j = 0; j < height; j += 2) {
    for (i = 0; i < width; i += 2) {
      if (flipUV) {
	u = (*V++) - 128;
	v = (*U++) - 128;
      }
      else {
	v = (*V++) - 128;
	u = (*U++) - 128;
      }
      switch (ColSpace) {
	// M$ color space
	case 0: {
	  v1 = ((v << 10) + (v << 9) + (v << 6) + (v << 5)) >> 10;	// 1.593
	  u1 = ((u << 8) + (u << 7) + (u << 4)) >> 10;	//         0.390
	  v2 = ((v << 9) + (v << 4)) >> 10;	//                0.515
	  u2 = ((u << 11) + (u << 4)) >> 10;	//               2.015
	  break;
	}
	  // PAL specific
	case 1: {
	  v1 = ((v << 10) + (v << 7) + (v << 4)) >> 10;	//      1.1406
	  u1 = ((u << 8) + (u << 7) + (u << 4) + (u << 3)) >> 10;	// 0.3984
	  v2 = ((v << 9) + (v << 6) + (v << 4) + (v << 1)) >> 10;	// 0.5800
	  u2 = ((u << 11) + (u << 5)) >> 10;	//              2.0312
	  break;
	}
	  // V4l2
	case 2: {
	  v1 = ((v << 10) + (v << 8) + (v << 7) + (v << 5)) >> 10;	//       1.406
	  u1 = ((u << 8) + (u << 6) + (u << 5)) >> 10;	//                0.343
	  v2 = ((v << 9) + (v << 7) + (v << 6) + (v << 5)) >> 10;	//        0.718
	  u2 = ((u << 10) + (u << 9) + (u << 8) + (u << 4) + (u << 3)) >> 10;	// 1.773
	  break;
	}
	case 3: {
	  v1 = u1 = v2 = u2 = 0;
	  break;
	}
	default:
	  break;
      }
      //up-left

      y1 = (*pty1++);
      if (y1 > 0) {
	r = y1 + (v1);
	g = y1 - (u1) - (v2);
	b = y1 + (u2);

	r = CLIP (r);
	g = CLIP (g);
	b = CLIP (b);
      }
      else {
	r = g = b = 0;
      }
      *RGB1++ = r;
      *RGB1++ = g;
      *RGB1++ = b;

      //down-left

      y2 = (*pty2++);
      if (y2 > 0) {
	r = y2 + (v1);
	g = y2 - (u1) - (v2);
	b = y2 + (u2);

	r = CLIP (r);
	g = CLIP (g);
	b = CLIP (b);
      }
      else {
	r = b = g = 0;
      }
      *RGB2++ = r;
      *RGB2++ = g;
      *RGB2++ = b;

      //up-right
      y1 = (*pty1++);

      if (y1 > 0) {
	r = y1 + (v1);
	g = y1 - (u1) - (v2);
	b = y1 + (u2);

	r = CLIP (r);
	g = CLIP (g);
	b = CLIP (b);
      }
      else {
	r = g = b = 0;
      }

      *RGB1++ = r;
      *RGB1++ = g;
      *RGB1++ = b;

      //down-right
      y2 = (*pty2++);
      if (y2 > 0) {
	r = y2 + (v1);
	g = y2 - (u1) - (v2);
	b = y2 + (u2);

	r = CLIP (r);
	g = CLIP (g);
	b = CLIP (b);
      }
      else {
	r = b = g = 0;
      }

      *RGB2++ = r;
      *RGB2++ = g;
      *RGB2++ = b;

    }
    RGB1 += 3 * width;
    RGB2 += 3 * width;
    pty1 += width;
    pty2 += width;
  }
}
