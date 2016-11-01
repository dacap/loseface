// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

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
  query.prepare("SELECT user_name,first_name,last_name,email,ssn FROM users WHERE id=?");
  query.addBindValue(id);
  query.exec();

  dto::UserPtr user;

  // If the user was found in the database
  if (query.next()) {
    user = dto::UserPtr(new dto::User);
    user->setId(id);
    user->setUserName(query.value(0).toString());
    user->setFirstName(query.value(1).toString());
    user->setLastName(query.value(2).toString());
    user->setEmail(query.value(3).toString());
    user->setSsn(query.value(4).toString());
  }

  return user;
}

int dao::User::insertUser(dto::User* user)
{
  QSqlQuery query(m_general->getDatabase());
  query.prepare("INSERT INTO users (user_name,first_name,last_name,email,ssn) VALUES (?,?,?,?,?)");
  query.addBindValue(user->getUserName());
  query.addBindValue(user->getFirstName());
  query.addBindValue(user->getLastName());
  query.addBindValue(user->getEmail());
  query.addBindValue(user->getSsn());
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
    m_query.exec("SELECT id,user_name,first_name,last_name,email FROM users");
  }

  // Iterator<dto::User> implementation

  bool next(dto::User& user)
  {
    if (m_query.next()) {
      user.setId(m_query.value(0).toInt());
      user.setUserName(m_query.value(1).toString());
      user.setFirstName(m_query.value(2).toString());
      user.setLastName(m_query.value(3).toString());
      user.setEmail(m_query.value(4).toString());
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

dao::UserIteratorPtr dao::User::getSearchIterator(const char* searchWords)
{
  return UserIteratorPtr(new UserIteratorImpl(m_general->getDatabase()));
}
