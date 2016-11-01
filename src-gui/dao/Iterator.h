// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

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
