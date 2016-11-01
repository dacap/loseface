// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "lua/imglib.h"
#include "image/faceloc.h"

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

static int image__create(lua_State* L)
{
  lua_Image** img = toImage(L, 1);
  if (img) {
    int width = 0;
    int height = 0;
    if (lua_isnumber(L, 2)) width = lua_tonumber(L, 2);
    if (lua_isnumber(L, 3)) height = lua_tonumber(L, 3);

    if (width > 0 && height > 0)
      (*img)->assign(width, height, 1, 1);
  }
  return 0;
}

static int image__draw(lua_State* L)
{
  lua_Image** img = toImage(L, 1);
  if (img) {
    lua_Image* sprite = NULL;
    int x = 0;
    int y = 0;

    if (lua_isuserdata(L, 2)) sprite = *toImage(L, 2);
    if (lua_isnumber(L, 3)) x = lua_tonumber(L, 3);
    if (lua_isnumber(L, 4)) y = lua_tonumber(L, 4);

    if (!sprite)
      return luaL_error(L, "Invalid sprite image. Image expected as first parameter.");

    (*img)->draw_image(*sprite, x, y, 0, 0);
  }
  return 0;
}

static int image__load(lua_State* L)
{
  lua_Image** img = toImage(L, 1);
  if (img) {
    string file;

    if (lua_isstring(L, 2))
      file = lua_tostring(L, 2);
    else
      return luaL_error(L, "Invalid argument in Image:load function (string expected).");

    (*img)->load(file.c_str());

    // TODO error
  }
  return 0;
}

static int image__save(lua_State* L)
{
  lua_Image** img = toImage(L, 1);
  if (img) {
    string file;

    if (lua_isstring(L, 2))
      file = lua_tostring(L, 2);
    else
      return luaL_error(L, "Invalid argument in Image:save function (string expected).");

    (*img)->save(file.c_str());

    // TODO error
  }
  return 0;
}

static int image__get_face(lua_State* L)
{
  lua_Image** img = toImage(L, 1);
  if (img) {
    lua_Image photo = **img;
    lua_Image facergb;
    lua_Image face;

    faceloc::get_face(**img, facergb, 92, 112);
    imageproc::rgb_to_gray(facergb, face);

    **newimage(L) = face;
    return 1;

  }
  return 0;
}

static int image__width(lua_State* L)
{
  lua_Image** img = toImage(L, 1);
  if (img) {
    lua_pushnumber(L, (*img)->width);
    return 1;
  }
  return 0;
}

static int image__height(lua_State* L)
{
  lua_Image** img = toImage(L, 1);
  if (img) {
    lua_pushnumber(L, (*img)->height);
    return 1;
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
  { "create",		image__create },
  { "draw",		image__draw },
  { "load",		image__load },
  { "save",		image__save },
  { "get_face",		image__get_face },
  { "width",		image__width },
  { "height",		image__height },
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

void imglib::details::image2vector(const lua_Image* _img, Vector& output)
{
  assert(_img != NULL);

  const lua_Image& img(*_img);
  output.resize(img.width*img.height);

  size_t x, y, i = 0;
  for (y=0; y<img.height; ++y)
    for (x=0; x<img.width; ++x)
      output(i++) = img(x, y, 0, 0);
}
