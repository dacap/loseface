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

#define LUAOBJ_MLP		"Mlp"

using namespace std;
using namespace annlib::details;

static double calculate_hits(lua_Mlp& net, lua_PatternSet& pattern_set);

lua_Mlp** annlib::details::toMlp(lua_State* L, int pos)
{
  return ((lua_Mlp**)luaL_checkudata(L, pos, LUAOBJ_MLP));
}

static lua_Mlp** newmlp(lua_State* L)
{
  lua_Mlp** p = (lua_Mlp**)lua_newuserdata(L, sizeof(lua_Mlp**));
  *p = new lua_Mlp;
  luaL_getmetatable(L, LUAOBJ_MLP);
  lua_setmetatable(L, -2);
  return p;
}

static int mlp__clone(lua_State* L)
{
  lua_Mlp** net = toMlp(L, 1);
  if (net) {
    **newmlp(L) = **net;
    return 1;
  }
  return 0;
}

/// Initializes weights and bias of the artificial neural network.
///
/// @code
/// net:init({ min=NUMBER, max=NUMBER })
/// @endcode
///
static int mlp__init(lua_State* L)
{
  lua_Mlp** net = toMlp(L, 1);
  if (net) {
    luaL_checktype(L, 2, LUA_TTABLE);

    double min = 0.0, max = 0.0;
    lua_getfield(L, 2, "min");
    lua_getfield(L, 2, "max");
    if (lua_isnumber(L, -2)) max = lua_tonumber(L, -2);
    if (lua_isnumber(L, -1)) min = lua_tonumber(L, -1);
    lua_pop(L, 2);

    (*net)->initRandom(min, max);
  }
  return 0;
}

/// Loads a neural network from the specified file.
/// @code
/// net:load({ file=FILENAME })
/// @endcode
///
static int mlp__load(lua_State* L)
{
  lua_Mlp** net = toMlp(L, 1);
  if (net) {
    luaL_checktype(L, 2, LUA_TTABLE);

    string file;
    lua_getfield(L, 2, "file");
    if (lua_isstring(L, -1)) file = lua_tostring(L, -1);
    lua_pop(L, 1);

    (*net)->load(file.c_str());

    // TODO
    // throw ScriptError(string("Error loading neural network from ") + arg);
  }
  return 0;
}

/// Saves the network to the specified file.
/// 
/// @code
/// net:save({ file=FILENAME })
/// @endcode
///
static int mlp__save(lua_State* L)
{
  lua_Mlp** net = toMlp(L, 1);
  if (net) {
    luaL_checktype(L, 2, LUA_TTABLE);

    string file;
    lua_getfield(L, 2, "file");
    if (lua_isstring(L, -1)) file = lua_tostring(L, -1);
    lua_pop(L, 1);

    (*net)->save(file.c_str());
  }
  return 0;
}

