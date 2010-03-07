// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

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
    Vector& normalMin = (*n)->min;
    Vector& normalMax = (*n)->max;

    int n = lua_gettop(L);	// number of arguments
    for (int i=2; i<=n; ++i) {
      lua_PatternSet* set = *toPatternSet(L, i); // get argument "i"

      // Here we normalize the other pattern set through the calculate range
      for (lua_PatternSet::iterator it=set->begin(); it!=set->end(); ++it) {
	Pattern* pat = *it;

	for (size_t i=0; i<pat->getInput().size(); ++i) {
	  // Normalize input to [-1;+1] range
	  pat->setInput(i,
			2.0 * ((pat->getInput(i) - normalMin(i)) /
			       (normalMax(i) - normalMin(i))) - 1.0);
	}
      }
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
/// Calculates minimum and maximum values of the set
/// so then patterns are normalized between [-1,1] range.
/// 
/// @code
/// n = ann.Normalizer(set)
/// @endcode
int annlib::details::NormalizerCtor(lua_State* L)
{
  int type = MINMAX;
  lua_PatternSet* set = NULL;

  if (lua_isuserdata(L, 1))
    set = *toPatternSet(L, 1);

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
      Vector normalMin = (*it)->getInput();
      Vector normalMax = (*it)->getInput();

      for (++it; it!=pattern_set.end(); ++it) {
	for (size_t i=0; i<(*it)->getInput().size(); ++i) {
	  if ((*it)->getInput()(i) < normalMin(i))  normalMin(i) = (*it)->getInput()(i);
	  if ((*it)->getInput()(i) > normalMax(i))  normalMax(i) = (*it)->getInput()(i);
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
