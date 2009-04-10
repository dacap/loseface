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

#define LUAOBJ_IMAGE	"Image"

using namespace std;
using namespace imglib::details;

lua_Image** imglib::details::toImage(lua_State* L, int pos)
{
  return ((lua_Image**)luaL_checkudata(L, pos, LUAOBJ_IMAGE));
}

static lua_Image** newimage(lua_State* L)
{
  lua_Image** img = (lua_Image**)lua_newuserdata(L, sizeof(lua_Image**));
  *img = new lua_Image;
  luaL_getmetatable(L, LUAOBJ_IMAGE);
  lua_setmetatable(L, -2);
  return img;
}

static int image__load(lua_State* L)
{
  lua_Image** img = toImage(L, 1);
  if (img) {
    luaL_checktype(L, 2, LUA_TTABLE);

    string file;
    lua_getfield(L, 2, "file");
    if (lua_isstring(L, -1)) file = lua_tostring(L, -1);
    lua_pop(L, 1);

    (*img)->load(file.c_str());

    // TODO error
  }
  return 0;
}

static int image__gc(lua_State* L)
{
  lua_Image** img = toImage(L, 1);
  if (img) {
    delete *img;
    *img = NULL;
  }
  return 0;
}

static const luaL_Reg image_metatable[] = {
  { "load",		image__load },
  { "__gc",		image__gc },
  { NULL, NULL }
};

void imglib::details::registerImage(lua_State* L)
{
  // Image user data
  luaL_newmetatable(L, LUAOBJ_IMAGE);		// create metatable for Image
  lua_pushvalue(L, -1);				// push metatable
  lua_setfield(L, -2, "__index");		// metatable.__index = metatable
  luaL_register(L, NULL, image_metatable);	// Image methods
}

int imglib::details::ImageCtor(lua_State* L)
{
  lua_Image* img = *newimage(L);
  return 1;
}

void imglib::details::image2vector(const lua_Image* _img, Vector<double>& output)
{
  assert(_img != NULL);

  const lua_Image& img(*_img);
  output.resize(img.width*img.height);

  size_t x, y, i = 0;
  for (y=0; y<img.height; ++y)
    for (x=0; x<img.width; ++x)
      output(i++) = img(x, y, 0, 0);
}
