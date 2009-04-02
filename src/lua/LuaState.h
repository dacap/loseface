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

#ifndef LOSEFACE_LUA_LUASTATE_H
#define LOSEFACE_LUA_LUASTATE_H

#include "lua.hpp"

#include <cstdio>

class LuaState
{
  lua_State* L;
  bool m_done;
public:
  LuaState() {
    L = lua_open();
    m_done = false;
  }
  ~LuaState() {
#if 0 				// this does not work yet
    if (!m_done) {
      lua_getglobal(L, "debug");
      lua_getfield(L, -1, "traceback");
      lua_remove(L, -2);	// remove 'debug'
      lua_pcall(L, 0, 0, -1);	// call debug.traceback

      // Get the result string
      if (lua_isstring(L, -1))
	std::fprintf(stderr, "%s\n", lua_tostring(L, -1));

      lua_pop(L, 1);		// pop the result and 'debug' global
    }
#endif
    lua_close(L);
  }
  void done() {
    m_done = true;
  }
  operator lua_State*() {
    return L;
  }
};

#endif // LOSEFACE_LUA_LUASTATE_H
