// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

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

  int size = 0;
  if (lua_isnumber(L, 2))
    size = lua_tonumber(L, 2);

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

  int n = lua_gettop(L);	// number of arguments
  for (int i=2; i<=n; ++i) {
    lua_Image* img = *toImage(L, i); // get argument "i"
    if (img) {
      Vector imgVector;
      imglib::details::image2vector(img, imgVector);
      (*eig)->addImage(imgVector);
    }
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
  else if (components > (*eig)->getEigenvaluesCount()) {
    char buf[1024];
    std::sprintf(buf, "You have to specified a number of eigenfaces greater than %d",
		 (*eig)->getEigenvaluesCount());
    return luaL_error(L, buf);
  }

  (*eig)->calculateEigenfaces(components);
  lua_pushnumber(L, components);
  return 1;
}

static int eigenfaces__save(lua_State* L)
{
  lua_Eigenfaces** eig = toEigenfaces(L, 1);
  if (!eig)
    return luaL_error(L, "No Eigenfaces user-data specified");

  string file;
  if (lua_isstring(L, 2))
    file = lua_tostring(L, 2);
  else
    return luaL_error(L, "File-name expected in Eigenfaces:save() as first argument");

  (*eig)->save(file.c_str());

  // TODO error
  return 0;
}

static int eigenfaces__eigenvalues_count(lua_State* L)
{
  lua_Eigenfaces** eig = toEigenfaces(L, 1);
  if (!eig)
    return luaL_error(L, "No Eigenfaces user-data specified");

  lua_pushnumber(L, (*eig)->getEigenvaluesCount());
  return 1;
}

///
/// @code
/// { output1, output2, output3,... } =
///   Eigenfaces:project_in_eigenspace({ image1, image2, image3... })
/// @endcode
///
static int eigenfaces__project_in_eigenspace(lua_State* L)
{
  lua_Eigenfaces** eig = toEigenfaces(L, 1);
  if (!eig)
    return luaL_error(L, "No Eigenfaces user-data specified");

  luaL_checktype(L, 2, LUA_TTABLE);

  vector<Vector> outputs;

  // iterate table
  lua_pushnil(L);		// push nil for first element of table
  while (lua_next(L, 2) != 0) {
    lua_Image* img = *toImage(L, -1); // get value
    if (img) {
      Vector imgVector, output;
      imglib::details::image2vector(img, imgVector);

      (*eig)->projectInEigenspace(imgVector, output);
      outputs.push_back(output);
    }
    lua_pop(L, 1);		// remove value, the key is in stack for next iteration
  }

  // Create table of converted images
  lua_newtable(L);
  size_t i = 1;
  for (vector<Vector>::iterator it =
  	 outputs.begin(); it != outputs.end(); ++it, ++i) {
    // a new table in the stack
    lua_pushinteger(L, i);
    lua_newtable(L);

    Vector& output(*it);
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
  { "eigenvalues_count",	eigenfaces__eigenvalues_count },
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
