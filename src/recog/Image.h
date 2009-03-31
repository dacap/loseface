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
