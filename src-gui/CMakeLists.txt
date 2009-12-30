# Lose Face - An open source face recognition project
# Copyright (C) 2008-2009 David Capello
# All rights reserved.

find_package(Qt4)

if(QT4_FOUND)

  include_directories(${CMAKE_CURRENT_SOURCE_DIR})

  # Use QtSql
  set(QT_USE_QTSQL true)

  # Include directories
  include(${QT_USE_FILE})

  # Set of source files for Lose Face UI
  set(loseface-ui-srcs
    dao/General.cpp
    dao/Photo.cpp
    dao/User.cpp
    FlowLayout.cpp
    IdentifyDialog.cpp
    LoginDialog.cpp
    LoseFaceApp.cpp
    main.cpp
    MainWindow.cpp
    UserNavigation.cpp
    UserNavigationButton.cpp
    WebCamWidget.cpp)

  set(loseface-ui-mocs-srcs
    FlowLayout.h
    IdentifyDialog.h
    LoginDialog.h
    LoseFaceApp.h
    MainWindow.h
    UserNavigation.h
    UserNavigationButton.h
    WebCamWidget.h)

  # Wrap cpp files to moc'em
  qt4_wrap_cpp(loseface-ui-mocs ${loseface-ui-mocs-srcs})

  # Add the executable
  add_executable(loseface-ui ${loseface-ui-srcs} ${loseface-ui-mocs})

  # Link with Qt and other libraries
  target_link_libraries(loseface-ui
    ${QT_LIBRARIES}
    loseface-lib
    ${sys_libs})

  # Extra flags
  set_target_properties(loseface-ui PROPERTIES
    COMPILE_FLAGS "-Dcimg_debug=0 -Dcimg_use_png=1 -Dcimg_display=0")

endif(QT4_FOUND)
