# Lose Face - An open source face recognition project
# Copyright (C) 2008-2009 David Capello
# All rights reserved.

find_package(VACA)

if(VACA_FOUND)
  add_executable(loseface-ui WIN32
    LoseFaceApp.cpp MainFrame.cpp Navigator.cpp main.cpp)

  set_target_properties(loseface-ui PROPERTIES
    COMPILE_FLAGS "-I${VACA_INCLUDE_DIR} -D_UNICODE -DUNICODE")

  target_link_libraries(loseface-ui ${VACA_LIBRARY})
endif(VACA_FOUND)
