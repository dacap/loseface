// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "LoseFaceApp.h"
#include "MainWindow.h"
#include "dao/General.h"
#include <QFileInfo>
#include <QDir>

LoseFaceApp::LoseFaceApp(int& argc, char** argv)
 : QApplication(argc, argv)
{
  initDao();

  m_mainWindow = new MainWindow();
  m_mainWindow->showMaximized();
}

LoseFaceApp::~LoseFaceApp()
{
  delete m_mainWindow;
  delete m_generalDao;
}

dao::General* LoseFaceApp::getGeneralDao()
{
  return m_generalDao;
}

void LoseFaceApp::initDao()
{
  m_generalDao = NULL;

  QString app_path = applicationDirPath();
  QString dbs_path = app_path + "/dbs";
  QString default_db_path = dbs_path + "/default.db";

  if (!QFileInfo(dbs_path).isDir())
    QDir().mkpath(dbs_path);

  m_generalDao = new dao::General(default_db_path);
}
