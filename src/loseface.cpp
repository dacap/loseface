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

#include <iostream>
#include <lua.hpp>
#include <lfs.h>

#ifdef _WIN32
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
