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

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <sstream>

#include "Ann.h"
#include "lua/annlib.h"

using namespace std;

typedef Mlp<double, Logsig<double>, Logsig<double> > MlpModel;
typedef MlpModel::Set PatternSet;
struct Normalizer {
  Vector<double> min, max;
};

static bool verbose_mode = false;

static double calculate_hits(MlpModel& mlp, PatternSet& pattern_set);

//////////////////////////////////////////////////////////////////////
// "ann" lua library

static const luaL_Reg funcstable[] = {
  { "init_random", annlib::init_random },
  { "create_mlp", annlib::create_mlp },
  { "create_patternset", annlib::create_patternset },
  { "create_normalizer", annlib::create_normalizer },
  { NULL, NULL }
};

//////////////////////////////////////////////////////////////////////
// PatternSet user data

#define LUAOBJ_PATTERNSET	"PatternSet"
#define topatternset(L, pos)	((PatternSet**)luaL_checkudata(L, pos, LUAOBJ_PATTERNSET))

static PatternSet* newpatternset(lua_State* L)
{
  PatternSet** p = (PatternSet**)lua_newuserdata(L, sizeof(PatternSet**));
  *p = new PatternSet;
  luaL_getmetatable(L, LUAOBJ_PATTERNSET);
  lua_setmetatable(L, -2);
  return *p;
}

static int patternset_clone(lua_State* L)
{
  PatternSet** p = topatternset(L, 1);
  if (p) {
    *newpatternset(L) = **p;
    return 1;
  }
  return 0;
}

static int patternset_save(lua_State* L)
{
  PatternSet** p = topatternset(L, 1);
  if (p) {
    const char* filename = luaL_checkstring(L, 2);
    std::ofstream f(filename);

    f.precision(16);

    for (PatternSet::const_iterator
	   it=(*p)->begin(); it!=(*p)->end(); ++it) {
      const Pattern<double>& pat(*it);

      for (size_t i=0; i<pat.input.size(); ++i)
	f << '\t' << pat.input(i);

      if (pat.output.size() > 1)
	f << '\t' << ((int)pat.output.getMaxPos()+1);
      else
	f << '\t' << pat.output(0);

      f << std::endl;
    }
  }
  return 0;
}

static int patternset_set_output(lua_State* L)
{
  PatternSet** p = topatternset(L, 1);
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
    for (PatternSet::iterator
	   it=(*p)->begin(); it!=(*p)->end(); ++it) {
      Pattern<double>& pat(*it);
      pat.output = newoutput;
    }
    
    return 0;
  }
  return 0;
}

static int patternset_shuffle(lua_State* L)
{
  PatternSet** p = topatternset(L, 1);
  if (p) {
    random_shuffle((*p)->begin(), (*p)->end());
    return 0;
  }
  return 0;
}

/// Creates a set of patterns through the specified list of
/// partitions.
///
/// @code
/// set:split({ bypercentage={ percentage1, percentage2... },
///             byoutput={ output1, output2, output3... } })
/// @endcode
static int patternset_split(lua_State* L)
{
  PatternSet** p = topatternset(L, 1);
  if (!p)
    return luaL_error(L, "Invalid pattern set specified");

  PatternSet* set = *p;

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
    PatternSet* newset = newpatternset(L);
    lua_settable(L, -3);

    // output of the pattern
    int output_nth = *it;

    // for each pattern in the original set
    for (int c=0; c<set->size(); ++c) {
      Pattern<double>& pat = (*set)[c];

      // this is a pattern for output_nth
      if (output_nth == pat.output.getMaxPos()+1)
	newset->push_back(pat);
    }
  }

  if (verbose_mode && !percentages.empty())
    cout << "Split by percentages: ";

  int beg = 0, end;
  for (vector<double>::iterator it = percentages.begin(); it != percentages.end(); ++it, ++i) {
    // a new pattern in the stack
    lua_pushinteger(L, i);
    PatternSet* newset = newpatternset(L);
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
    if (verbose_mode)
      cout << percentage << "% [" << beg << ", " << end << "] ";
    beg = end;
  }
  if (verbose_mode && !percentages.empty())
    cout << endl;

  return 1;
}

