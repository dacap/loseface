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

#include "MainFrame.h"
#include "CmdIds.h"
#include <Vaca/Vaca.h>

using namespace Vaca;

MainFrame::MainFrame()
  : Frame(L"Lose Face", NULL, Frame::Styles::Default |
			      Widget::Styles::AcceptFiles)
  , m_navigator(this)
{
  setLayout(new ClientLayout);
  setMenuBar(createMenuBar());
}

void MainFrame::refresh()
{
  m_navigator.refresh();
}

MenuBar* MainFrame::createMenuBar()
{
  MenuBar* menuBar	 = new MenuBar();
  Menu* fileMenu	 = new Menu(L"&Archivo");
  Menu* editMenu	 = new Menu(L"&Edición");
  // Menu* viewMenu	 = new Menu(L"&Ver");
  // Menu* usersMenu	 = new Menu(L"&Usuarios");
  Menu* toolsMenu	 = new Menu(L"&Herramientas");
  Menu* helpMenu	 = new Menu(L"Ay&uda");
  Menu* dbMenu		 = new Menu(L"&Base de Datos");
  Menu* researchMenu	 = new Menu(L"&Investigación");

  menuBar->add(fileMenu);
  menuBar->add(editMenu);
  // menuBar->add(viewMenu);
  // menuBar->add(usersMenu);
  menuBar->add(toolsMenu);
  menuBar->add(helpMenu);

  // File menu
  fileMenu->add(dbMenu);
  fileMenu->addSeparator();
  fileMenu->add(L"&Salir",		CMD_FILE_EXIT);

  // DB menu
  dbMenu->add(L"&Conexión",		CMD_DB_CONNECTIONS);
  dbMenu->addSeparator();
  dbMenu->add(L"&Importar",		CMD_DB_IMPORT);
  dbMenu->add(L"&Exportar",		CMD_DB_EXPORT);

  // Edit menu
  editMenu->add(L"&Deshacer",		CMD_EDIT_UNDO);
  editMenu->add(L"&Repetir",		CMD_EDIT_REDO);
  editMenu->addSeparator();
  editMenu->add(L"C&ortar",		CMD_EDIT_CUT);
  editMenu->add(L"&Copiar",		CMD_EDIT_COPY);
  editMenu->add(L"&Pegar",		CMD_EDIT_PASTE);

  // View menu
  // viewMenu->add(L"&Actualizar",		CMD_VIEW_REFRESH);

  // // Users menu
  // usersMenu->add(L"&Navegar",		CMD_USERS_NAVIGATE);
  // usersMenu->add(L"&Agregar",		CMD_USERS_ADD);
  // usersMenu->addSeparator();
  // usersMenu->add(L"&Perfil",		CMD_USERS_PROFILE);

  // Tools menu
  toolsMenu->add(researchMenu);
  toolsMenu->addSeparator();
  toolsMenu->add(L"&Opciones",		CMD_TOOLS_OPTIONS);

  // Research menu
  researchMenu->add(L"Abrir &Script",	CMD_RESEARCH_OPEN_SCRIPT);

  // Help menu
  helpMenu->add(L"Acerca de...",	CMD_HELP_ABOUT);

  return menuBar;
}

void MainFrame::onDropFiles(DropFilesEvent& ev)
{
  Frame::onDropFiles(ev);

  std::vector<String> files = ev.getFiles();
  std::vector<String>::iterator it;

  for (it=files.begin(); it!=files.end(); ++it) {
    m_navigator.addImagesToProcess(*it);
  }
}
