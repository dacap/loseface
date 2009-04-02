# Lose Face - An open source face recognition project
# Copyright (C) 2008-2009 David Capello
# All rights reserved.

cmake_minimum_required(VERSION 2.6)

project(loseface)

if(WIN32)
  set(sys_libs User32 Shell32 ComCtl32 ComDlg32 Gdi32 Msimg32
    WinMM AdvAPI32 Ole32 ShLwApi Vfw32 WinInet)
endif(WIN32)
if(UNIX)
  set(sys_libs X11 pthread)
endif(UNIX)
set(lapack_libs lapack-double blas-double blaswrap f2c)
set(lua_libs lua luafilesystem)

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/image
  ${CMAKE_SOURCE_DIR}/third_party
  ${CMAKE_SOURCE_DIR}/third_party/clapack
  ${CMAKE_SOURCE_DIR}/third_party/lua
  ${CMAKE_SOURCE_DIR}/third_party/luafilesystem)

if(WIN32)
  set(platforms_sources
    src/captu/win32/VideoCaptureWin32.cpp)
endif(WIN32)

if(UNIX)
  set(platforms_sources
    src/captu/linux/VideoCaptureLinux.cpp
    src/captu/linux/spcadecoder.c
    src/captu/linux/spcav4l.c)
endif(UNIX)

######################################################################
# Lose Face

add_executable(loseface
  src/recog/Eigenfaces.cpp
  src/recog/FrEigenfacesMlp.cpp
  src/recog/recog_util.cpp
  src/lua/lua_annlib.cpp
  src/loseface.cpp
  ${platforms_sources})

target_link_libraries(loseface ${sys_libs} ${lapack_libs} ${lua_libs})

######################################################################
# Subdirectories

add_subdirectory(third_party)	# Third party libraries
# add_subdirectory(tools)	# Tools
# add_subdirectory(tests)	# Tests
