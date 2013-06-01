import qbs 1.0

Application {
      Depends { name: "cpp" }
      Depends {
            name: "Qt"
            submodules: ["widgets", "xml"]
            }
      name: "qped"
      files: [
            "ped.cpp", "all.h", "editor.cpp", "editor.h",
            "config.cpp",
            "editwin.cpp",
            "enter.cpp",
            "file.cpp",
            "help.cpp",
            "kontext.cpp",
            "line.cpp",
            "mtext.cpp",
            "search.cpp",
            "text.cpp",
            "tree.cpp",
            "utils.cpp",
            "view.cpp",
            "xml.cpp",
            "cmd.h",
            "config.h",
            "editwin.h",
            "enter.h",
            "file.h",
            "gettag.h",
            "kontext.h",
            "line.h",
            "ped.h",
            "text.h",
            "tree.h",
            "utils.h",
            "view.h",
            "xml.h"
            ]
      cpp.precompiledHeader: "all.h"
      cpp.cppFlags: "-std=c++11"
      }

