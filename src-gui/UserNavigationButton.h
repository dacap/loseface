// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef USERNAVIGATIONBUTTON_H
#define USERNAVIGATIONBUTTON_H

#include <QWidget>
#include "dto/User.h"

class UserNavigationButton : public QWidget
{
  Q_OBJECT

  dto::User m_user;
  QImage m_photo;
  bool m_forceHot;

public:
  UserNavigationButton(QWidget* parent = 0,
		       const dto::User& user = dto::User(),
		       QImage photo = QImage());

  QSize sizeHint() const;

  const dto::User& getUser() const { return m_user; }

signals:
  void userDeleted(UserNavigationButton*);

protected:
  void paintEvent(QPaintEvent* event);
  void enterEvent(QEvent* event);
  void leaveEvent(QEvent* event);
  void mousePressEvent(QMouseEvent* event);

private slots:
  void deleteUser();

private:
  void createActions();

  QAction* m_deleteUser;

};

#endif