/// Adds to this set of patterns other patterns.
/// 
/// @code
/// set:merge({ set1, set2, set3, ... })
/// @endcode
static int patternset_merge(lua_State* L)
{
  PatternSet** p = topatternset(L, 1);
  if (!p)
    return luaL_error(L, "Invalid pattern set specified");

  luaL_checktype(L, 2, LUA_TTABLE);
  size_t N = lua_objlen(L, 2);
  if (N < 1)
    return luaL_error(L, "Error empty table of pattern sets");

  // iterate table
  lua_pushnil(L);		// push nil for first element of table
  while (lua_next(L, 2) != 0) {
    PatternSet* set = *topatternset(L, -1); // get value

    // Add all patterns of 'set' in 'p'
    for (PatternSet::iterator it=set->begin(); it!=set->end(); ++it)
      (*p)->push_back(*it);

    lua_pop(L, 1);		// remove value, the key is in stack for next iteration
  }

  return 0;
}

static int patternset_gc(lua_State* L)
{
  PatternSet** p = topatternset(L, 1);
  if (p) {
    delete *p;
    *p = NULL;
  }
  return 0;
}

static int patternset_len(lua_State* L)
{
  PatternSet** p = topatternset(L, 1);
  if (p) {
    lua_pushnumber(L, (*p)->size());
    return 1;
  }
  return 0;
}

static const luaL_Reg patternset_metatable[] = {
  { "clone", patternset_clone },
  { "save", patternset_save },
  { "set_output", patternset_set_output },
  { "shuffle", patternset_shuffle },
  { "split", patternset_split },
  { "merge", patternset_merge },
  { "__gc", patternset_gc },
  { "__len", patternset_len },
  { NULL, NULL }
};

//////////////////////////////////////////////////////////////////////
// Mlp user data

#define LUAOBJ_MLPMODEL		"MlpModel"
#define tomlpmodel(L, pos)	((MlpModel**)luaL_checkudata(L, pos, LUAOBJ_MLPMODEL))

static MlpModel* newmlpmodel(lua_State* L)
{
  MlpModel** p = (MlpModel**)lua_newuserdata(L, sizeof(MlpModel**));
  *p = new MlpModel;
  luaL_getmetatable(L, LUAOBJ_MLPMODEL);
  lua_setmetatable(L, -2);
  return *p;
}

static int mlpmodel_clone(lua_State* L)
{
  MlpModel** mlp = tomlpmodel(L, 1);
  if (mlp) {
    *newmlpmodel(L) = **mlp;
    return 1;
  }
  return 0;
}

/// Initializes weights and bias of the MLP model
///
/// @code
/// mlp:init({ min=NUMBER, max=NUMBER })
/// @endcode
///
static int mlpmodel_init(lua_State* L)
{
  MlpModel** mlp = tomlpmodel(L, 1);
  if (mlp) {
    luaL_checktype(L, 2, LUA_TTABLE);

    double min = 0.0, max = 0.0;
    lua_getfield(L, 2, "min");
    lua_getfield(L, 2, "max");
    if (lua_isnumber(L, -2)) max = lua_tonumber(L, -2);
    if (lua_isnumber(L, -1)) min = lua_tonumber(L, -1);
    lua_pop(L, 2);

    (**mlp).initRandom(min, max);
  }
  return 0;
}

/// Loads a MLP model from the specified file
/// @code
/// mlp:load({ file=FILENAME })
/// @endcode
///
static int mlpmodel_load(lua_State* L)
{
  MlpModel** mlp = tomlpmodel(L, 1);
  if (mlp) {
    luaL_checktype(L, 2, LUA_TTABLE);

    string file;
    lua_getfield(L, 2, "file");
    if (lua_isstring(L, -1)) file = lua_tostring(L, -1);
    lua_pop(L, 1);

    (*mlp)->load(file.c_str());

    // TODO
    // throw ScriptError(string("Error loading MLP from ") + arg);

    if (verbose_mode)
      cout << "MLP ["
	   << (**mlp).getInputs() << " inputs, "
	   << (**mlp).getHiddens() << " hiddens, "
	   << (**mlp).getOutputs() << " outputs] loaded from "
	   << file << endl;
  }
  return 0;
}

