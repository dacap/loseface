// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QMenu;
class QAction;
class Navigation;

class MainWindow : public QMainWindow
{
  Q_OBJECT

  Navigation* m_navigation;

public:
  MainWindow();

private slots:
  void onLoginMode();
  void onIdentifyMode();
  void onNewUser();
  void onSearchUser();
  void onPreferences();

private:
  void createActions();
  void createMenus();

  QAction* m_fileLoginMode;
  QAction* m_fileIdentifyMode;
  QAction* m_fileExit;
  QAction* m_editNewUser;
  QAction* m_editSearchUser;
  QAction* m_toolsPreferences;
  QMenu* m_fileMenu;
  QMenu* m_editMenu;
  QMenu* m_toolsMenu;
  QMenu* m_helpMenu;

};

#endif
