// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

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