/// Saves current MLP model to the specified file
/// 
/// @code
/// mlp:save({ file=FILENAME })
/// @endcode
///
static int mlpmodel_save(lua_State* L)
{
  MlpModel** mlp = tomlpmodel(L, 1);
  if (mlp) {
    luaL_checktype(L, 2, LUA_TTABLE);

    string file;
    lua_getfield(L, 2, "file");
    if (lua_isstring(L, -1)) file = lua_tostring(L, -1);
    lua_pop(L, 1);

    (*mlp)->save(file.c_str());
  }
  return 0;
}

/// Trains the MLP model with a specified set of patterns and by some epochs.
/// 
/// @code
/// mlp:train({ set=PatternSet,
///		learning_rate=LEARNING_RATE,
///		momentum=MOMENTUM
///		epochs=NUMBER,
///		shuffle=NUMBER,
///		goal=ann.LAST|ann.BESTMSE|ann.BESTHIT })
/// @endcode
///
static int mlpmodel_train(lua_State* L)
{
  MlpModel** mlp = tomlpmodel(L, 1);
  if (mlp) {
    luaL_checktype(L, 2, LUA_TTABLE);

    PatternSet* set = NULL;
    int epochs = 1, shuffle = 0;
    int goal = annlib::LAST;
    double learning_rate = 0.6, momentum = 0.4;
    lua_getfield(L, 2, "learning_rate");
    lua_getfield(L, 2, "momentum");
    lua_getfield(L, 2, "goal");
    lua_getfield(L, 2, "set");
    lua_getfield(L, 2, "epochs");
    lua_getfield(L, 2, "shuffle");
    if (lua_isnumber(L, -1)) shuffle = (int)lua_tonumber(L, -1);
    if (lua_isnumber(L, -2)) epochs = (int)lua_tonumber(L, -2);
    if (lua_isuserdata(L, -3)) set = *topatternset(L, -3);
    if (lua_isnumber(L, -4)) goal = (int)lua_tonumber(L, -4);
    if (lua_isnumber(L, -5)) momentum = lua_tonumber(L, -5);
    if (lua_isnumber(L, -6)) learning_rate = lua_tonumber(L, -6);
    lua_pop(L, 6);

    if (!set)
      return luaL_error(L, "Invalid pattern set specified");

    /// Back-propagation algorithm configuration
    BackPropagation<MlpModel> backPropagation(**mlp);
    backPropagation.setLearningRate(learning_rate);
    backPropagation.setMomentum(momentum);
    if (verbose_mode)
      cout << "Back-propagation configured with"
	   << " learning-rate=" << learning_rate
	   << " momentum=" << momentum
	   << endl;
    
    PatternSet& pattern_set(*set);
    MlpModel best;
    double score, bestScore = 0.0;	// Best MSE or hits
    if (goal != annlib::LAST) {
      best = (**mlp);
      switch (goal) {
	case annlib::BESTMSE: bestScore = (**mlp).calcMSE(pattern_set); break;
	case annlib::BESTHIT: bestScore = calculate_hits((**mlp), pattern_set); break;
      }
    }

    // For each training epoch...
    for (int i=0, j=0; i<epochs; ++i, ++j) {
      // Time to shuffle patterns?
      if (j == shuffle-1) {
	random_shuffle(pattern_set.begin(), pattern_set.end());
	j = 0;
      }

      // Train one epoch
      backPropagation.train(pattern_set);

      // What we are looking for? (MLP with best MSE, with best hits, etc.)
      if (goal != annlib::LAST) {
	switch (goal) {

	  case annlib::BESTMSE:
	    score = (**mlp).calcMSE(pattern_set);
	    if (bestScore < score) {
	      best = (**mlp);
	      bestScore = score;
	    }
	    break;

	  case annlib::BESTHIT:
	    score = calculate_hits((**mlp), pattern_set);
	    if (bestScore < score) {
	      best = (**mlp);
	      bestScore = score;
	    }
	    break;
	}
      }
    }

    if (goal != annlib::LAST)
      (**mlp) = best;
  }
  return 0;
}

/// Tests the MLP model
/// 
/// @code
/// mlp:test({ set=PatternSet,
///            criterion=TODO })
/// @endcode
///
static int mlpmodel_test(lua_State* L)
{
  MlpModel** mlp = tomlpmodel(L, 1);
  if (mlp) {
    luaL_checktype(L, 2, LUA_TTABLE);

    PatternSet* set = NULL;
    lua_getfield(L, 2, "set");
    if (lua_isuserdata(L, -1)) set = *topatternset(L, -1);
    lua_pop(L, 1);

    if (!set)
      return luaL_error(L, "Invalid pattern set specified");

    PatternSet& pattern_set(*set);
    double hits = calculate_hits((**mlp), pattern_set);
    lua_pushnumber(L, hits);
    return 1;
  }
  return 0;
}

