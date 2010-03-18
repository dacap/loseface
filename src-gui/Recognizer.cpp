// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "Recognizer.h"
#include <QThread>
#include <QString>

class RecognizerBuilderThread : public QThread
{
public:
  RecognizerBuilderThread(QObject* parent = NULL)
    : QThread(parent)
  {
  }
  void run()
  {
    exec();			// TODO train the MLP in background
  }
};

Recognizer::Recognizer()
{
  m_thread = NULL;
}

Recognizer::~Recognizer()
{
  if (m_thread != NULL) {
    m_thread->exit(1);
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
    m_thread = new RecognizerBuilderThread();
    m_thread->start();
  }
}

double Recognizer::getBuildProgress()
{
  return 0.0;
}
