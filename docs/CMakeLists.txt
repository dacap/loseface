# Lose Face - An open source face recognition project
# Copyright (C) 2008-2009 David Capello
# All rights reserved.

set(DOXYGEN_SKIP_DOT true)
include(FindDoxygen)

if(DOXYGEN_FOUND)
  get_target_property(docs_sources loseface SOURCES)

  add_custom_target(docs
    COMMAND echo ${DOXYGEN_EXECUTABLE} Doxyfile
    COMMAND echo ${docs_sources}
    DEPENDS ${docs_sources}
    COMMENT "Generates documentation with Doxygen")
endif(DOXYGEN_FOUND)
