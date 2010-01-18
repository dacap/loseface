// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "UserNavigation.h"
#include "UserNavigationButton.h"
#include "FlowLayout.h"
#include "dao/General.h"
#include "dao/User.h"
#include "dao/Photo.h"
#include "dto/User.h"
#include "dto/Photo.h"
#include "LoseFaceApp.h"
#include <QtGui>

UserNavigation::UserNavigation(QWidget* parent)
  : QWidget(parent)
{
  m_addUser = new QPushButton(tr("Add User"));
  connect(m_addUser, SIGNAL(clicked()), this, SLOT(addNewUser()));

  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);

  FlowLayout* layout = new FlowLayout();
  setLayout(layout);

  layout->addWidget(m_addUser);

  // Load all users from DB
  loadUsers();
}

void UserNavigation::addNewUser()
{
  addUser(dto::User(), QImage());
}

void UserNavigation::addUser(const dto::User& user, QImage image)
{
  int addUserIndex = layout()->indexOf(m_addUser);
  QLayoutItem* addUserItem = layout()->takeAt(addUserIndex);

  layout()->addWidget(new UserNavigationButton(this, user, image));
  layout()->addItem(addUserItem);
}

void UserNavigation::loadUsers()
{
  dao::General* generalDao = get_general_dao();
  if (generalDao) {
    dao::User userDao(generalDao);
    dao::Photo pictureDao(generalDao);
    dao::UserIteratorPtr userIter = userDao.getIterator();
    dto::User user;

    while (userIter->next(user)) {
      QImage image;
      dao::PhotoIteratorPtr pictureIter = pictureDao.getIterator(user.getId());
      dto::Photo picture;
      if (pictureIter->next(picture)) {
      	image = pictureDao.loadImage(picture.getId());
      }

      addUser(user, image);
    }
  }
}
