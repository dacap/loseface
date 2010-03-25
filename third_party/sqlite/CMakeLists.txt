# Lose Face - An open source face recognition project
# Copyright (C) 2008-2009 David Capello
# All rights reserved.

add_library(sqlite
  sqlite3.c)

set_target_properties(sqlite PROPERTIES
  COMPILE_FLAGS "-DSQLITE_THREADSAFE")
