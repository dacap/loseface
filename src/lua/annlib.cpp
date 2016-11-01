// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "lua/annlib.h"
#include "Random.h"

using namespace std;

/// Changes the seed of pseudo-random number generator
///
/// @code
/// ann.init_random(NUMBER)
/// @endcode
///
static int annlib_init_random(lua_State* L)
{
  int seed = 1;
  if (lua_isnumber(L, 1))
    seed = (int)lua_tonumber(L, 1);

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

  lua_pushnumber(L, PURELIN);
  lua_setfield(L, -2, "PURELIN");
  lua_pushnumber(L, LOGSIG);
  lua_setfield(L, -2, "LOGSIG");
  lua_pushnumber(L, TANSIG);
  lua_setfield(L, -2, "TANSIG");

  // Userdatas
  annlib::details::registerMlp(L);
  annlib::details::registerMlpArray(L);
  annlib::details::registerNormalizer(L);
  annlib::details::registerPatternSet(L);
}
