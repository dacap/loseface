// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef SEARCHPANEL_H
#define SEARCHPANEL_H

#include <QWidget>

class QLineEdit;
class QPushButton;
class QTableView;
class QTimer;
class QStandardItemModel;

class SearchPanel : public QWidget
{
  Q_OBJECT

  class SearchThread;

  QLineEdit* m_searchBox;
  QPushButton* m_searchButton;
  QTableView* m_usersList;
  SearchThread* m_thread;
  QTimer* m_poll;
  QStandardItemModel* m_model;

public:
  SearchPanel(QWidget* parent = 0);
  ~SearchPanel();

signals:
  void createUser();

private slots:
  void onSearchClicked();
  void onNewUserClicked();
  void pollResults();

private:
  void setupModel();
  void setupView();

// private slots:
//   void addNewUser();
//   void deleteUser(UserSearchPanelButton* button);

// private:
//   void addUser(dto::User* user, QImage image);
//   void loadUsers();

};

#endif
