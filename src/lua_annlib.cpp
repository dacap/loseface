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

#include "lua.hpp"
#include "Ann.h"
#include "util.h"
#include "lua_annlib.h"

using namespace std;

typedef Mlp<double, Logsig<double>, Logsig<double> > MlpModel;
typedef MlpModel::Set PatternSet;

/// Current MLP model
static MlpModel g_mlpModel(1, 1, 1);

/// Current back-propagation algorithm configuration
static StdBp<MlpModel> g_backPropagation(g_mlpModel);

static bool verbose_mode = false;

static double calculate_hits(MlpModel& mlp, PatternSet& pattern_set);

//////////////////////////////////////////////////////////////////////
// "ann" lua library

static const luaL_Reg funcstable[] = {
  { "init_random", annlib::init_random },
  { "create_mlp", annlib::create_mlp },
  { "init_mlp", annlib::init_mlp },
  { "load_mlp", annlib::load_mlp },
  { "save_mlp", annlib::save_mlp },
  { "load_patterns", annlib::load_patterns },
  { "init_bp", annlib::init_bp },
  { "train_mlp", annlib::train_mlp },
  { "test_mlp", annlib::test_mlp },
  { "mlp_mse", annlib::mlp_mse },
  { "normalize_patterns", annlib::normalize_patterns },
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

static int patternset_gc(lua_State* L)
{
  PatternSet** p = topatternset(L, 1);
  if (p) {
    delete *p;
    *p = NULL;
  }
  return 0;
}

static const luaL_Reg pattern_metatable[] = {
  { "clone", patternset_clone },
  { "__gc", patternset_gc },
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

    // PatternSet user data
    luaL_newmetatable(L, LUAOBJ_PATTERNSET);	// create metatable for PatternSet
    lua_pushvalue(L, -1);			// push metatable
    lua_setfield(L, -2, "__index");		// metatable.__index = metatable
    luaL_register(L, NULL, pattern_metatable);	// Pattern methods
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
  /// ann.create_mlp({ input=NUMBER, hidden=NUMBER, output=NUMBER })
  /// @endcode
  ///
  int create_mlp(lua_State* L)
  {
    luaL_checktype(L, 1, LUA_TTABLE);

    int input = 1, hidden = 1, output = 1;
    lua_getfield(L, 1, "input");
    lua_getfield(L, 1, "hidden");
    lua_getfield(L, 1, "output");
    if (lua_isnumber(L, -3)) input = (int)lua_tonumber(L, -3);
    if (lua_isnumber(L, -2)) hidden = (int)lua_tonumber(L, -2);
    if (lua_isnumber(L, -1)) output = (int)lua_tonumber(L, -1);
    lua_pop(L, 3);

    g_mlpModel = MlpModel(input, hidden, output);
    return 0;
  }

  /// Initializes weights and bias of the MLP model
  /// @code
  /// ann.init_mlp({ min=NUMBER, max=NUMBER })
  /// @endcode
  ///
  int init_mlp(lua_State* L)
  {
    luaL_checktype(L, 1, LUA_TTABLE);

    double min = 0.0, max = 0.0;
    lua_getfield(L, 1, "min");
    lua_getfield(L, 1, "max");
    if (lua_isnumber(L, -2)) max = lua_tonumber(L, -2);
    if (lua_isnumber(L, -1)) min = lua_tonumber(L, -1);
    lua_pop(L, 2);

    g_mlpModel.initRandom(min, max);
    return 0;
  }

  /// Loads a MLP model from the specified file
  /// @code
  /// load_mlp({ file=FILE })
  /// @endcode
  ///
  int load_mlp(lua_State* L)
  {
    luaL_checktype(L, 1, LUA_TTABLE);

    string file;
    lua_getfield(L, 1, "file");
    if (lua_isstring(L, -1)) file = lua_tostring(L, -1);
    lua_pop(L, 1);

    g_mlpModel.load(file.c_str());
    // TODO
    // throw ScriptError(string("Error loading MLP from ") + arg);

    if (verbose_mode)
      cout << "MLP ["
	   << g_mlpModel.getInputs() << " inputs, "
	   << g_mlpModel.getHiddens() << " hiddens, "
	   << g_mlpModel.getOutputs() << " outputs] loaded from "
	   << file << endl;

    return 0;
  }

  /// Saves current MLP model to the specified file
  /// 
  /// @code
  /// save_mlp({ file=FILENAME })
  /// @endcode
  ///
  int save_mlp(lua_State* L)
  {
    luaL_checktype(L, 1, LUA_TTABLE);

    string file;
    lua_getfield(L, 1, "file");
    if (lua_isstring(L, -1)) file = lua_tostring(L, -1);
    lua_pop(L, 1);

    g_mlpModel.save(file.c_str());

    return 0;
  }

  /// Loads a set of patterns from a file.
  ///
  /// @code
  /// load_patterns({ file=FILE })
  /// @endcode
  ///
  /// @return Pattern user data
  ///
  int load_patterns(lua_State* L)
  {
    luaL_checktype(L, 1, LUA_TTABLE);

    string file;
    lua_getfield(L, 1, "file");
    if (lua_isstring(L, -1)) file = lua_tostring(L, -1);
    lua_pop(L, 1);

    // Create the new pattern set
    PatternSet* set = newpatternset(L);
    PatternSet& pattern_set(*set);

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
      Pattern<double> pat;
      pat.input = g_mlpModel.createInput();
      pat.output = g_mlpModel.createOutput();

      // Read input
      for (size_t c=0; c<pat.input.size(); ++c)
	str >> pat.input(c);

      // Read last digit (target)
      int target = 0;
      str >> target;

      for (size_t c=0; c<pat.output.size(); ++c)
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

  /// Configures the Back-propagation training algorithm.
  /// 
  /// @code
  /// init_bp({ learning_rate=LEARNING_RATE, momentum=MOMENTUM })
  /// @endcode
  ///
  int init_bp(lua_State* L)
  {
    luaL_checktype(L, 1, LUA_TTABLE);

    double learning_rate = 0.6, momentum = 0.4;
    lua_getfield(L, 1, "learning_rate");
    lua_getfield(L, 1, "momentum");
    if (lua_isnumber(L, -1)) momentum = lua_tonumber(L, -1);
    if (lua_isnumber(L, -2)) learning_rate = lua_tonumber(L, -2);
    lua_pop(L, 2);

    g_backPropagation.setLearningRate(learning_rate);
    g_backPropagation.setMomentum(momentum);

    if (verbose_mode)
      cout << "Back-propagation configured with"
	   << " learning-rate=" << learning_rate
	   << " momentum=" << momentum
	   << endl;

    return 0;
  }

  /// Trains the MLP model with a specified set of patterns and by some epochs.
  /// 
  /// @code
  /// train_mlp({ set=PatternSet,
  ///		  epochs=NUMBER,
  ///		  shuffle=NUMBER,
  ///		  target=ann.LAST|ann.BESTMSE|ann.BESTHIT })
  /// @endcode
  ///
  int train_mlp(lua_State* L)
  {
    luaL_checktype(L, 1, LUA_TTABLE);

    PatternSet* set = NULL;
    int epochs = 1, shuffle = 0;
    int target = LAST;
    lua_getfield(L, 1, "target");
    lua_getfield(L, 1, "set");
    lua_getfield(L, 1, "epochs");
    lua_getfield(L, 1, "shuffle");
    if (lua_isnumber(L, -1)) shuffle = (int)lua_tonumber(L, -1);
    if (lua_isnumber(L, -2)) epochs = (int)lua_tonumber(L, -2);
    if (lua_isuserdata(L, -3)) set = *topatternset(L, -3);
    if (lua_isnumber(L, -4)) target = (int)lua_tonumber(L, -4);
    lua_pop(L, 4);

    if (!set)
      return luaL_error(L, "Invalid pattern set specified");

    PatternSet& pattern_set(*set);
    MlpModel best;
    double score, bestScore = 0.0;	// Best MSE or hits
    if (target != LAST) {
      best = g_mlpModel;
      switch (target) {
	case BESTMSE: bestScore = g_mlpModel.calcMSE(pattern_set); break;
	case BESTHIT: bestScore = calculate_hits(g_mlpModel, pattern_set); break;
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
      g_backPropagation.train(pattern_set);

      // What we are looking for? (MLP with best MSE, with best hits, etc.)
      if (target != LAST) {
	switch (target) {

	  case BESTMSE:
	    score = g_mlpModel.calcMSE(pattern_set);
	    if (bestScore < score) {
	      best = g_mlpModel;
	      bestScore = score;
	    }
	    break;

	  case BESTHIT:
	    score = calculate_hits(g_mlpModel, pattern_set);
	    if (bestScore < score) {
	      best = g_mlpModel;
	      bestScore = score;
	    }
	    break;
	}
      }
    }

    if (target != LAST)
      g_mlpModel = best;

    return 0;
  }

  /// Tests the MLP model
  /// 
  /// @code
  /// test_mlp({ set=PatternSet })
  /// @endcode
  ///
  int test_mlp(lua_State* L)
  {
    luaL_checktype(L, 1, LUA_TTABLE);

    PatternSet* set = NULL;
    lua_getfield(L, 1, "set");
    if (lua_isuserdata(L, -1)) set = *topatternset(L, -1);
    lua_pop(L, 1);

    if (!set)
      return luaL_error(L, "Invalid pattern set specified");

    PatternSet& pattern_set(*set);
    double hits = calculate_hits(g_mlpModel, pattern_set);
    lua_pushnumber(L, hits);
    return 1;
  }

  /// Calculates MSE given a set of patterns.
  /// 
  /// @code
  /// mlp_mse({ set=PatternSet })
  /// @endcode
  ///
  int mlp_mse(lua_State* L)
  {
    luaL_checktype(L, 1, LUA_TTABLE);

    PatternSet* set = NULL;
    lua_getfield(L, 1, "set");
    if (lua_isuserdata(L, -1)) set = *topatternset(L, -1);
    lua_pop(L, 1);

    if (!set)
      return luaL_error(L, "Invalid pattern set specified");

    PatternSet& pattern_set(*set);
    if (pattern_set.empty())
      return luaL_error(L, "Empty pattern set to calculate MSE");

    lua_pushnumber(L, g_mlpModel.calcMSE(pattern_set));
    return 1;
  }

  /// @code
  /// normalize_patterns({ range=PatternSet,
  ///			   normalize=PatternSet,
  ///			   min=NUMBER,
  ///			   max=NUMBER })
  /// @endcode
  int normalize_patterns(lua_State* L)
  {
    luaL_checktype(L, 1, LUA_TTABLE);

    PatternSet* range = NULL;
    PatternSet* normalize = NULL;
    double min = -1.0, max = 1.0;
    lua_getfield(L, 1, "min");
    lua_getfield(L, 1, "max");
    lua_getfield(L, 1, "range");
    lua_getfield(L, 1, "normalize");
    if (lua_isuserdata(L, -1)) normalize = *topatternset(L, -1);
    if (lua_isuserdata(L, -2)) range = *topatternset(L, -2);
    if (lua_isnumber(L, -3)) max = lua_tonumber(L, -3);
    if (lua_isnumber(L, -4)) min = lua_tonumber(L, -4);
    lua_pop(L, 4);

    if (!range || !normalize)
      return luaL_error(L, "Invalid pattern set specified");

    PatternSet& range_set(*range);
    PatternSet& normalize_set(*normalize);

    if (range_set.empty())
      return luaL_error(L, "Empty 'range' pattern set specified");

    // First we have to calculate min-max ranges
    PatternSet::iterator it = range_set.begin();
    Vector<double> normalMin = it->input;
    Vector<double> normalMax = it->input;

    for (++it; it!=range_set.end(); ++it) {
      for (size_t i=0; i<it->input.size(); ++i) {
	if (it->input(i) < normalMin(i)) normalMin(i) = it->input(i);
	if (it->input(i) > normalMax(i)) normalMax(i) = it->input(i);
      }
    }

    // Here we normalize the other pattern set through the calculate range
    for (it=normalize_set.begin(); it!=normalize_set.end(); ++it) {
      for (size_t i=0; i<it->input.size(); ++i) {
	it->input(i) = (max-min) * ((it->input(i) - normalMin(i)) /
				    (normalMax(i) - normalMin(i))) + min;
      }
    }

    return 0;
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
