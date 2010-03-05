// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef LOSEFACE_UTIL_H
#define LOSEFACE_UTIL_H

#include <string>
#include <cstdio>
#include <sstream>

template<class T>
std::string lexical_cast(T p)
{
  std::ostringstream t;
  t << p;
  return t.str();
}

template<class T1, class T2>
T1 lexical_cast(const T2& a)
{
  std::istringstream s(a);
  T1 v;
  s >> v;
  return v;
}

template<class T>
struct delete_functor
{
  delete_functor &operator()(T* t) {
    delete t;
    return *this;
  }
};

#endif // LOSEFACE_UTIL_H
