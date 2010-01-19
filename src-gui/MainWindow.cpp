// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "MainWindow.h"
#include "IdentifyDialog.h"
#include "LoginDialog.h"
#include "UserNavigation.h"
#include <QtGui>

MainWindow::MainWindow()
{
  createActions();
  createMenus();

  setWindowTitle(tr("Lose Face"));
  resize(500, 400);

  UserNavigation* userNav = new UserNavigation();
  m_scrollArea = new QScrollArea();
  m_scrollArea->setWidgetResizable(true);
  m_scrollArea->setWidget(userNav);
  m_scrollArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);

  setCentralWidget(m_scrollArea);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
  QMainWindow::resizeEvent(event);

  m_scrollArea->adjustSize();
}

void MainWindow::createActions()
{
  m_fileLoginMode = new QAction(tr("&Login Mode"), this);
  connect(m_fileLoginMode, SIGNAL(triggered()), this, SLOT(goLoginMode()));

  m_fileIdentifyMode = new QAction(tr("&Identify Mode"), this);
  connect(m_fileIdentifyMode, SIGNAL(triggered()), this, SLOT(goIdentifyMode()));

  m_fileExit = new QAction(tr("&Exit"), this);
  m_fileExit->setShortcuts(QKeySequence::Quit);
  connect(m_fileExit, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::createMenus()
{
  m_fileMenu = menuBar()->addMenu(tr("&File"));
  m_fileMenu->addAction(m_fileLoginMode);
  m_fileMenu->addAction(m_fileIdentifyMode);
  m_fileMenu->addSeparator();
  m_fileMenu->addAction(m_fileExit);

  m_editMenu = menuBar()->addMenu(tr("&Edit"));
  m_toolsMenu = menuBar()->addMenu(tr("&Tools"));
  menuBar()->addSeparator();
  m_helpMenu = menuBar()->addMenu(tr("&Help"));
}

void MainWindow::goLoginMode()
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

void MainWindow::goIdentifyMode()
{
  hide();

  IdentifyDialog dlg;
  dlg.exec();

  show();
}

// void MainWindow::paintEvent(QPaintEvent* event)
// {
// }

