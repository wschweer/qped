project (qped)
#set(CMAKE_BUILD_TYPE Debug)

SET(QT_USE_QTXML TRUE)
find_package(Qt4)
INCLUDE(${QT_USE_FILE})

QT_WRAP_CPP(qped mocheaders ped.h editor.h enter.h editwin.h)

set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -fno-exceptions -include ${PROJECT_SOURCE_DIR}/all.h")

ADD_EXECUTABLE(qped
      ${mocheaders}
      ped.cpp editor.cpp file.cpp help.cpp line.cpp mtext.cpp
      search.cpp text.cpp tree.cpp enter.cpp
      utils.cpp view.cpp kontext.cpp xml.cpp config.cpp editwin.cpp
      )

TARGET_LINK_LIBRARIES(qped
      ${QT_LIBRARIES}
      )

install_targets ( /bin qped )
