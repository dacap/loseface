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