/// Calculates MSE given a set of patterns.
/// 
/// @code
/// mlpmodel:mse({ set=PatternSet })
/// @endcode
///
static int mlpmodel_mse(lua_State* L)
{
  MlpModel** mlp = tomlpmodel(L, 1);
  if (mlp) {
    luaL_checktype(L, 2, LUA_TTABLE);

    PatternSet* set = NULL;
    lua_getfield(L, 2, "set");
    if (lua_isuserdata(L, -1)) set = *topatternset(L, -1);
    lua_pop(L, 1);

    if (!set)
      return luaL_error(L, "Invalid pattern set specified");

    PatternSet& pattern_set(*set);
    if (pattern_set.empty())
      return luaL_error(L, "Empty pattern set to calculate MSE");

    lua_pushnumber(L, (**mlp).calcMSE(pattern_set));
    return 1;
  }
  return 0;
}

static int mlpmodel_gc(lua_State* L)
{
  MlpModel** m = tomlpmodel(L, 1);
  if (m) {
    delete *m;
    *m = NULL;
  }
  return 0;
}

static const luaL_Reg mlpmodel_metatable[] = {
  { "clone", mlpmodel_clone },
  { "init", mlpmodel_init },
  { "load", mlpmodel_load },
  { "save", mlpmodel_save },
  { "train", mlpmodel_train },
  { "test", mlpmodel_test },
  { "mse", mlpmodel_mse },
  { "__gc", mlpmodel_gc },
  { NULL, NULL }
};

//////////////////////////////////////////////////////////////////////
// Normalizer user data

#define LUAOBJ_NORMALIZER	"Normalizer"
#define tonormalizer(L, pos)	((Normalizer**)luaL_checkudata(L, pos, LUAOBJ_NORMALIZER))

static Normalizer* newnormalizer(lua_State* L)
{
  Normalizer** n = (Normalizer**)lua_newuserdata(L, sizeof(Normalizer**));
  *n = new Normalizer;
  luaL_getmetatable(L, LUAOBJ_NORMALIZER);
  lua_setmetatable(L, -2);
  return *n;
}

static int normalizer_normalize(lua_State* L)
{
  Normalizer** n = tonormalizer(L, 1);
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
      PatternSet* set = *topatternset(L, -1); // get value

      // Here we normalize the other pattern set through the calculate range
      for (PatternSet::iterator it=set->begin(); it!=set->end(); ++it) {
	for (size_t i=0; i<it->input.size(); ++i) {
	  // Normalize input to [-1;+1] range
	  it->input(i) = 2.0 * ((it->input(i) - normalMin(i)) /
				(normalMax(i) - normalMin(i))) - 1.0;
	}
      }

      lua_pop(L, 1);		// remove value, the key is in stack for next iteration
    }

    return 0;
  }
  return 0;
}

static int normalizer_gc(lua_State* L)
{
  Normalizer** n = tonormalizer(L, 1);
  if (n) {
    delete *n;
    *n = NULL;
  }
  return 0;
}

static const luaL_Reg normalizer_metatable[] = {
  { "normalize", normalizer_normalize },
  { "__gc", normalizer_gc },
  { NULL, NULL }
};

//////////////////////////////////////////////////////////////////////

namespace annlib
{

