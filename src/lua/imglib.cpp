// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

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
