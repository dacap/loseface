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

#include "dao/Photo.h"
#include "dao/General.h"
#include "dao/Iterator.h"
#include <QtSql/QtSql>

dao::Photo::Photo(General* general)
  : m_general(general)
{
}

dao::Photo::~Photo()
{
}

int dao::Photo::getCountForUser(int userId)
{
  QSqlQuery query(m_general->getDatabase());
  query.prepare("SELECT COUNT(*) FROM photos WHERE user_id=?");
  query.addBindValue(userId);
  query.exec();

  int count = 0;
  if (query.next())
    count = query.value(0).toInt();

  return count;
}

dto::PhotoPtr dao::Photo::getById(int id)
{
  QSqlQuery query(m_general->getDatabase());
  query.prepare("SELECT file_name FROM photos WHERE id=?");
  query.addBindValue(id);
  query.exec();

  dto::PhotoPtr photo;

  // If the user was found in the database
  if (query.next()) {
    photo = dto::PhotoPtr(new dto::Photo);
    photo->setId(id);
    photo->setFileName(query.value(0).toString());
  }

  return photo;
}

QImage dao::Photo::loadImage(int id)
{
  QImage image;

  dto::PhotoPtr photo = getById(id);
  if (photo != NULL)
    image.load(m_general->getDBFilesPath() + "/" + photo->getFileName());

  return image;
}

void dao::Photo::deletePhoto(int id)
{
  QSqlQuery query(m_general->getDatabase());
  query.prepare("DELETE FROM photos WHERE id=?");
  query.addBindValue(id);
  query.exec();
}

//////////////////////////////////////////////////////////////////////
// Iterator of photos

class PhotoIteratorImpl : public dao::Iterator<dto::Photo>
{
  QSqlQuery m_query;

public:

  PhotoIteratorImpl(const QSqlDatabase& db, int userId)
    : m_query(db)
  {
    m_query.prepare("SELECT id,file_name FROM photos WHERE user_id=?");
    m_query.addBindValue(userId);
    m_query.exec();
  }

  // Iterator<dto::Photo> implementation

  bool next(dto::Photo& photo)
  {
    if (m_query.next()) {
      photo.setId(m_query.value(0).toInt());
      photo.setFileName(m_query.value(1).toString());
      return true;
    }
    else
      return false;
  }

};

dao::PhotoIteratorPtr dao::Photo::getIterator(int userId)
{
  return PhotoIteratorPtr(new PhotoIteratorImpl(m_general->getDatabase(), userId));
}
