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

#include "dao/General.h"
#include "sqlite3.h"
#include <Vaca/String.h>

dao::General::General(const String& fileName)
{
  std::string fileName_utf8 = Vaca::to_utf8(fileName);

  m_dbFileName = fileName;

  // Open the sqlite database
  m_db = NULL;
  sqlite3_open(fileName_utf8.c_str(), &m_db);

  // Create data-model if it does not exist

  // sqlite3_exec(m_db, "CREATE TABLE IF NOT EXISTS users ( "
  // 		     " id INTEGER PRIMARY KEY, "
  // 		     " name TEXT ) ", NULL, NULL, NULL);

  // sqlite3_exec(m_db, "CREATE TABLE IF NOT EXISTS pictures ( "
  // 		     " id INTEGER PRIMARY KEY, "
  // 		     " user_id INTEGER, "
  // 		     " file_name TEXT, "
  // 		     " CONSTRAINT fk_user_id FOREIGN KEY (user_id) REFERENCES users (id) ) ", NULL, NULL, NULL);

  // // Create the admin user if it does not exist
  // dto::UserPtr adminUser = getById(1);
  // if (!adminUser) {
  //   sqlite3_exec(m_db, "INSERT INTO users (id, name) VALUES (1, 'Admin')", NULL, NULL, NULL);

  //   adminUser = getById(1);
  //   if (!adminUser)
  //     throw CreateAdminError();
  // }
}

dao::General::~General()
{
}

String dao::General::getDBFilesPath() const
{
  using namespace Vaca;
  return (file_path(m_dbFileName) / file_title(m_dbFileName)) + L"_files";
}

sqlite3* dao::General::getSqliteDB() const
{
  return m_db;
}

