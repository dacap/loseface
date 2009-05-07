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

#define LUAOBJ_NORMALIZER	"Normalizer"

using namespace std;
using namespace annlib::details;

lua_Normalizer** annlib::details::toNormalizer(lua_State* L, int pos)
{
  return ((lua_Normalizer**)luaL_checkudata(L, pos, LUAOBJ_NORMALIZER));
}

static lua_Normalizer** newnormalizer(lua_State* L)
{
  lua_Normalizer** n = (lua_Normalizer**)lua_newuserdata(L, sizeof(lua_Normalizer**));
  *n = new lua_Normalizer;
  luaL_getmetatable(L, LUAOBJ_NORMALIZER);
  lua_setmetatable(L, -2);
  return n;
}

static int normalizer__normalize(lua_State* L)
{
  lua_Normalizer** n = toNormalizer(L, 1);
  if (n) {
    Vector<double>& normalMin = (*n)->min;
    Vector<double>& normalMax = (*n)->max;

    luaL_checktype(L, 2, LUA_TTABLE);
    size_t N = lua_objlen(L, 2);
    if (N < 1)
      return luaL_error(L, "Error empty set of patterns");

    // iterate table
    lua_pushnil(L);		// push nil for first element of table
    while (lua_next(L, 2) != 0) {
      lua_PatternSet* set = *toPatternSet(L, -1); // get value

      // Here we normalize the other pattern set through the calculate range
      for (lua_PatternSet::iterator it=set->begin(); it!=set->end(); ++it) {
	Pattern<double>& pat = *it;

	  for (size_t i=0; i<pat.input.size(); ++i) {
	  // Normalize input to [-1;+1] range
	  pat.input(i) = 2.0 * ((pat.input(i) - normalMin(i)) /
				(normalMax(i) - normalMin(i))) - 1.0;
	}
      }

      lua_pop(L, 1);		// remove value, the key is in stack for next iteration
    }

    return 0;
  }
  return 0;
}

static int normalizer__gc(lua_State* L)
{
  lua_Normalizer** n = toNormalizer(L, 1);
  if (n) {
    delete *n;
    *n = NULL;
  }
  return 0;
}

static const luaL_Reg normalizer_metatable[] = {
  { "normalize",	normalizer__normalize },
  { "__gc",		normalizer__gc },
  { NULL, NULL }
};

void annlib::details::registerNormalizer(lua_State* L)
{
  // Normalizer user data
  luaL_newmetatable(L, LUAOBJ_NORMALIZER);	// create metatable for Normalizer
  lua_pushvalue(L, -1);				// push metatable
  lua_setfield(L, -2, "__index");		// metatable.__index = metatable
  luaL_register(L, NULL, normalizer_metatable); // Normalizer methods
}

/// Normalizes a pattern set.
///
/// @param set
///   Set of patterns to be used in the calculation of bounds
///   or statistics.
///
/// @param type
///   Type of normalizer:
///   @li ann.MINMAX
///     Calculates minimum and maximum values of the set
///     so then patterns are normalized between [-1,1] range.
///   @li ann.STDDEV
///     Calculates the mean and standard deviation of the
///     specified pattern set so then the normalization makes
///     sets to have a mean of zero and a standard deviation
///     of one.
/// 
/// @code
/// n = ann.Normalizer({ set=PatternSet
///			 type=ann.MINMAX|ann.STDDEV })
/// @endcode
int annlib::details::NormalizerCtor(lua_State* L)
{
  int type = MINMAX;
  lua_PatternSet* set = NULL;

  if (lua_istable(L, 1)) {
    lua_getfield(L, 1, "type");
    lua_getfield(L, 1, "set");
    if (lua_isuserdata(L, -1)) set = *toPatternSet(L, -1);
    if (lua_isnumber(L, -2)) type = lua_tointeger(L, -2);
    lua_pop(L, 2);
  }

  if (!set)
    return luaL_error(L, "Invalid pattern set specified");

  lua_PatternSet& pattern_set(*set);
  if (pattern_set.empty())
    return luaL_error(L, "Empty pattern set specified");

  lua_Normalizer* n = *newnormalizer(L);

  switch (type) {

    case MINMAX: {
      // First we have to calculate min-max ranges
      lua_PatternSet::iterator it = pattern_set.begin();
      Vector<double> normalMin = it->input;
      Vector<double> normalMax = it->input;

      for (++it; it!=pattern_set.end(); ++it) {
	for (size_t i=0; i<it->input.size(); ++i) {
	  if (it->input(i) < normalMin(i)) normalMin(i) = it->input(i);
	  if (it->input(i) > normalMax(i)) normalMax(i) = it->input(i);
	}
      }

      n->min = normalMin;
      n->max = normalMax;
      break;
    }

    case STDDEV: {
      // TODO
      break;
    }

  }

  return 1; // one element in stack, the normalizer
}
