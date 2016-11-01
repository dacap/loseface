// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "Recognizer.h"
#include "dao/User.h"
#include "dao/Photo.h"
#include "LoseFaceApp.h"
#include "ActivationFunctions.h"
#include <QThread>
#include <QString>

class RecognizerBuilderThread : public QThread
{
  Mlp& m_mlp;
  bool m_stop;
public:
  RecognizerBuilderThread(Mlp& mlp, QObject* parent = NULL)
    : m_mlp(mlp)
    , QThread(parent)
  {
    m_stop = false;
  }
  void stopTrainingProcess()
  {
    m_stop = true;
  }
  void run()
  {
    int subjects = 0;

    // Load all photos of all users and convert them to training patterns
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
	  while (pictureIter->next(picture)) {
	    image = pictureDao.loadImage(picture.getId());

	    printf("Loading picture %d for subject %d\n", user.getId(), picture.getId());
	    // TODO
	  }
	}

	subjects++;
      }
    }

    fflush(stdout);

    if (!subjects)
      return;

    m_mlp = Mlp(10, 10, subjects);
    m_mlp.setHiddenActivationFunction(Logsig());
    m_mlp.setOutputActivationFunction(Logsig());
    m_mlp.initRandom(-1.0, +1.0);

    // Backpropagation bp(m_mlp);
    // while (!m_stop) {
    //   //exec();			// TODO train the MLP in background
    //   m_mlp.train();
    // }
  }
};

Recognizer::Recognizer()
{
  m_thread = NULL;
}

Recognizer::~Recognizer()
{
  if (m_thread != NULL) {
    m_thread->stopTrainingProcess();
    m_thread->wait();
    delete m_thread;
  }
}

bool Recognizer::login(const QImage& photo, const QString& name)
{
  return false;
}

QString Recognizer::identify(const QImage& photo)
{
  return QString();
}

void Recognizer::rebuild()
{
  if (m_thread == NULL) {
    m_thread = new RecognizerBuilderThread(m_mlp);
    m_thread->start();
  }
}

double Recognizer::getBuildProgress()
{
  return 0.5;
}
