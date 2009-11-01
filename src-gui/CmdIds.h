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

#ifndef CMDIDS_H
#define CMDIDS_H

#include <Vaca/Command.h>

using Vaca::CommandId;

const CommandId CMD_FILE_EXIT		 = 100;
const CommandId CMD_DB_CONNECTIONS	 = 200;
const CommandId CMD_DB_IMPORT		 = 201;
const CommandId CMD_DB_EXPORT		 = 202;
const CommandId CMD_EDIT_UNDO		 = 300;
const CommandId CMD_EDIT_REDO		 = 301;
const CommandId CMD_EDIT_CUT		 = 302;
const CommandId CMD_EDIT_COPY		 = 303;
const CommandId CMD_EDIT_PASTE		 = 304;
// const CommandId CMD_VIEW_REFRESH	 = 400;
const CommandId CMD_USERS_ADD		 = 500;
const CommandId CMD_USERS_NAVIGATE	 = 501;
const CommandId CMD_USERS_PROFILE	 = 502;
const CommandId CMD_RESEARCH_OPEN_SCRIPT = 600;
const CommandId CMD_TOOLS_OPTIONS	 = 700;
const CommandId CMD_HELP_ABOUT		 = 800;

#endif
