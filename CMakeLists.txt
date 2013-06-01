##=============================================================================
##  PEd Editor
##
##  Copyright (C) 1997-2011 Werner Schweer
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License version 2
##  as published by the Free Software Foundation and appearing in
##  the file LICENCE.GPL
##=============================================================================

project (qped)
cmake_minimum_required(VERSION 2.8)
#set(CMAKE_BUILD_TYPE Debug)

set(QT_USE_QTXML TRUE)
find_package(Qt4)
INCLUDE(${QT_USE_FILE})

QT_WRAP_CPP(qped moc ped.h editor.h enter.h editwin.h)

set(CMAKE_CXX_FLAGS "-std=c++11 -g -Wall -Wextra -fno-exceptions -include ${PROJECT_SOURCE_DIR}/all.h")

ADD_EXECUTABLE(qped
      ${moc}
      ped.cpp editor.cpp file.cpp help.cpp line.cpp mtext.cpp
      search.cpp text.cpp tree.cpp enter.cpp
      utils.cpp view.cpp kontext.cpp xml.cpp config.cpp editwin.cpp
      )

TARGET_LINK_LIBRARIES(qped
      ${QT_LIBRARIES}
      )

install_targets ( /bin qped )

