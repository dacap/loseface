// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "recog/recog_util.h"
#include "recog/Image.h"
#include <cstdio>
#include <sstream>
#include "CImg.h"

using namespace recog;

bool recog::file_exists(const std::string& fn)
{
  std::FILE* f = std::fopen(fn.c_str(), "r");
  if (f) {
    std::fclose(f);
    return true;
  }
  else
    return false;
}

Image* recog::load_pgm(const std::string& filename)
{
  std::FILE *f;
  char buf[256];
  int w, h, max;

  f = std::fopen(filename.c_str(), "rb");
  if (!f)
    return NULL;

  memset(buf, sizeof(buf), 0);

  // PGM version (P5 expected)
  std::fgets(buf, 256, f);
  if (buf[0] != 'P' || buf[1] != '5')
    return NULL;

  // read width and height fields
  do {
    std::fgets(buf, 256, f);
  } while (buf[0] == '#');
  std::istringstream sbuf1(buf);
  sbuf1 >> w;
  sbuf1 >> h;

  // 255 colors
  do {
    std::fgets(buf, 256, f);
  } while (buf[0] == '#');
  std::istringstream sbuf2(buf);
  sbuf2 >> max;
  if (max != 255)
    return NULL;

  // read bytes
  Image* img = new Image(8, w, h);
  std::fread(img->getBytes(), 1, w*h, f); // read width*height bytes
  std::fclose(f);

  return img;
}

Vector<double> recog::image2vector(const Image* img)
{
  assert(img != NULL);

  Vector<double> v(img->width()*img->height());

  int x, y, i = 0;
  for (y=0; y<img->height(); ++y)
    for (x=0; x<img->width(); ++x)
      v(i++) = img->getPixel(x, y);

  return v;
}

Image* recog::vector2image(const Vector<double>& v, int bpp, int w, int h)
{
  Image* img = new Image(bpp, w, h);
  double min = v.getMin();
  double max = v.getMax();
  int x, y, i = 0;

  for (y=0; y<h; ++y)
    for (x=0; x<w; ++x)
      img->putPixel(x, y, (v(i++)-min) / (max-min));

  return img;
}
