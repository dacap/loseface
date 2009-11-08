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

  setCentralWidget(new UserNavigation());
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
  hide();

  LoginDialog dlg;
  bool tryAgain = false;

  do {
    tryAgain = false;
    dlg.exec();

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

