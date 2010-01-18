// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef USERNAVIGATION_H
#define USERNAVIGATION_H

#include <QWidget>

class QPushButton;
class QImage;

namespace dto { class User; }

class UserNavigation : public QWidget
{
  Q_OBJECT

  QPushButton* m_addUser;

public:
  UserNavigation(QWidget* parent = 0);

private slots:
  void addNewUser();

private:
  void addUser(const dto::User& user, QImage image);
  void loadUsers();

};

#endif
