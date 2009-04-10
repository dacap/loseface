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

#define LUAOBJ_MLPARRAY		"MlpArray"

using namespace std;
using namespace annlib::details;

lua_MlpArray** annlib::details::toMlpArray(lua_State* L, int pos)
{
  return ((lua_MlpArray**)luaL_checkudata(L, pos, LUAOBJ_MLPARRAY));
}

static lua_MlpArray** newmlparray(lua_State* L)
{
  lua_MlpArray** n = (lua_MlpArray**)lua_newuserdata(L, sizeof(lua_MlpArray**));
  *n = new lua_MlpArray;
  luaL_getmetatable(L, LUAOBJ_MLPARRAY);
  lua_setmetatable(L, -2);
  return n;
}

static int mlparray__recall(lua_State* L)
{
  lua_MlpArray** _array = toMlpArray(L, 1);
  if (!_array)
    return 0;

  lua_MlpArray& array(**_array);
  luaL_checktype(L, 2, LUA_TTABLE);

  lua_PatternSet* set = NULL;
  lua_getfield(L, 2, "set");
  if (lua_isuserdata(L, -1)) set = *toPatternSet(L, -1);
  lua_pop(L, 1);

  if (!set)
    return luaL_error(L, "Invalid pattern set specified");

  Vector<double> input, output;

  // Put a table in the stack: array of outputs
  lua_newtable(L);

  lua_PatternSet::iterator it = set->begin();
  lua_PatternSet::iterator end = set->end();
  size_t i = 1;
  for (; it != end; ++it, ++i) {
    // Execute the array of neural networks
    array.recall(it->input, output);

    // A new table in the stack: output vector
    lua_pushinteger(L, i);
    lua_newtable(L);

    // Fill the output vector (converts Vector<double> -> Lua Table)
    for (size_t j=0; j<output.size(); ++j) {
      lua_pushinteger(L, j+1);
      lua_pushnumber(L, output(j));
      lua_settable(L, -3);
    }

    lua_settable(L, -3); // add the output-table in the "array of outputs"-table
  }
  return 1;
}

static int mlparray__gc(lua_State* L)
{
  lua_MlpArray** n = toMlpArray(L, 1);
  if (n) {
    delete *n;
    *n = NULL;
  }
  return 0;
}

static const luaL_Reg mlparray_metatable[] = {
  { "recall",	mlparray__recall },
  { "__gc",	mlparray__gc },
  { NULL, NULL }
};

void annlib::details::registerMlpArray(lua_State* L)
{
  // MlpArray user data
  luaL_newmetatable(L, LUAOBJ_MLPARRAY);	// create metatable for MlpArray
  lua_pushvalue(L, -1);				// push metatable
  lua_setfield(L, -2, "__index");		// metatable.__index = metatable
  luaL_register(L, NULL, mlparray_metatable);	// MlpArray methods
}

int annlib::details::MlpArrayCtor(lua_State* L)
{
  lua_MlpArray* array = *newmlparray(L);
  if (!array)
    return 0;

  luaL_checktype(L, 1, LUA_TTABLE);

  // iterate table
  lua_pushnil(L);		// push nil for first element of table
  while (lua_next(L, 1) != 0) {
    lua_Mlp* mlp = *toMlp(L, -1); // get value
    array->add(*mlp);
    lua_pop(L, 1); // remove value, the key is in stack for next iteration
  }

  return 1; // the array is in the stack
}
