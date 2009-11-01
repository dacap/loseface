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

#include "dao/Picture.h"
#include "dao/General.h"
#include "dao/Iterator.h"
#include "sqlite3.h"
#include <Vaca/ImagePng.h>
#include <cstring>

dao::Picture::Picture(General* general)
  : m_general(general)
{
}

dao::Picture::~Picture()
{
}

int dao::Picture::getCountForUser(int userId)
{
  const char* query = "SELECT COUNT(*) FROM pictures WHERE user_id=?";
  sqlite3* db = m_general->getSqliteDB();
  sqlite3_stmt* stmt;

  sqlite3_prepare(db, query, std::strlen(query), &stmt, NULL);
  sqlite3_bind_int(stmt, 1, userId);

  int count = 0;
  if (sqlite3_step(stmt) == SQLITE_ROW)
    count = sqlite3_column_int(stmt, 0);

  sqlite3_finalize(stmt);
  return count;
}

dto::PicturePtr dao::Picture::getById(int id)
{
  // TODO Cache of users (See if the user is already loaded)
  // for (PictureList::iterator
  // 	 it = m_users.begin(); it != m_users.end(); ++it) {
  //   Picture* user = *it;

  //   if (user->getId() == id)
  //     return user;
  // }

  // The picture has to be loaded from the database

  const char* query = "SELECT file_name FROM pictures WHERE id=?";
  sqlite3* db = m_general->getSqliteDB();
  sqlite3_stmt* stmt;

  sqlite3_prepare(db, query, std::strlen(query), &stmt, NULL);
  sqlite3_bind_int(stmt, 1, id);

  dto::PicturePtr picture;

  // If the user was found in the database
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    picture.reset(new dto::Picture);
    picture->setId(id);
    picture->setFileName((const char*)sqlite3_column_text(stmt, 0));
    
    // TODO add it to the cache We add the user in the cache
    //m_users.push_back(new_user);
  }

  sqlite3_finalize(stmt);
  return picture;
}

Vaca::Image dao::Picture::loadImage(int id)
{
  dto::PicturePtr picture = getById(id);
  if (picture != NULL) {
    return Vaca::ImagePng::load(m_general->getDBFilesPath()
				/ convert_to<String>(picture->getFileName()));
  }
  else
    return Vaca::Image();
}

//////////////////////////////////////////////////////////////////////
// Iterator of pictures

class PictureIteratorImpl : public dao::Iterator<dto::Picture>
{
  sqlite3_stmt* m_stmt;
  
public:

  PictureIteratorImpl(sqlite3* db, int userId)
    : m_stmt(NULL)
  {
    const char* query = "SELECT id,file_name FROM pictures WHERE user_id=?";
    sqlite3_prepare(db, query, std::strlen(query), &m_stmt, NULL);
    sqlite3_bind_int(m_stmt, 1, userId);
  }

  ~PictureIteratorImpl()
  {
    sqlite3_finalize(m_stmt);
  }

  // Iterator<dto::Picture> implementation

  bool next(dto::Picture& picture)
  {
    if (sqlite3_step(m_stmt) == SQLITE_ROW) {
      picture.setId(sqlite3_column_int(m_stmt, 0));
      picture.setFileName((const char*)sqlite3_column_text(m_stmt, 1));
      return true;
    }
    else
      return false;
  }

};

dao::PictureIteratorPtr dao::Picture::getIterator(int userId)
{
  sqlite3* db = m_general->getSqliteDB();

  return PictureIteratorPtr(new PictureIteratorImpl(db, userId));
}
