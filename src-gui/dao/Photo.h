// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef DAO_PHOTO_H
#define DAO_PHOTO_H

#include <QSharedPointer>
#include <QImage>
#include "dao/Iterator.h"
#include "dto/Photo.h"

struct sqlite3;

namespace dao {

  class General;

  typedef QSharedPointer<Iterator<dto::Photo> > PhotoIteratorPtr;

  class Photo
  {
    General* m_general;

  public:
    Photo(General* general);
    ~Photo();

    int getCountForUser(int userId);
    PhotoIteratorPtr getIterator(int userId);

    dto::PhotoPtr getById(int id);
    QImage loadImage(int id);

    void deletePhoto(int id);

  };

}

#endif
