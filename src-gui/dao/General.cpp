// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

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