  void register_lib(lua_State* L)
  {
    // Functions
    luaL_register(L, "ann", funcstable);

    // Constants
    lua_pushnumber(L, LAST);
    lua_setfield(L, -2, "LAST");
    lua_pushnumber(L, BESTMSE);
    lua_setfield(L, -2, "BESTMSE");
    lua_pushnumber(L, BESTHIT);
    lua_setfield(L, -2, "BESTHIT");

    lua_pushnumber(L, MINMAX);
    lua_setfield(L, -2, "MINMAX");
    lua_pushnumber(L, STDDEV);
    lua_setfield(L, -2, "STDDEV");
    
    // MlpModel user data
    luaL_newmetatable(L, LUAOBJ_MLPMODEL);	// create metatable for MlpModel
    lua_pushvalue(L, -1);			// push metatable
    lua_setfield(L, -2, "__index");		// metatable.__index = metatable
    luaL_register(L, NULL, mlpmodel_metatable);	// MlpModel methods

    // PatternSet user data
    luaL_newmetatable(L, LUAOBJ_PATTERNSET);	  // create metatable for PatternSet
    lua_pushvalue(L, -1);			  // push metatable
    lua_setfield(L, -2, "__index");		  // metatable.__index = metatable
    luaL_register(L, NULL, patternset_metatable); // PatternSet methods

    // Normalizer user data
    luaL_newmetatable(L, LUAOBJ_NORMALIZER);	// create metatable for Normalizer
    lua_pushvalue(L, -1);			// push metatable
    lua_setfield(L, -2, "__index");		// metatable.__index = metatable
    luaL_register(L, NULL, normalizer_metatable); // Normalizer methods
  }

  /// Changes the seed of pseudo-random number generator
  /// @code
  /// init_random({ seed=NUMBER })
  /// @endcode
  ///
  int init_random(lua_State* L)
  {
    luaL_checktype(L, 1, LUA_TTABLE);

    int seed = 1;
    lua_getfield(L, 1, "seed");
    if (lua_isnumber(L, -1)) seed = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    srand(seed);

    return 0;
  }

  /// Create a new MLP
  /// @code
  /// ann.create_mlp({ inputs=NUMBER, hiddens=NUMBER, outputs=NUMBER })
  /// @endcode
  ///
  int create_mlp(lua_State* L)
  {
    int inputs = 1, hiddens = 1, outputs = 1;
    if (lua_istable(L, 1)) {
      lua_getfield(L, 1, "inputs");
      lua_getfield(L, 1, "hiddens");
      lua_getfield(L, 1, "outputs");
      if (lua_isnumber(L, -3)) inputs = (int)lua_tonumber(L, -3);
      if (lua_isnumber(L, -2)) hiddens = (int)lua_tonumber(L, -2);
      if (lua_isnumber(L, -1)) outputs = (int)lua_tonumber(L, -1);
      lua_pop(L, 3);
    }

    *newmlpmodel(L) = MlpModel(inputs, hiddens, outputs);
    return 1;
  }

  /// Creates an empty pattern set or loads it from a file.
  ///
  /// @code
  /// create_patternset()
  /// create_patternset({ inputs=NUMBER, outputs=NUMBER, file=FILE })
  /// @endcode
  ///
  /// @return Pattern user data
  ///
  int create_patternset(lua_State* L)
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
    PatternSet* set = newpatternset(L);
    PatternSet& pattern_set(*set);

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

    if (verbose_mode)
      cout << pattern_set.size() << " pattern(s) loaded from '"
	   << file << "' file" << endl;

    return 1;			// the PatternSet is in the stack
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
  /// create_normalizer({ set=PatternSet
  ///			  type=ann.MINMAX|ann.STDDEV })
  /// @endcode
  int create_normalizer(lua_State* L)
  {
    int type = MINMAX;
    PatternSet* set = NULL;

    if (lua_istable(L, 1)) {
      lua_getfield(L, 1, "type");
      lua_getfield(L, 1, "set");
      if (lua_isuserdata(L, -1)) set = *topatternset(L, -1);
      if (lua_isnumber(L, -2)) type = lua_tointeger(L, -2);
      lua_pop(L, 2);
    }

    if (!set)
      return luaL_error(L, "Invalid pattern set specified");

    PatternSet& pattern_set(*set);
    if (pattern_set.empty())
      return luaL_error(L, "Empty pattern set specified");

    Normalizer* n = newnormalizer(L);

    switch (type) {

      case MINMAX: {
	// First we have to calculate min-max ranges
	PatternSet::iterator it = pattern_set.begin();
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
  
}

static double calculate_hits(MlpModel& mlp, PatternSet& pattern_set)
{
  int hits = 0;

  for (size_t c=0; c<pattern_set.size(); ++c) {
    Vector<double> hidden, output;
    mlp.recall(pattern_set[c].input, hidden, output);

    if (output.getMaxPos() == pattern_set[c].output.getMaxPos())
      ++hits;
  }

  return (double)hits / (double)pattern_set.size();
}
