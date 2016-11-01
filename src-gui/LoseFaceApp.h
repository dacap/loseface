// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

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
