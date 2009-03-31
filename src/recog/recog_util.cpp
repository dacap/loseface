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
