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

#include "lua/imglib.h"

#define LUAOBJ_EIGENFACES	"Eigenfaces"

using namespace std;
using namespace imglib::details;

lua_Eigenfaces** imglib::details::toEigenfaces(lua_State* L, int pos)
{
  return ((lua_Eigenfaces**)luaL_checkudata(L, pos, LUAOBJ_EIGENFACES));
}

static lua_Eigenfaces** neweigenfaces(lua_State* L)
{
  lua_Eigenfaces** eig = (lua_Eigenfaces**)lua_newuserdata(L, sizeof(lua_Eigenfaces**));
  *eig = new lua_Eigenfaces;
  luaL_getmetatable(L, LUAOBJ_EIGENFACES);
  lua_setmetatable(L, -2);
  return eig;
}

static int eigenfaces__reserve(lua_State* L)
{
  lua_Eigenfaces** eig = toEigenfaces(L, 1);
  if (!eig)
    return luaL_error(L, "No Eigenfaces user-data specified");

  luaL_checktype(L, 2, LUA_TTABLE);

  int size = 0;
  lua_getfield(L, 2, "size");
  if (lua_isnumber(L, -1)) size = lua_tonumber(L, -1);
  lua_pop(L, 1);

  if (size <= 0)
      return luaL_error(L, "'size' argument expected with a value greater than zero");

  (*eig)->reserve(size);
  return 0;
}

static int eigenfaces__add_image(lua_State* L)
{
  lua_Eigenfaces** eig = toEigenfaces(L, 1);
  if (!eig)
    return luaL_error(L, "No Eigenfaces user-data specified");

  luaL_checktype(L, 2, LUA_TTABLE);

  // iterate table
  lua_pushnil(L);		// push nil for first element of table
  while (lua_next(L, 2) != 0) {
    lua_Image* img = *toImage(L, -1); // get value
    if (img) {
      Vector<double> imgVector;
      imglib::details::image2vector(img, imgVector);
      (*eig)->addImage(imgVector);
    }
    lua_pop(L, 1);		// remove value, the key is in stack for next iteration
  }

  return 0;
}

static int eigenfaces__calculate_eigenfaces(lua_State* L)
{
  lua_Eigenfaces** eig = toEigenfaces(L, 1);
  if (!eig)
    return luaL_error(L, "No Eigenfaces user-data specified");

  luaL_checktype(L, 2, LUA_TTABLE);

  size_t components = -1;
  double variance = -1.0;
  lua_getfield(L, 2, "components");
  lua_getfield(L, 2, "variance");
  if (lua_isstring(L, -1)) variance = lua_tonumber(L, -1);
  if (lua_isstring(L, -2)) components = lua_tonumber(L, -2);
  lua_pop(L, 2);

  if (!(*eig)->calculateEigenvalues())
    return luaL_error(L, "Error calculating eigenvalues/eigenvectors of covariance matrix");

  if (variance > 0.0)
    components = (*eig)->getNumComponentsFor(variance);

  if (components < 1)
    return luaL_error(L, "You have to specified the number of eigenfaces to create (at least 1)");

  (*eig)->calculateEigenfaces(components);
  lua_pushnumber(L, components);
  return 1;
}

static int eigenfaces__save(lua_State* L)
{
  lua_Eigenfaces** eig = toEigenfaces(L, 1);
  if (!eig)
    return luaL_error(L, "No Eigenfaces user-data specified");

  luaL_checktype(L, 2, LUA_TTABLE);

  string file;
  lua_getfield(L, 2, "file");
  if (lua_isstring(L, -1)) file = lua_tostring(L, -1);
  lua_pop(L, 1);

  (*eig)->save(file.c_str());
  // TODO error
  return 0;
}

///
/// @code
/// { output1, output2, output3 } =
///   Eigenfaces:project_in_eigenspace({ image1, image2, image3... })
/// @endcode
///
static int eigenfaces__project_in_eigenspace(lua_State* L)
{
  lua_Eigenfaces** eig = toEigenfaces(L, 1);
  if (!eig)
    return luaL_error(L, "No Eigenfaces user-data specified");

  luaL_checktype(L, 2, LUA_TTABLE);

  vector<Vector<double> > outputs;

  // iterate table
  lua_pushnil(L);		// push nil for first element of table
  while (lua_next(L, 2) != 0) {
    lua_Image* img = *toImage(L, -1); // get value
    if (img) {
      Vector<double> imgVector, output;
      imglib::details::image2vector(img, imgVector);

      (*eig)->projectInEigenspace(imgVector, output);
      outputs.push_back(output);
    }
    lua_pop(L, 1);		// remove value, the key is in stack for next iteration
  }

  // Create table of converted images
  lua_newtable(L);
  size_t i = 1;
  for (vector<Vector<double> >::iterator it =
  	 outputs.begin(); it != outputs.end(); ++it, ++i) {
    // a new table in the stack
    lua_pushinteger(L, i);
    lua_newtable(L);

    Vector<double>& output(*it);
    for (size_t j=0; j<output.size(); ++j) {
      lua_pushinteger(L, j+1);
      lua_pushnumber(L, output(j));
      lua_settable(L, -3);
    }

    lua_settable(L, -3);
  }

  return 1;
}

static int eigenfaces__gc(lua_State* L)
{
  lua_Eigenfaces** eig = toEigenfaces(L, 1);
  if (eig) {
    delete *eig;
    *eig = NULL;
  }
  return 0;
}

static const luaL_Reg eigenfaces_metatable[] = {
  { "reserve",			eigenfaces__reserve },
  { "add_image",		eigenfaces__add_image },
  { "calculate_eigenfaces",	eigenfaces__calculate_eigenfaces },
  { "project_in_eigenspace",	eigenfaces__project_in_eigenspace },
  { "save",			eigenfaces__save },
  { "__gc",			eigenfaces__gc },
  { NULL, NULL }
};

void imglib::details::registerEigenfaces(lua_State* L)
{
  // Eigenfaces user data
  luaL_newmetatable(L, LUAOBJ_EIGENFACES);	// create metatable for Eigenfaces
  lua_pushvalue(L, -1);				// push metatable
  lua_setfield(L, -2, "__index");		// metatable.__index = metatable
  luaL_register(L, NULL, eigenfaces_metatable); // Eigenfaces methods
}

int imglib::details::EigenfacesCtor(lua_State* L)
{
  lua_Eigenfaces* e = *neweigenfaces(L);
  return 1;
}
