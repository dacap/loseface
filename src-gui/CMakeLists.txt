# Lose Face - An open source face recognition project
# Copyright (C) 2008-2009 David Capello
# All rights reserved.

find_package(VACA)

if(VACA_FOUND)
  include_directories(
    ${CMAKE_SOURCE_DIR}/src-gui
    ${VACA_INCLUDE_DIR})

  add_executable(loseface-ui WIN32
    LoseFaceApp.cpp
    MainFrame.cpp
    Navigator.cpp
    main.cpp
    dao/General.cpp
    dao/User.cpp
    dao/Picture.cpp
    )

  set_target_properties(loseface-ui PROPERTIES
    COMPILE_FLAGS "-D_UNICODE -DUNICODE -DVACA_STATIC=1")

  target_link_libraries(loseface-ui ${VACA_LIBRARY} sqlite libpng zlib)
endif(VACA_FOUND)
