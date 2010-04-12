// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "MainWindow.h"
#include "LoginDialog.h"
#include "Navigation.h"
#include "Recognizer.h"
#include <QtGui>

MainWindow::MainWindow()
{
  createActions();
  createMenus();

  setWindowTitle(tr("Lose Face"));
  resize(500, 400);

  m_navigation = new Navigation(this);
  setCentralWidget(m_navigation);
}

void MainWindow::createActions()
{
  m_fileLoginMode = new QAction(tr("&Login Mode"), this);
  connect(m_fileLoginMode, SIGNAL(triggered()), this, SLOT(onLoginMode()));

  m_fileIdentifyMode = new QAction(tr("&Identify Mode"), this);
  connect(m_fileIdentifyMode, SIGNAL(triggered()), this, SLOT(onIdentifyMode()));

  m_retrain = new QAction(tr("&Train Model (TEST ONLY)"), this);
  connect(m_retrain, SIGNAL(triggered()), this, SLOT(onTrainModel()));

  m_fileExit = new QAction(tr("&Exit"), this);
  m_fileExit->setShortcuts(QKeySequence::Quit);
  connect(m_fileExit, SIGNAL(triggered()), this, SLOT(close()));

  m_editNewUser = new QAction(tr("&New User"), this);
  connect(m_editNewUser, SIGNAL(triggered()), this, SLOT(onNewUser()));

  m_editSearchUser = new QAction(tr("&Search User"), this);
  connect(m_editSearchUser, SIGNAL(triggered()), this, SLOT(onSearchUser()));

  m_toolsPreferences = new QAction(tr("&Preferences"), this);
  connect(m_toolsPreferences, SIGNAL(triggered()), this, SLOT(onPreferences()));
}

void MainWindow::createMenus()
{
  m_fileMenu = menuBar()->addMenu(tr("&File"));
  m_fileMenu->addAction(m_fileLoginMode);
  m_fileMenu->addAction(m_fileIdentifyMode);
  m_fileMenu->addSeparator();
  m_fileMenu->addAction(m_retrain);
  m_fileMenu->addSeparator();
  m_fileMenu->addAction(m_fileExit);

  m_editMenu = menuBar()->addMenu(tr("&Edit"));
  m_editMenu->addAction(m_editNewUser);
  m_editMenu->addAction(m_editSearchUser);

  m_toolsMenu = menuBar()->addMenu(tr("&Tools"));
  m_toolsMenu->addAction(m_toolsPreferences);

  menuBar()->addSeparator();

  m_helpMenu = menuBar()->addMenu(tr("&Help"));
}

void MainWindow::webCamMode(bool identify)
{
  // Create login dialog
  LoginDialog dlg(this);
  dlg.show();

  // Check if the camera is working
  if (!dlg.initCamera()) {
    QMessageBox::critical(this, tr("Error"),
			  tr("Camera not found or it cannot be initialized."));
    return;
  }

  // Hide main window
  hide();

  bool tryAgain = false;
  do {
    tryAgain = false;
    dlg.exec();			// Show the login dialog

    switch (dlg.result()) {

      case LoginDialog::LOGIN_RESULT:
	// TODO login user and show result
	dlg.show();
	tryAgain = true;
	break;

      case LoginDialog::ENTER_ADMIN_RESULT:
	show();
	break;

      default:
	// close...
	break;
    }
  } while (tryAgain);
}

void MainWindow::onLoginMode()
{
  webCamMode(false);
}

void MainWindow::onIdentifyMode()
{
  webCamMode(true);
}

void MainWindow::onTrainModel()
{
  Recognizer recognizer;
  recognizer.rebuild();
}

void MainWindow::onNewUser()
{
  m_navigation->openCreateUserPanel();
}

void MainWindow::onSearchUser()
{
  m_navigation->openSearchPanel();
}

void MainWindow::onPreferences()
{
  m_navigation->openPreferencesPanel();
}
