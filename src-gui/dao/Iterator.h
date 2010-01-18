// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef DAO_ITERATOR_H
#define DAO_ITERATOR_H

namespace dao {

  template<class DtoObject>
  class Iterator
  {
  public:
    virtual ~Iterator() { }
    virtual bool next(DtoObject& object) = 0;
  };

}

#endif
