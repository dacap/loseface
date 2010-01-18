// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

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
