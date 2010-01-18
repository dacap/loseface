// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef LOSEFACEAPP_H
#define LOSEFACEAPP_H

#include <QApplication>

namespace dao { class General; }

class MainWindow;

class LoseFaceApp : public QApplication
{
  dao::General* m_generalDao;
  MainWindow* m_mainWindow;

public:
  LoseFaceApp(int& argc, char** argv);
  ~LoseFaceApp();

  dao::General* getGeneralDao();

private:
  void initDao();
};

inline static dao::General* get_general_dao()
{
  return static_cast<LoseFaceApp*>(qApp)->getGeneralDao();
}

#endif
