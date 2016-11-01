// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

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
