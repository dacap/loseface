// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "dao/User.h"
#include "dao/General.h"
#include "dao/Iterator.h"
#include <QtSql/QtSql>

dao::User::User(General* general)
  : m_general(general)
{
}

dao::User::~User()
{
}

int dao::User::getCount()
{
  QSqlQuery query(m_general->getDatabase());
  query.exec("SELECT COUNT(*) FROM users");

  int count = 0;
  if (query.next())
    count = query.value(0).toInt();

  return count;
}

dto::UserPtr dao::User::getById(int id)
{
  QSqlQuery query(m_general->getDatabase());
  query.prepare("SELECT name FROM users WHERE id=?");
  query.addBindValue(id);
  query.exec();

  dto::UserPtr user;

  // If the user was found in the database
  if (query.next()) {
    user = dto::UserPtr(new dto::User);
    user->setId(id);
    user->setName(query.value(0).toString());
  }

  return user;
}

int dao::User::insertUser(dto::User* user)
{
  QSqlQuery query(m_general->getDatabase());
  query.prepare("INSERT INTO users (name) VALUES (?)");
  query.addBindValue(user->getName());
  query.exec();
  
  QVariant id = query.lastInsertId();
  if (id.isValid())
    return id.toInt();
  else
    return -1;
}

void dao::User::deleteUser(int id)
{
  QSqlQuery query(m_general->getDatabase());
  query.prepare("DELETE FROM users WHERE id=?");
  query.addBindValue(id);
  query.exec();
}

//////////////////////////////////////////////////////////////////////
// Iterator of users

class UserIteratorImpl : public dao::Iterator<dto::User>
{
  QSqlQuery m_query;
  
public:

  UserIteratorImpl(const QSqlDatabase& db)
    : m_query(db)
  {
    m_query.exec("SELECT id,name FROM users");
  }

  // Iterator<dto::User> implementation

  bool next(dto::User& user)
  {
    if (m_query.next()) {
      user.setId(m_query.value(0).toInt());
      user.setName(m_query.value(0).toString());
      return true;
    }
    else
      return false;
  }

};

dao::UserIteratorPtr dao::User::getIterator()
{
  return UserIteratorPtr(new UserIteratorImpl(m_general->getDatabase()));
}
