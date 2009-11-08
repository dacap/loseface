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
