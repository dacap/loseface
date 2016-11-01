// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include <QObject>
#include "Mlp.h"

class RecognizerBuilderThread;
class QString;
class QImage;

class Recognizer : public QObject
{
  Q_OBJECT

  Mlp m_mlp;
  RecognizerBuilderThread* m_thread;

public:
  Recognizer();
  ~Recognizer();

  bool login(const QImage& photo, const QString& name);
  QString identify(const QImage& photo);

  void rebuild();
  double getBuildProgress();

};

#endif
