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
#include "sqlite3.h"
#include <cstring>

dao::User::User(General* general)
  : m_general(general)
{
}

dao::User::~User()
{
}

int dao::User::getCount()
{
  const char* query = "SELECT COUNT(*) FROM users";
  sqlite3* db = m_general->getSqliteDB();
  sqlite3_stmt* stmt;

  sqlite3_prepare(db, query, std::strlen(query), &stmt, NULL);

  int count = 0;
  if (sqlite3_step(stmt) == SQLITE_ROW)
    count = sqlite3_column_int(stmt, 0);

  sqlite3_finalize(stmt);
  return count;
}

dto::UserPtr dao::User::getById(int id)
{
  // TODO Cache of users (See if the user is already loaded)
  // for (UserList::iterator
  // 	 it = m_users.begin(); it != m_users.end(); ++it) {
  //   User* user = *it;

  //   if (user->getId() == id)
  //     return user;
  // }

  // The user has to be loaded from the database

  const char* query = "SELECT name FROM users WHERE id=?";
  sqlite3* db = m_general->getSqliteDB();
  sqlite3_stmt* stmt;

  sqlite3_prepare(db, query, std::strlen(query), &stmt, NULL);
  sqlite3_bind_int(stmt, 1, id);

  dto::UserPtr user;

  // If the user was found in the database
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    user.reset(new dto::User);
    user->setId(id);
    user->setName((const char*)sqlite3_column_text(stmt, 0));
    
    // TODO add it to the cache We add the user in the cache
    //m_users.push_back(new_user);
  }

  sqlite3_finalize(stmt);
  return user;
}

//////////////////////////////////////////////////////////////////////
// Iterator of users

class UserIteratorImpl : public dao::Iterator<dto::User>
{
  sqlite3_stmt* m_stmt;
  
public:

  UserIteratorImpl(sqlite3* db)
    : m_stmt(NULL)
  {
    const char* query = "SELECT id,name FROM users";
    sqlite3_prepare(db, query, std::strlen(query), &m_stmt, NULL);
  }

  ~UserIteratorImpl()
  {
    sqlite3_finalize(m_stmt);
  }

  // Iterator<dto::User> implementation

  bool next(dto::User& user)
  {
    if (sqlite3_step(m_stmt) == SQLITE_ROW) {
      user.setId(sqlite3_column_int(m_stmt, 0));
      user.setName((const char*)sqlite3_column_text(m_stmt, 1));
      return true;
    }
    else
      return false;
  }

};

dao::UserIteratorPtr dao::User::getIterator()
{
  sqlite3* db = m_general->getSqliteDB();

  return UserIteratorPtr(new UserIteratorImpl(db));
}