/// Trains the network with a specified set of patterns and by some epochs.
///
/// @code
/// net:train({ set=PatternSet,
///		learning_rate=LEARNING_RATE,
///		momentum=MOMENTUM,
///		epochs=NUMBER,
///		shuffle=NUMBER,
///		goal=ann.LAST|ann.BESTMSE,
///		goal_mse=NUMBER,
///		early_stopping={ set=PatternSet, iterations=NUMBER } })
/// @endcode
///
/// This routine can be used to train the network with some variants:
/// @li goal=ann.LAST: Keeps the last network after the training.
/// @li goal=ann.BESTMSE: Keeps the network with best MSE after the training.
///     Anyway if goal_mse is > 0 then the last network has the best MSE.
/// @li @a epochs > 0: Trains a fixed number of epochs.
/// @li Without @a epochs parameter and @a goal_mse > 0: Trains the necessary
///     number of epochs to reach the given MSE level.
/// @li With @a early_stopping: The early stopping technique is used to
///     stop the training if in the given @a iterations number of epochs
///     the MSE of the given validation set is getting worse.
/// @li shuffle > 0: Shuffles the patterns every @a shuffle number of epochs.
///
/// @return Returns how many epochs the net was trained
///
static int mlp__train(lua_State* L)
{
  lua_Mlp** _net = toMlp(L, 1);
  if (!_net)
    return 0;

  lua_Mlp& net(**_net);
  luaL_checktype(L, 2, LUA_TTABLE);

  lua_PatternSet* set = NULL;
  lua_PatternSet* early_stopping_set = NULL;
  int early_stopping_iterations = 5;
  int epochs = 0;
  int shuffle = 0;
  int goal = annlib::LAST;
  double learning_rate = 0.6, momentum = 0.4;
  double goal_mse = -1;
  lua_getfield(L, 2, "goal_mse");
  lua_getfield(L, 2, "early_stopping");
  lua_getfield(L, 2, "learning_rate");
  lua_getfield(L, 2, "momentum");
  lua_getfield(L, 2, "goal");
  lua_getfield(L, 2, "set");
  lua_getfield(L, 2, "epochs");
  lua_getfield(L, 2, "shuffle");
  if (lua_isnumber(L, -1)) shuffle = (int)lua_tonumber(L, -1);
  if (lua_isnumber(L, -2)) epochs = (int)lua_tonumber(L, -2);
  if (lua_isuserdata(L, -3)) set = *toPatternSet(L, -3);
  if (lua_isnumber(L, -4)) goal = (int)lua_tonumber(L, -4);
  if (lua_isnumber(L, -5)) momentum = lua_tonumber(L, -5);
  if (lua_isnumber(L, -6)) learning_rate = lua_tonumber(L, -6);
  if (lua_istable(L, -7)) {
    lua_getfield(L, -7, "set");
    if (lua_isuserdata(L, -1)) early_stopping_set = *toPatternSet(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -7, "iterations");
    if (lua_isnumber(L, -1)) early_stopping_iterations = lua_tointeger(L, -1);
    lua_pop(L, 1);
  }
  if (lua_isnumber(L, -8)) {
    goal_mse = lua_tonumber(L, -8);
  }
  else if (epochs == 0)
    epochs = 1;
  lua_pop(L, 8);

  if (!set)
    return luaL_error(L, "Invalid pattern set specified");

  /// Backpropagation algorithm configuration
  Backpropagation<lua_Mlp, NoAdaptativeLearningRate, UpdateWeightsImmediatelyWithMomentum> bp(net);
  // Backpropagation<lua_Mlp, BoldDriverMethod, UpdateWeightsImmediatelyWithMomentum> bp(net);
  bp.setLearningRate(learning_rate);
  bp.setMomentum(momentum);

  lua_PatternSet& pattern_set(*set);
  lua_Mlp best;
  double mse = net.calcMSE(pattern_set);
  double measure, bestMeasure = 0.0;	// Best MSE
  if (goal != annlib::LAST) {
    best = net;
    switch (goal) {
      case annlib::BESTMSE:
	bestMeasure = mse;
	break;
    }
  }

  // Early stopping
  double early_stopping_mse = 1.0;
  int early_stopping_bad_iterations = 0;
  if (early_stopping_set)
    early_stopping_mse = net.calcMSE(*early_stopping_set);

  // For each training epoch...
  int trained_epochs = 0;
  for (int i=0, j=0; epochs == 0 || i < epochs; ++i, ++j) {
    // Time to shuffle patterns?
    if (shuffle > 0 && j == shuffle-1) {
      pattern_set.shuffle();
      j = 0;
    }

    // Train one epoch
    bp.train(pattern_set);
    trained_epochs++;

    // Recalculate MSE
    mse = net.calcMSE(pattern_set);

    // What we are looking for? (network with best MSE, etc.)
    if (goal != annlib::LAST) {
      switch (goal) {

	case annlib::BESTMSE:
	  measure = mse;
	  if (bestMeasure > measure) { // this is error: less is better
	    best = net;
	    bestMeasure = measure;
	  }
	  break;

      }
    }

    // MSE goal?
    if (goal_mse > -.5 && mse < goal_mse)
      break;

    // Early stopping rules
    if (early_stopping_set) {
      double mse2 = net.calcMSE(*early_stopping_set);

      if (mse2 > early_stopping_mse) {
	early_stopping_bad_iterations++;
	if (early_stopping_bad_iterations >= early_stopping_iterations)
	  break;
      }
      else
	early_stopping_bad_iterations = 0;

      early_stopping_mse = mse2;
    }
  }

  if (goal != annlib::LAST)
    net = best;

  lua_pushnumber(L, trained_epochs);
  return 1;
}

