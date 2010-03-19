// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <QWidget>

class QPushButton;
class QImage;
class UserNavigationButton;

namespace dto { class User; }

class SearchPanel;
class CreateUserPanel;
class PreferencesPanel;

class Navigation : public QWidget
{
  Q_OBJECT

  enum panel_t {
    SEARCH,
    CREATE_USER,
    PREFERENCES,
  };

  QWidget* m_currentPanel;
  SearchPanel* m_searchPanel;
  CreateUserPanel* m_createUserPanel;
  PreferencesPanel* m_preferencesPanel;

public:
  Navigation(QWidget* parent = 0);
  ~Navigation();

  void openSearchPanel() { changePanel(SEARCH); }
  void openCreateUserPanel() { changePanel(CREATE_USER); }
  void openPreferencesPanel() { changePanel(PREFERENCES); }

private slots:
//   void addNewUser();
//   void deleteUser(UserNavigationButton* button);
  void openCreateUserPanelSlot();
  void openSearchPanelSlot();

// private:
//   void addUser(dto::User* user, QImage image);
//   void loadUsers();

private:
  void changePanel(panel_t panel);

};

#endif
