// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef LOSEFACE_RECOG_UTIL_H
#define LOSEFACE_RECOG_UTIL_H

#include <string>
#include "Vector.h"

namespace recog {

  class Image;

  bool file_exists(const std::string& fn);
  Image* load_pgm(const std::string& filename);

  Vector<double> image2vector(const Image* img);
  Image* vector2image(const Vector<double>& v, int bpp, int w, int h);
  
} // namespace recog

#endif // LOSEFACE_RECOG_UTIL_H