/// Tests the network model
/// 
/// @code
/// net:test({ set=PatternSet,
///            criterion=TODO })
/// @endcode
///
static int mlp__test(lua_State* L)
{
  lua_Mlp** _net = toMlp(L, 1);
  if (_net) {
    lua_Mlp& net(**_net);

    luaL_checktype(L, 2, LUA_TTABLE);

    lua_PatternSet* set = NULL;
    lua_getfield(L, 2, "set");
    if (lua_isuserdata(L, -1)) set = *toPatternSet(L, -1);
    lua_pop(L, 1);

    if (!set)
      return luaL_error(L, "Invalid pattern set specified");

    lua_PatternSet& pattern_set(*set);
    double hits = calculate_hits(net, pattern_set);
    lua_pushnumber(L, hits);
    return 1;
  }
  return 0;
}

/// Calculates MSE given a set of patterns.
/// 
/// @code
/// net:mse({ set=PatternSet })
/// @endcode
///
static int mlp__mse(lua_State* L)
{
  lua_Mlp** net = toMlp(L, 1);
  if (net) {
    luaL_checktype(L, 2, LUA_TTABLE);

    lua_PatternSet* set = NULL;
    lua_getfield(L, 2, "set");
    if (lua_isuserdata(L, -1)) set = *toPatternSet(L, -1);
    lua_pop(L, 1);

    if (!set)
      return luaL_error(L, "Invalid pattern set specified");

    lua_PatternSet& pattern_set(*set);
    if (pattern_set.empty())
      return luaL_error(L, "Empty pattern set to calculate MSE");

    lua_pushnumber(L, (*net)->calcMSE(pattern_set));
    return 1;
  }
  return 0;
}

/// @code
/// { output_table1, output_table2, ... } = net:recall({ set=PatternSet })
/// @endcode
///
static int mlp__recall(lua_State* L)
{
  lua_Mlp** _net = toMlp(L, 1);
  if (!_net)
    return 0;

  lua_Mlp& net(**_net);
  luaL_checktype(L, 2, LUA_TTABLE);

  lua_PatternSet* set = NULL;
  lua_getfield(L, 2, "set");
  if (lua_isuserdata(L, -1)) set = *toPatternSet(L, -1);
  lua_pop(L, 1);

  if (!set)
    return luaL_error(L, "Invalid pattern set specified");

  Vector<double> input, hidden, output;

  // Put a table in the stack: array of outputs
  lua_newtable(L);

  lua_PatternSet::iterator it = set->begin();
  lua_PatternSet::iterator end = set->end();
  size_t i = 1;
  for (; it != end; ++it, ++i) {
    // Execute the neural network
    net.recall((*it)->input, hidden, output);

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

static int mlp__gc(lua_State* L)
{
  lua_Mlp** m = toMlp(L, 1);
  if (m) {
    delete *m;
    *m = NULL;
  }
  return 0;
}

static const luaL_Reg mlp_metatable[] = {
  { "clone", mlp__clone },
  { "init", mlp__init },
  { "load", mlp__load },
  { "save", mlp__save },
  { "train", mlp__train },
  { "test", mlp__test },
  { "mse", mlp__mse },
  { "recall", mlp__recall },
  { "__gc", mlp__gc },
  { NULL, NULL }
};

void annlib::details::registerMlp(lua_State* L)
{
  // Mlp user data
  luaL_newmetatable(L, LUAOBJ_MLP);		// create metatable for Mlp
  lua_pushvalue(L, -1);				// push metatable
  lua_setfield(L, -2, "__index");		// metatable.__index = metatable
  luaL_register(L, NULL, mlp_metatable);	// Mlp methods
}

/// Create a new artificial neural network.
///
/// At the moment this constructor creates only Multi-layer
/// perceptrons of 3 layers.
/// 
/// @code
/// ann.Mlp({ inputs=NUMBER, hiddens=NUMBER, outputs=NUMBER })
/// @endcode
///
int annlib::details::MlpCtor(lua_State* L)
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

  **newmlp(L) = lua_Mlp(inputs, hiddens, outputs);
  return 1;
}

static double calculate_hits(lua_Mlp& net, lua_PatternSet& pattern_set)
{
  Vector<double> hidden, output;
  int hits = 0;

  for (size_t c=0; c<pattern_set.size(); ++c) {
    net.recall(pattern_set[c].input, hidden, output);

    if (output.getMaxPos() == pattern_set[c].output.getMaxPos())
      ++hits;
  }

  return (double)hits / (double)pattern_set.size();
}
