// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef LOSEFACE_LUA_IMGLIB_H
#define LOSEFACE_LUA_IMGLIB_H

#include <string>
#include <lua.hpp>
#include <CImg.h>

#include "Eigenfaces.h"

namespace imglib {

  void registerLibrary(lua_State* L);

  namespace details {

    typedef Eigenfaces lua_Eigenfaces;
    typedef cimg_library::CImg<unsigned char> lua_Image;

    void registerEigenfaces(lua_State* L);
    void registerImage(lua_State* L);

    int EigenfacesCtor(lua_State* L);
    int ImageCtor(lua_State* L);

    lua_Eigenfaces** toEigenfaces(lua_State* L, int pos);
    lua_Image** toImage(lua_State* L, int pos);

    void image2vector(const lua_Image* img, Vector& output);

  }

}

#endif // LOSEFACE_LUA_IMGLIB_H
