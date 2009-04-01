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

#include "lua.hpp"
#include "lfs.h"

#include "LuaState.h"
#include "lua_annlib.h"

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

/// Lose Face command line utility.
///
/// Usage: 
/// @code
/// losefase SCRIPT_FILES...
/// @endcode
///
int main(int argc, const char *argv[])
{
#ifdef _WIN32
  // We can modify process priority to avoid killing the CPU
  ::SetPriorityClass(::GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
#endif
  
  try {
    LuaState L;
    luaL_openlibs(L);		     // Register command libraries
    luaopen_lfs(L);		     // Register Lua File System library
    annlib::register_lib(L);	     // Register ANN library

    // Process files specified in the command line
    for (int i=1; i<argc; ++i) {
      if (luaL_dofile(L, argv[i]) != 0)
	std::fprintf(stderr, "%s\n", lua_tostring(L, -1));
    }

    L.done();
  }
  catch (exception& e) {
    std::fprintf(stderr, "%s\n", e.what());
    return 1;
  }
  catch (...) {
    std::fprintf(stderr, "Unknown exception caught\n");
    return 1;
  }
  return 0;
}
