// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

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
