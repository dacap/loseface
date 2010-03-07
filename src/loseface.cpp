// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include <iostream>
#include <lua.hpp>
#include <lfs.h>

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#endif

#include "lua/LuaState.h"
#include "lua/annlib.h"
#include "lua/imglib.h"

using namespace std;

/// Lose Face command line utility.
///
/// Usage: 
/// @code
/// losefase [SCRIPT_FILE [ARGUMENTS...]]
/// @endcode
///
int main(int argc, const char *argv[])
{
#ifdef _WIN32
  // We can modify process priority to avoid killing the CPU
  ::SetPriorityClass(::GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
#endif
  
  try {
    lua::LuaState L;
    luaL_openlibs(L);		     // Register command libraries
    luaopen_lfs(L);		     // Register Lua File System library

    imglib::registerLibrary(L);	     // Register Image library
    annlib::registerLibrary(L);	     // Register Artificial Neural Network library

    // Process file specified in the command line
    if (argc > 1) {
      lua_createtable(L, argc-2, 0);
      for (int i=2; i<argc; ++i) {
	lua_pushstring(L, argv[i]);
	lua_rawseti(L, -2, 1+i-2);
      }
      lua_setglobal(L, "arg");

      if (luaL_dofile(L, argv[1]) != 0)
	std::fprintf(stderr, "%s\n", lua_tostring(L, -1));
    }
    else {
      cerr << "You have to specify a script to execute" << endl;
    }

    L.done();
  }
  catch (exception& e) {
    cerr << e.what() << endl;
    return 1;
  }
  catch (...) {
    cerr << "Unknown exception caught" << endl;
    return 1;
  }
  return 0;
}
