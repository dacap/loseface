// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef DTO_PHOTO_H
#define DTO_PHOTO_H

#include <string>
#include <QSharedPointer>

namespace dto {

  class Photo
  {
    int m_id;
    QString m_file_name;

  public:
    int getId() const { return m_id; }
    QString getFileName() const { return m_file_name; }

    void setId(int id) { m_id = id; }
    void setFileName(const QString& file_name) { m_file_name = file_name; }
  };

  typedef QSharedPointer<dto::Photo> PhotoPtr;

}

#endif
