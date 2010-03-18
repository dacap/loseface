// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef SEARCHPANEL_H
#define SEARCHPANEL_H

#include <QWidget>

class QListWidget;
// class QImage;
// class UserSearchPanelButton;

class SearchPanel : public QWidget
{
  Q_OBJECT

  QListWidget* m_usersList;

public:
  SearchPanel(QWidget* parent = 0);

signals:
  void createUser();

private slots:
  void onSearchClicked();
  void onNewUserClicked();

// private slots:
//   void addNewUser();
//   void deleteUser(UserSearchPanelButton* button);

// private:
//   void addUser(dto::User* user, QImage image);
//   void loadUsers();

};

#endif
