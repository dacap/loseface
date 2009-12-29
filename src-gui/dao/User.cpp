// Lose Face - An open source face recognition project
// Copyright (C) 2008-2009 David Capello
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the authors nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

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
