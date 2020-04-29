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

cmake_minimum_required(VERSION 2.8)
project (qped)

set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Widgets Xml)

# QT5_WRAP_CPP(qped moc ped.h editor.h enter.h editwin.h)


set(CMAKE_CXX_FLAGS  "-std=c++17 -g -Wall -fno-exceptions -include ${PROJECT_SOURCE_DIR}/all.h")

ADD_EXECUTABLE(qped
      ${moc}
      ped.h editor.h enter.h editwin.h
      ped.cpp editor.cpp file.cpp line.cpp mtext.cpp
      search.cpp text.cpp enter.cpp
      utils.cpp view.cpp kontext.cpp xml.cpp config.cpp editwin.cpp
      )

qt5_use_modules(qped Widgets Xml)

TARGET_LINK_LIBRARIES(qped ${QT_LIBRARIES})

install_targets ( /bin qped )

