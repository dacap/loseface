// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef LOSEFACE_APPROX_EQ_H
#define LOSEFACE_APPROX_EQ_H

template<typename T>
bool approx_eq(T a, T b, unsigned precision)
{
  T diff = std::fabs(a - b);
  T threshold = 1.0 / std::pow(static_cast<T>(10.0),
			       static_cast<T>(precision));
  return (diff < threshold);
}

#endif // LOSEFACE_APPROX_EQ_H
