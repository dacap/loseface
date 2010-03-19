// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "Navigation.h"
// #include "UserNavigationButton.h"
// #include "dao/General.h"
// #include "dao/User.h"
// #include "dao/Photo.h"
// #include "dto/User.h"
// #include "dto/Photo.h"
// #include "LoseFaceApp.h"
#include <QHBoxLayout>

#include "SearchPanel.h"
#include "CreateUserPanel.h"
#include "PreferencesPanel.h"

Navigation::Navigation(QWidget* parent)
  : QWidget(parent)
{
  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);
  setLayout(new QHBoxLayout());

  m_currentPanel = NULL;
  m_searchPanel = NULL;
  m_createUserPanel = NULL;
  m_preferencesPanel = NULL;

  // setBackgroundRole(QPalette::Window);
  // setBackgroundRole(QPalette::Base);
  // setAutoFillBackground(true);

  // layout()->addWidget(m_searchPanel);

  // QGridLayout* layout = new QGridLayout();
  // layout->addWidget(m_titleLabel, 0, 0);
  // layout->addWidget(m_nameLabel, 1, 0);
  // layout->addWidget(m_searchButton, 1, 1);
  // layout->addWidget(m_addUser, 1, 2);
  // layout->setColumnStretch(0, 100);
  // layout->setColumnStretch(3, 100);
  // layout->setRowStretch(2, 100);

  // layout->addWidget();

  // QVBoxLayout* vboxLayout = new QVBoxLayout;
  // vboxLayout->addWidget(m_addUser);
  // layout->addLayout(vboxLayout, 2, 0, 1, 2);

  // setLayout(layout);

  // Load all users from DB
  // loadUsers();

  changePanel(SEARCH);
}

Navigation::~Navigation()
{
  delete m_searchPanel;
  delete m_createUserPanel;
  delete m_preferencesPanel;
}

void Navigation::changePanel(panel_t panel)
{
  QWidget* oldPanel = m_currentPanel;

  switch (panel) {

    case SEARCH:
      if (!m_searchPanel) {
	m_searchPanel = new SearchPanel(this);
	connect(m_searchPanel, SIGNAL(createUser()), this, SLOT(openCreateUserPanelSlot()));
      }
      m_currentPanel = m_searchPanel;
      break;

    case CREATE_USER:
      if (!m_createUserPanel)
	m_createUserPanel = new CreateUserPanel(this);
      m_currentPanel = m_createUserPanel;
      break;

    case PREFERENCES:
      if (!m_preferencesPanel)
	m_preferencesPanel = new PreferencesPanel(this);
      m_currentPanel = m_preferencesPanel;
      break;
  }

  if (oldPanel != NULL && m_currentPanel != oldPanel) {
    layout()->removeWidget(oldPanel);
    oldPanel->setVisible(false);
  }

  if (m_currentPanel != NULL && m_currentPanel != oldPanel) {
    layout()->addWidget(m_currentPanel);
    m_currentPanel->setVisible(true);
  }
}

void Navigation::openCreateUserPanelSlot()
{
  openCreateUserPanel();
}

#if 0
// QSize UserNavigation::sizeHint() const
// {
//   return QSize(1024, 1024);
//   // return QSize(parentWidget()->width(),
//   // 	       layout()->heightForWidth(parentWidget()->width()));
//   // return QSize(0, layout()->heightForWidth(parentWidget()->width()));
// }

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
#if 0
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
#endif
}

void UserNavigation::addUser(dto::User* user, QImage image)
{
#if 0
  int addUserIndex = layout()->indexOf(m_addUser);
  QLayoutItem* addUserItem = layout()->takeAt(addUserIndex);

  UserNavigationButton* userButton = new UserNavigationButton(this, *user, image);
  connect(userButton, SIGNAL(userDeleted(UserNavigationButton*)),
	  this, SLOT(deleteUser(UserNavigationButton*)));

  layout()->addWidget(userButton);
  layout()->addItem(addUserItem);
#endif
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
#endif
