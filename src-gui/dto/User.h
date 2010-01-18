// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef DTO_USER_H
#define DTO_USER_H

#include <QSharedPointer>
#include <QString>

namespace dto {

  class User
  {
    int m_id;
    QString m_name;

  public:
    int getId() const { return m_id; }
    QString getName() const { return m_name; }

    void setId(int id) { m_id = id; }
    void setName(const QString& name) { m_name = name; }
  };

  typedef QSharedPointer<dto::User> UserPtr;

}

#endif
