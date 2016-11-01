// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef LOSEFACE_RECOG_IMAGE_H
#define LOSEFACE_RECOG_IMAGE_H

#ifndef cimg_display
#define cimg_display 0 // Avoid to use display from CImg
#endif

#include "CImg.h"
#include "SharedPtr.h"

namespace recog {

  /// An image
  ///
  class Image {
    unsigned char* bytes;
    int w, h;

  public:

    Image(int bpp, int w, int h) {
      (void)bpp;
      this->w = w;
      this->h = h;
      bytes = new unsigned char[w*h];
    }

    ~Image() {
      delete[] bytes;
    }

    unsigned char* getBytes() const { return bytes; }
    int width() const { return w; }
    int height() const { return h; }

    double getPixel(int x, int y) const {
      return bytes[y*w + x] / 255.0;
    }

    void putPixel(int x, int y, double v) const {
      bytes[y*w + x] = static_cast<int>(1.0 * v / 255.0);
    }

  }; // class Image

  typedef SharedPtr<Image> ImagePtr;

} // namespace recog

#endif	// LOSEFACE_RECOG_IMAGE_H
