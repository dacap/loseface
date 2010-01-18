// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "dao/General.h"
#include <QFileInfo>

dao::General::General(const QString& fileName)
{
  m_dbFileName = fileName;

  // Open the sqlite database
  m_db = QSqlDatabase::addDatabase("QSQLITE");
  m_db.setDatabaseName(fileName);
  m_db.open();
}

dao::General::~General()
{
  m_db.close();
}

QString dao::General::getDBFilesPath() const
{
  QFileInfo fi(m_dbFileName);
  QString path;
  path.append(fi.path());
  path.append("/");
  path.append(fi.baseName());
  path.append("_files");
  return path;
}

QSqlDatabase dao::General::getDatabase() const
{
  return m_db;
}
