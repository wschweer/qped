##=============================================================================
##  PEd Editor
##
##  Clsopyright (C) 1997-2011 Werner Schweer
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License version 2
##  as published by the Free Software Foundation and appearing in
##  the file LICENCE.GPL
##=============================================================================

cmake_minimum_required(VERSION 3.16)
project (qped VERSION 6.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD  20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Xml Core5Compat)

qt6_wrap_cpp(qped_mocs ped.h editor.h enter.h editwin.h)

ADD_EXECUTABLE(qped
      ${qped_mocs}
      ped.h editor.h enter.h editwin.h
      ped.cpp editor.cpp file.cpp line.cpp mtext.cpp
      search.cpp text.cpp enter.cpp
      utils.cpp view.cpp kontext.cpp xml.cpp config.cpp editwin.cpp
      )

target_link_libraries(qped Qt6::Gui Qt6::Widgets Qt6::Xml Qt6::Core5Compat)
target_compile_options(qped PRIVATE -Wall)

install_targets ( /bin qped )

