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

#include <sstream>

#include "lua/annlib.h"

#define LUAOBJ_PATTERNSET	"PatternSet"

using namespace std;
using namespace annlib::details;

lua_PatternSet** annlib::details::toPatternSet(lua_State* L, int pos)
{
  return ((lua_PatternSet**)luaL_checkudata(L, pos, LUAOBJ_PATTERNSET));
}

static lua_PatternSet** newpatternset(lua_State* L)
{
  lua_PatternSet** p = (lua_PatternSet**)lua_newuserdata(L, sizeof(lua_PatternSet**));
  *p = new lua_PatternSet;
  luaL_getmetatable(L, LUAOBJ_PATTERNSET);
  lua_setmetatable(L, -2);
  return p;
}

static int patternset__clone(lua_State* L)
{
  lua_PatternSet** p = toPatternSet(L, 1);
  if (p) {
    **newpatternset(L) = **p;
    return 1;
  }
  return 0;
}

// TODO add support to save different kind of outputs
static int patternset__save(lua_State* L)
{
  lua_PatternSet** p = toPatternSet(L, 1);
  if (p) {
    const char* filename = luaL_checkstring(L, 2);
    std::ofstream f(filename);

    f.precision(16);

    for (lua_PatternSet::const_iterator
	   it=(*p)->begin(); it!=(*p)->end(); ++it) {
      const Pattern<double>* pat = *it;

      for (size_t i=0; i<pat->input.size(); ++i)
	f << '\t' << pat->input(i);

      if (pat->output.size() > 1)
	f << '\t' << ((int)pat->output.getMaxPos()+1);
      else
	f << '\t' << pat->output(0);

      f << std::endl;
    }
  }
  return 0;
}

static int patternset__add_pattern(lua_State* L)
{
  lua_PatternSet** p = toPatternSet(L, 1);
  if (!p)
    return luaL_error(L, "Invalid pattern set specified");

  luaL_checktype(L, 2, LUA_TTABLE);

  Pattern<double> newPattern;

  lua_getfield(L, 2, "input");
  if (lua_istable(L, -1)) {
    size_t N = lua_objlen(L, -1);
    if (N == 0) {
      lua_pop(L, 1);
      return luaL_error(L, "Invalid 'input' vector with zero length");
    }
    newPattern.input.resize(N);

    // iterate the 'input' table
    lua_pushnil(L);		// push nil for first element of table
    size_t i = 0;
    while (lua_next(L, -2) != 0) {
      newPattern.input(i++) = lua_tonumber(L, -1); // get value
      lua_pop(L, 1);		// remove value, the key is in stack for next iteration
    }
    lua_pop(L, 1);		// pop the 'input' table
  }
  else {
    lua_pop(L, 1);
    return luaL_error(L, "You have to specified the 'input' vector");
  }

  lua_getfield(L, 2, "output");
  if (lua_istable(L, -1)) {
    size_t N = lua_objlen(L, -1);
    if (N == 0) {
      lua_pop(L, 1);
      return luaL_error(L, "Invalid 'output' vector with zero length");
    }
    newPattern.output.resize(N);

    // iterate the 'output' table
    lua_pushnil(L);		// push nil for first element of table
    size_t i = 0;
    while (lua_next(L, -2) != 0) {
      newPattern.output(i++) = lua_tonumber(L, -1); // get value
      lua_pop(L, 1);		// remove value, the key is in stack for next iteration
    }
    lua_pop(L, 1);		// pop the 'output' table
  }
  else {
    lua_pop(L, 1);
    return luaL_error(L, "You have to specified the 'output' vector");
  }

  // add the new pattern
  (*p)->push_back(newPattern);
  return 0;
}

