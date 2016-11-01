// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef LOSEFACE_LUA_LUASTATE_H
#define LOSEFACE_LUA_LUASTATE_H

#include <cstdio>
#include <lua.hpp>

namespace lua {

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

}

#endif // LOSEFACE_LUA_LUASTATE_H
