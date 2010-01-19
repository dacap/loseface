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

QSize UserNavigation::sizeHint() const
{
  return QSize(0, 0);
  // return QSize(parentWidget()->width(),
  // 	       layout()->heightForWidth(parentWidget()->width()));
  // return QSize(0, layout()->heightForWidth(parentWidget()->width()));
}

void UserNavigation::addNewUser()
{
  dto::User user;
  user.setName(tr("No Name"));

  dao::General* generalDao = get_general_dao();
  if (generalDao) {
    dao::User userDao(generalDao);
    userDao.insertUser(&user);
  }

  addUser(&user, QImage());
  updateGeometry();
}

void UserNavigation::deleteUser(UserNavigationButton* userButton)
{
  const dto::User user(userButton->getUser());
  QString msg;
  msg = QString(tr("Are you sure you want to remove user \"%1\"?")).arg(user.getName());

  int ret = QMessageBox::warning(this, tr("Confirmation"),
				 msg,
				 QMessageBox::Yes | QMessageBox::No,
				 QMessageBox::No);
  if (ret != QMessageBox::Yes)
    return;

  // Remove the user from the database
  dao::General* generalDao = get_general_dao();
  if (generalDao) {
    dao::User userDao(generalDao);
    userDao.deleteUser(user.getId());
  }

  // Remove the button to view this user
  userButton->setParent(NULL);
  userButton->deleteLater();
  updateGeometry();
}

void UserNavigation::addUser(dto::User* user, QImage image)
{
  int addUserIndex = layout()->indexOf(m_addUser);
  QLayoutItem* addUserItem = layout()->takeAt(addUserIndex);

  UserNavigationButton* userButton = new UserNavigationButton(this, *user, image);
  connect(userButton, SIGNAL(userDeleted(UserNavigationButton*)),
	  this, SLOT(deleteUser(UserNavigationButton*)));

  layout()->addWidget(userButton);
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

      addUser(&user, image);
    }
  }
}