static int patternset__set_output(lua_State* L)
{
  lua_PatternSet** p = toPatternSet(L, 1);
  if (p) {
    luaL_checktype(L, 2, LUA_TTABLE);
    size_t N = lua_objlen(L, 2);
    if (N < 1)
      return luaL_error(L, "Error empty output vector");

    Vector<double> newoutput(N);
    size_t i = 0;

    // iterate table
    lua_pushnil(L);		// push nil for first element of table
    while (lua_next(L, 2) != 0) {
      newoutput(i++) = lua_tointeger(L, -1); // get value
      lua_pop(L, 1);		// remove value, the key is in stack for next iteration
    }

    // Setup all outputs
    for (lua_PatternSet::iterator
	   it=(*p)->begin(); it!=(*p)->end(); ++it) {
      Pattern<double>& pat(**it);
      pat.output = newoutput;
    }
    
    return 0;
  }
  return 0;
}

static int patternset__shuffle(lua_State* L)
{
  lua_PatternSet** p = toPatternSet(L, 1);
  if (p) {
    (*p)->shuffle();
    return 0;
  }
  return 0;
}

/// Creates a set of patterns through the specified list of
/// partitions.
///
/// @code
/// PatternSet:split({ bypercentage={ percentage1, percentage2... } })
/// PatternSet:split({ byoutput={ output1, output2, output3... } })
/// @endcode
static int patternset__split(lua_State* L)
{
  lua_PatternSet** p = toPatternSet(L, 1);
  if (!p)
    return luaL_error(L, "Invalid pattern set specified");

  lua_PatternSet* set = *p;

  // Split by output...
  vector<int> outputs;

  lua_getfield(L, 2, "byoutput");
  if (lua_istable(L, -1)) {
    // iterate the 'byoutput' table
    lua_pushnil(L);		// push nil for first element of table
    while (lua_next(L, -2) != 0) {
      int output_nth = lua_tointeger(L, -1); // get value
      outputs.push_back(output_nth);
      lua_pop(L, 1);		// remove value, the key is in stack for next iteration
    }
    lua_pop(L, 1);		// pop the byoutput table
  }
  else {
    lua_pop(L, 1);
  }

  // Split by percentage...
  vector<double> percentages;

  lua_getfield(L, 2, "bypercentage");
  if (lua_istable(L, -1)) {
    // iterate the 'bypercentage' table
    lua_pushnil(L);		// push nil for first element of table
    while (lua_next(L, -2) != 0) {
      double percentage = lua_tonumber(L, -1); // get value
      percentages.push_back(percentage);
      lua_pop(L, 1);		// remove value, the key is in stack for next iteration
    }
    lua_pop(L, 1);		// pop the bypercentage table
  }
  else {
    lua_pop(L, 1);
  }

  // put in the stack the return value (a new table)
  lua_newtable(L);
  int i = 1;
  for (vector<int>::iterator it = outputs.begin(); it != outputs.end(); ++it, ++i) {
    // a new pattern in the stack
    lua_pushinteger(L, i);
    lua_PatternSet* newset = *newpatternset(L);
    lua_settable(L, -3);

    // output of the pattern
    int output_nth = *it;

    // for each pattern in the original set
    for (int c=0; c<set->size(); ++c) {
      Pattern<double>& pat = (*set)[c];

      // this is a pattern for output_nth
      if (output_nth == pat.output.getMaxPos()+1) // TODO this should be parametric
	newset->push_back(pat);
    }
  }

  int beg = 0, end;
  for (vector<double>::iterator it = percentages.begin(); it != percentages.end(); ++it, ++i) {
    // a new pattern in the stack
    lua_pushinteger(L, i);
    lua_PatternSet* newset = *newpatternset(L);
    lua_settable(L, -3);

    // chunk of the patterns to split
    double percentage = *it;

    // for each pattern in the original set
    end = beg + set->size()*percentage/100.0;
    end = end < set->size() ? end: set->size();
    for (int c=beg; c<end; ++c) {
      Pattern<double>& pat = (*set)[c];
      newset->push_back(pat);
    }
    beg = end;
  }

  return 1;
}

