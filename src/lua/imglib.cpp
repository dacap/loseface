// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "lua/imglib.h"

using namespace std;

static const luaL_Reg imglib_funcstable[] = {
  { "Eigenfaces",	imglib::details::EigenfacesCtor },
  { "Image",		imglib::details::ImageCtor },
  { NULL,		NULL }
};

/// Register functions and constants of imglib in the specified Lua state.
///
void imglib::registerLibrary(lua_State* L)
{
  // Functions
  luaL_register(L, "img", imglib_funcstable);

  // Userdatas
  imglib::details::registerImage(L);
  imglib::details::registerEigenfaces(L);
}
