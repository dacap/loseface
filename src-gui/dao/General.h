// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef DAO_GENERAL_H
#define DAO_GENERAL_H

#include <QString>
#include <QtSql/QSqlDatabase>

namespace dao {

  class General
  {
    QString m_dbFileName;
    QSqlDatabase m_db;

  public:
    General(const QString& fileName);
    ~General();

    QString getDBFilesPath() const;
    QSqlDatabase getDatabase() const;
  };

}

#endif
