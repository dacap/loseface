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
  enum { LAST, BESTMSE };	    // Learning algorithm goal
  enum { MINMAX, STDDEV };	    // Type of normalizer
  enum { PURELIN, LOGSIG, TANSIG }; // Type of activation function

  void registerLibrary(lua_State* L);

  namespace details {

    typedef Mlp lua_Mlp;
    typedef MlpArray lua_MlpArray;

    typedef PatternSet lua_PatternSet;

    struct lua_Normalizer
    {
      Vector min, max;
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
