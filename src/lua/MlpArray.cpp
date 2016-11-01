// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

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

/// Loads a neural network from the specified file.
/// @code
/// net:load({ file=FILENAME })
/// @endcode
///
static int mlparray__load(lua_State* L)
{
  lua_MlpArray** net = toMlpArray(L, 1);
  if (net) {
    string file;

    if (lua_isstring(L, 2))
      file = lua_tostring(L, 2);
    else
      return luaL_error(L, "Invalid argument in MlpArray:load function (string expected).");

    (*net)->load(file.c_str());

    // TODO
    // throw ScriptError(string("Error loading neural network from ") + arg);
  }
  return 0;
}

/// Saves the network to the specified file.
///
/// @code
/// net:save(FILENAME)
/// @endcode
///
static int mlparray__save(lua_State* L)
{
  lua_MlpArray** net = toMlpArray(L, 1);
  if (net) {
    string file;

    if (lua_isstring(L, 2))
      file = lua_tostring(L, 2);
    else
      return luaL_error(L, "Invalid argument in MlpArray:save function (string expected).");

    (*net)->save(file.c_str());
  }
  return 0;
}

static int mlparray__recall(lua_State* L)
{
  lua_MlpArray** _array = toMlpArray(L, 1);
  if (!_array)
    return 0;

  lua_MlpArray& array(**_array);

  lua_PatternSet* set = NULL;
  if (lua_isuserdata(L, 2))
    set = *toPatternSet(L, -1);

  if (!set)
    return luaL_error(L, "Invalid pattern set specified");

  Vector input, output;

  // Put a table in the stack: array of outputs
  lua_newtable(L);

  lua_PatternSet::iterator it = set->begin();
  lua_PatternSet::iterator end = set->end();
  size_t i = 1;
  for (; it != end; ++it, ++i) {
    // Execute the array of neural networks
    array.recall((*it)->getInput(), output);

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
  { "load",	mlparray__load },
  { "save",	mlparray__save },
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
