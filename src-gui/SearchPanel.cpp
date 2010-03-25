// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "SearchPanel.h"
#include "LoseFaceApp.h"
#include "dao/User.h"
#include "dao/Photo.h"
#include <QtGui>
#include <cassert>

class SearchPanel::SearchThread : public QThread
{
  QString m_term;
  QMutex m_mutex;
  std::list<dto::User> m_users;
  bool m_done;

public:

  SearchThread(QString term, QObject* parent = NULL)
    : QThread(parent)
    , m_term(term)
    , m_done(false)
  {
  }

  void run()
  {
    dao::General* generalDao = get_general_dao();
    if (generalDao) {
      dto::User user;
      dao::User userDao(generalDao);
      dao::UserIteratorPtr userIter = userDao.getSearchIterator(m_term.toUtf8());
      while (userIter->next(user)) {
	// Add the user in the list
	{
	  QMutexLocker locker(&m_mutex);
	  m_users.push_back(user);
	}
      }
    }
    m_done = true;
  }

  bool getNextUser(dto::User& user)
  {
    QMutexLocker locker(&m_mutex);

    if (!m_users.empty()) {
      // Get the next user from the list
      user = *m_users.begin();

      // And remove it
      m_users.erase(m_users.begin());

      return true;
    }
    else
      return false;
  }

  bool done() const
  {
    return m_done;
  }

};

SearchPanel::SearchPanel(QWidget* parent)
  : QWidget(parent)
  , m_thread(NULL)
  , m_poll(NULL)
{
  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);

  QLabel* titleLabel = new QLabel(tr("Search for registered users:"));
  titleLabel->setFont(QFont("Tahoma", 12, QFont::Bold));

  m_searchBox = new QLineEdit(tr(""));
  m_searchButton = new QPushButton(tr("Search"));
  connect(m_searchButton, SIGNAL(clicked()), this, SLOT(onSearchClicked()));

  QPushButton* newUser = new QPushButton(tr("New User"));
  connect(newUser, SIGNAL(clicked()), this, SLOT(onNewUserClicked()));

  m_usersList = new QListView();

  QGridLayout* layout = new QGridLayout();
  layout->addWidget(titleLabel, 0, 0);
  layout->addWidget(m_searchBox, 1, 0);
  layout->addWidget(m_searchButton, 1, 1);
  layout->addWidget(newUser, 1, 2);
  layout->addWidget(m_usersList, 2, 0, 1, 3);
  layout->setColumnStretch(0, 100);
  layout->setColumnStretch(3, 100);
  layout->setRowStretch(2, 100);

  setLayout(layout);
}

SearchPanel::~SearchPanel()
{
  delete m_poll;
}

void SearchPanel::onSearchClicked()
{
  m_searchButton->setEnabled(false);

  // Create the search thread
  m_thread = new SearchThread(m_searchBox->text(), this);
  m_thread->start();

  m_poll = new QTimer(this);
  m_poll->setSingleShot(false);
  m_poll->setInterval(100);
  connect(m_poll, SIGNAL(timeout()), this, SLOT(pollResults()));
  m_poll->start();
}

void SearchPanel::onNewUserClicked()
{
  emit createUser();
}

#if 0
class UserResultsModel : public QAbstractListModel
{
  std::vector<dto::User> m_users;

public:
  UserResultsModel(QObject* parent = 0)
    : QAbstractListModel(parent)
  {
  }

  int rowCount(const QModelIndex& parent = QModelIndex()) const
  {
    return m_users.size();
  }

  QVariant data(const QModelIndex& index, int role) const
  {
    if (!index.isValid() || index.row() >= m_users.size())
      return QVariant();

    if (role == Qt::DisplayRole)
      return m_users[index.row()];
    else
      return QVariant();
  }

  QVariant headerData(int section, Qt::Orientation orientation,
		      int role = Qt::DisplayRole) const
  {
    return QVariant();
  }

};

class UserResultsItem : public QListItem
{
  dto::User m_user;
  QImage m_photo;

public:

  UserResultItem(dto::User user)
    : m_user(user)
  {
    dao::General* generalDao = get_general_dao();
    if (generalDao) {
      // Load the first photo of the specified user
      dao::Photo pictureDao(generalDao);
      dao::PhotoIteratorPtr pictureIter = pictureDao.getIterator(user.getId());
      dto::Photo picture;
      if (pictureIter->next(picture)) {
	m_photo = pictureDao.loadImage(picture.getId());
      }
    }
  }

  // QSize sizeHint() const
  // {
  //   return QSize(128, 128);
  // }
  
  // void paintEvent(QPaintEvent* event)
  // {
  //   QPainter painter(this);
  //   QPen pen(QColor(0, 128, 255));
  //   painter.setPen(pen);

  //   painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  //   QRect rc(0, 0, width()-1, height()-1);

  //   if (underMouse() || m_forceHot) {
  //     QBrush brush(QColor(100, 240, 255));
  //     painter.setBrush(brush);
  //   }

  //   painter.drawRoundedRect(rc, 8, 8);

  //   QSize sz;

  //   if (m_photo.isNull()) {
  //     QFont font("Tahoma", 14);
  //     QString text = tr("No photo");
  //     painter.setFont(font);
  //     painter.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, text);
  //     sz = QFontMetrics(font).size(Qt::TextSingleLine, text);
  //   }
  //   else {
  //     sz = m_photo.size();
  //     sz.scale(rc.size()*0.9, Qt::KeepAspectRatio);
  //     painter.drawImage(QRect(QPoint(4, rc.center().y()-sz.height()/2), sz), m_photo);
  //   }

  //   painter.setFont(QFont("Tahoma", 14));
  //   painter.drawText(QPoint(4+sz.width()+4, rc.y()), tr("Name"));
  // }

};
#endif

void SearchPanel::pollResults()
{
  assert(m_thread != NULL);

  // Get all users from the working thread
  dto::User user;
  while (m_thread->getNextUser(user)) {
    // m_usersList->addItem(new UserResultItem(user));
  }

  // If the thread finishes, join it and stop the timer
  if (m_thread->done()) {
    m_thread->wait();
    delete m_thread;
    m_thread = NULL;

    m_poll->stop();
    delete m_poll;
    m_poll = NULL;

    m_searchButton->setEnabled(true);
  }
}

