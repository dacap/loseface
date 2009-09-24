# Lose Face - An open source face recognition project
# Copyright (C) 2008-2009 David Capello
# All rights reserved.

cmake_minimum_required(VERSION 2.6)

# Global project name
project(loseface)

# cmake/ directory has additional CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# System libraries on Windows
if(WIN32)
  set(sys_libs User32 Shell32 ComCtl32 ComDlg32 Gdi32 Msimg32
    WinMM AdvAPI32 Ole32 ShLwApi Vfw32 WinInet)
endif(WIN32)

# System libraries on Unix
if(UNIX)
  set(sys_libs X11 pthread)
endif(UNIX)

# Third-party libraries
set(lapack_libs lapack-double blas-double blaswrap f2c)
set(lua_libs lua luafilesystem)
set(file_libs libpng zlib)
set(libs mt19937 ${lapack_libs} ${lua_libs} ${file_libs})

# Directories where .h files can be found
include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/image
  ${CMAKE_SOURCE_DIR}/third_party
  ${CMAKE_SOURCE_DIR}/third_party/clapack
  ${CMAKE_SOURCE_DIR}/third_party/libpng
  ${CMAKE_SOURCE_DIR}/third_party/lua
  ${CMAKE_SOURCE_DIR}/third_party/luafilesystem
  ${CMAKE_SOURCE_DIR}/third_party/zlib)

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
#   src/recog/Eigenfaces.cpp
#   src/recog/FrEigenfacesMlp.cpp
#   src/recog/recog_util.cpp
  src/lua/Eigenfaces.cpp
  src/lua/Image.cpp
  src/lua/Mlp.cpp
  src/lua/MlpArray.cpp
  src/lua/Normalizer.cpp
  src/lua/PatternSet.cpp
  src/lua/annlib.cpp
  src/lua/imglib.cpp
  src/loseface.cpp
  ${platforms_sources})

target_link_libraries(loseface ${sys_libs} ${libs})

set_target_properties(loseface PROPERTIES
  COMPILE_FLAGS "-Dcimg_debug=0 -Dcimg_use_png=1 -Dcimg_display=0")

######################################################################
# Subdirectories

add_subdirectory(docs)		# Documentation
add_subdirectory(third_party)	# Third party libraries
# add_subdirectory(tools)	# Tools
add_subdirectory(tests)		# Tests
add_subdirectory(src-gui)	# GUI