/// Adds to this set of patterns other patterns.
/// 
/// @code
/// set:merge({ set1, set2, set3, ... })
/// @endcode
static int patternset__merge(lua_State* L)
{
  lua_PatternSet** p = toPatternSet(L, 1);
  if (!p)
    return luaL_error(L, "Invalid pattern set specified");

  luaL_checktype(L, 2, LUA_TTABLE);

  // iterate table
  lua_pushnil(L);		// push nil for first element of table
  while (lua_next(L, 2) != 0) {
    lua_PatternSet* set = *toPatternSet(L, -1); // get value

    // Add all patterns of 'set' in 'p'
    for (lua_PatternSet::iterator it=set->begin(); it!=set->end(); ++it)
      (*p)->push_back(**it);

    lua_pop(L, 1);		// remove value, the key is in stack for next iteration
  }

  return 0;
}

static int patternset__gc(lua_State* L)
{
  lua_PatternSet** p = toPatternSet(L, 1);
  if (p) {
    delete *p;
    *p = NULL;
  }
  return 0;
}

static int patternset__len(lua_State* L)
{
  lua_PatternSet** p = toPatternSet(L, 1);
  if (p) {
    lua_pushnumber(L, (*p)->size());
    return 1;
  }
  return 0;
}

static const luaL_Reg patternset_metatable[] = {
  { "clone", patternset__clone },
  { "save", patternset__save },
  { "add_pattern", patternset__add_pattern },
  { "set_output", patternset__set_output },
  { "shuffle", patternset__shuffle },
  { "split", patternset__split },
  { "merge", patternset__merge },
  { "__gc", patternset__gc },
  { "__len", patternset__len },
  { NULL, NULL }
};

void annlib::details::registerPatternSet(lua_State* L)
{
  // PatternSet user data
  luaL_newmetatable(L, LUAOBJ_PATTERNSET);	// create metatable for PatternSet
  lua_pushvalue(L, -1);				// push metatable
  lua_setfield(L, -2, "__index");		// metatable.__index = metatable
  luaL_register(L, NULL, patternset_metatable);	// PatternSet methods
}

/// Creates an empty pattern set or loads it from a file.
///
/// @code
/// set = ann.PatternSet()
/// set = ann.PatternSet({ inputs=NUMBER, outputs=NUMBER, file=FILE })
/// @endcode
///
/// @return Pattern user data
///
int annlib::details::PatternSetCtor(lua_State* L)
{
  string file;
  int inputs = 1, outputs = 1;
  if (lua_istable(L, 1)) {
    lua_getfield(L, 1, "file");
    lua_getfield(L, 1, "inputs");
    lua_getfield(L, 1, "outputs");
    if (lua_isstring(L, -1)) outputs = lua_tonumber(L, -1);
    if (lua_isstring(L, -2)) inputs = lua_tonumber(L, -2);
    if (lua_isstring(L, -3)) file = lua_tostring(L, -3);
    lua_pop(L, 3);
  }

  // Create the new pattern set
  lua_PatternSet* set = *newpatternset(L);
  lua_PatternSet& pattern_set(*set);

  // Return the empty pattern set
  if (file.empty())
    return 1;			// the PatternSet is in the stack

  // Load the file
  ifstream in(file.c_str());
  if (!in.good())
    return luaL_error(L, "Error loading file %s", file.c_str());

  char buf[4096*8]; // TODO this could not be sufficient for one line of text

  // Read line by line
  while (in.getline(buf, sizeof(buf)).good()) {
    // For this line
    istringstream str(buf);

    // Create one pattern (input/target pair)
    Pattern<double> pat(inputs, outputs);

    // Read input
    for (size_t c=0; c<inputs; ++c)
      str >> pat.input(c);

    // Read last digit (target)
    int target = 0;
    str >> target;

    for (size_t c=0; c<outputs; ++c)
      pat.output(c) = (c == target-1) ? 1.0: 0.0;

#if 0			// To see if patterns are loaded correctly
    cout << "Pattern loaded:" << endl;
    cout << "  in  = " << pat.input << endl;
    cout << "  out = " << pat.output << endl;
#endif

    pattern_set.push_back(pat);
  }

  // if (verbose_mode)
  //   cout << pattern_set.size() << " pattern(s) loaded from '"
  // 	 << file << "' file" << endl;

  return 1;			// the PatternSet is in the stack
}
