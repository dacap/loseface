// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef LOSEFACE_LUA_ANNLIB_H
#define LOSEFACE_LUA_ANNLIB_H

#include <string>
#include <lua.hpp>

#include "Ann.h"

namespace annlib {

  // Constants
  enum { LAST, BESTMSE };	// learning algorithm goal
  enum { MINMAX, STDDEV };	// type of normalizer

  void registerLibrary(lua_State* L);

  namespace details {

    typedef Mlp<double, Logsig<double>, Logsig<double> > lua_Mlp;

    typedef NetArray<lua_Mlp> lua_MlpArray;

    typedef PatternSet<double> lua_PatternSet;

    struct lua_Normalizer {
      Vector<double> min, max;
    };

    void registerMlp(lua_State* L);
    void registerMlpArray(lua_State* L);
    void registerNormalizer(lua_State* L);
    void registerPatternSet(lua_State* L);

    int MlpCtor(lua_State* L);
    int MlpArrayCtor(lua_State* L);
    int NormalizerCtor(lua_State* L);
    int PatternSetCtor(lua_State* L);

    lua_Mlp** toMlp(lua_State* L, int pos);
    lua_MlpArray** toMlpArray(lua_State* L, int pos);
    lua_Normalizer** toNormalizer(lua_State* L, int pos);
    lua_PatternSet** toPatternSet(lua_State* L, int pos);

  }

}

#endif // LOSEFACE_LUA_ANNLIB_H
