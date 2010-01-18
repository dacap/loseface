// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef DAO_USER_H
#define DAO_USER_H

#include <QSharedPointer.h>
#include "dao/Iterator.h"
#include "dto/User.h"

struct sqlite3;

namespace dao {

  class General;

  typedef QSharedPointer<Iterator<dto::User> > UserIteratorPtr;

  class User
  {
    General* m_general;

  public:
    User(General* general);
    ~User();

    int getCount();
    dto::UserPtr getById(int id);
    UserIteratorPtr getIterator();

    int insertUser(dto::User* user);
    void deleteUser(int id);

  };

}

#endif
