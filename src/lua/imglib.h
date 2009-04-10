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

#ifndef LOSEFACE_LUA_IMGLIB_H
#define LOSEFACE_LUA_IMGLIB_H

#include <string>
#include <lua.hpp>
#include <CImg.h>

#include "Eigenfaces.h"

namespace imglib {

  void registerLibrary(lua_State* L);

  namespace details {

    typedef Eigenfaces<double> lua_Eigenfaces;
    typedef cimg_library::CImg<unsigned char> lua_Image;

    void registerEigenfaces(lua_State* L);
    void registerImage(lua_State* L);

    int EigenfacesCtor(lua_State* L);
    int ImageCtor(lua_State* L);

    lua_Eigenfaces** toEigenfaces(lua_State* L, int pos);
    lua_Image** toImage(lua_State* L, int pos);

    void image2vector(const lua_Image* img, Vector<double>& output);

  }

}

#endif // LOSEFACE_LUA_IMGLIB_H
