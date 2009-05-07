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

#include "lua/annlib.h"
#include "Random.h"

using namespace std;

/// Changes the seed of pseudo-random number generator
/// 
/// @code
/// ann.init_random({ seed=NUMBER })
/// @endcode
///
static int annlib_init_random(lua_State* L)
{
  luaL_checktype(L, 1, LUA_TTABLE);

  int seed = 1;
  lua_getfield(L, 1, "seed");
  if (lua_isnumber(L, -1)) seed = (int)lua_tonumber(L, -1);
  lua_pop(L, 1);

  srand(seed);
  Random::init(seed);

  return 0;
}

static const luaL_Reg annlib_funcstable[] = {
  { "init_random",	annlib_init_random },
  { "Mlp",		annlib::details::MlpCtor },
  { "MlpArray",		annlib::details::MlpArrayCtor },
  { "PatternSet",	annlib::details::PatternSetCtor },
  { "Normalizer",	annlib::details::NormalizerCtor },
  { NULL,		NULL }
};

/// Register functions and constants of annlib in the specified Lua state.
///
void annlib::registerLibrary(lua_State* L)
{
  // Functions
  luaL_register(L, "ann", annlib_funcstable);

  // Constants
  lua_pushnumber(L, LAST);
  lua_setfield(L, -2, "LAST");
  lua_pushnumber(L, BESTMSE);
  lua_setfield(L, -2, "BESTMSE");

  lua_pushnumber(L, MINMAX);
  lua_setfield(L, -2, "MINMAX");
  lua_pushnumber(L, STDDEV);
  lua_setfield(L, -2, "STDDEV");

  annlib::details::registerMlp(L);
  annlib::details::registerMlpArray(L);
  annlib::details::registerNormalizer(L);
  annlib::details::registerPatternSet(L);
}
