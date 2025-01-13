#
# Qt build settings for Windows, Linux, macOS
#

if(my_module_QtSettings_included)
  return()
endif(my_module_QtSettings_included)
set(my_module_QtSettings_included true)


if (NOT WORKSPACE_ROOT)
  get_filename_component(WORKSPACE_ROOT ../.. ABSOLUTE)
endif()


#find correct qt variant for current build
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(QT_BUILD_BITS 64)
else()
  set(QT_BUILD_BITS 32)
endif()

if(WIN32)
  set(QT_BUILD_SYSTEM "win")
elseif(APPLE)
  set(QT_BUILD_SYSTEM "osx")
elseif(UNIX)
  set(QT_BUILD_SYSTEM "lin")
else()
  set(QT_BUILD_SYSTEM "unknown")
endif()

macro(GetQtVersionFromString SOME_PATH QT_VERSION)
  string(REGEX MATCH ".*([0-9]+)\\.([0-9]+)\\.([0-9]+).*" MY_PROGRAM_VERSION_MATCH ${SOME_PATH})
  set(QT_VERSION_MAJOR ${CMAKE_MATCH_1})
  set(QT_VERSION_MINOR ${CMAKE_MATCH_2})
  set(QT_VERSION_PATCH ${CMAKE_MATCH_3})
  set(QT_VERSION "${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}")
endmacro()


# Discover a Qt installation
if(NOT QT_VERSION AND NOT QT_BASE_PATH)
  if (WIN32)
    set(VS_VERSIONS "msvc2019_64" "msvc2022_64")
    set(QT_CANDIDATES "6.8.1;6.7.2;5.15.11;5.15.4;5.15.3;5.15.2;5.14.0;5.12.6;5.12.5")

    # QT_ROOT_DIR set? (github)
    set(QT_ROOT_DIR "$ENV{QT_ROOT_DIR}")
    if (NOT ${QT_ROOT_DIR} STREQUAL "")
      cmake_path(CONVERT ${QT_ROOT_DIR} TO_CMAKE_PATH_LIST QT_ROOT_DIR)
      message(STATUS "QT_ROOT_DIR = ${QT_ROOT_DIR}")
      if (NOT ${QT_ROOT_DIR} STREQUAL "")
        GetQtVersionFromString(${QT_ROOT_DIR} QT_VERSION)
        message(STATUS "GetQtVersionFromString ${QT_VERSION}")
        if (NOT EXISTS "${QT_ROOT_DIR}")
          set(QT_VERSION "")
        endif()
      endif()
    endif()
    
    # look for Qt custom build
    if (NOT QT_VERSION)
      foreach(_qt ${QT_CANDIDATES})
        set(_qt_search_path "${QT_CUSTOM_PATH}/${_qt}_${QT_BUILD_SYSTEM}${QT_BUILD_BITS}")
        message(STATUS "Looking for ${_qt_search_path}")
        if (EXISTS "${_qt_search_path}")
          set(QT_VERSION ${_qt})
          cmake_path(ABSOLUTE_PATH _qt_search_path NORMALIZE)
          list(APPEND CMAKE_PREFIX_PATH "${_qt_search_path}")
          break()
        endif()
      endforeach()
    endif()

    # look in default install dirs C:\Qt next or WORKSPACE\Qt
    set(QT_PREFIX_DIR "C:/Qt;${PROJECT_SOURCE_DIR}/Qt")
    if (NOT QT_VERSION)
      set(qt_found FALSE)
      foreach(_qt_prefix ${QT_PREFIX_DIR})
        foreach(_qt ${QT_CANDIDATES})
          message(STATUS "Looking for ${_qt_prefix}/${_qt}")
          # Check for Visual Studio 2019 and 2022 libraries
          foreach(_vs_version ${VS_VERSIONS})
            if (EXISTS "${_qt_prefix}/${_qt}/${_vs_version}")
              set(QT_VERSION ${_qt})
              list(APPEND CMAKE_PREFIX_PATH "${_qt_prefix}/${_qt}/${_vs_version}")
              set(qt_found TRUE)
              break()
            endif()
          endforeach()
          if (qt_found)
            break()
          endif()
        endforeach()
        if (qt_found)
          break()
        endif()
      endforeach()
    endif()
    message(STATUS "Qt version detected (${QT_VERSION})")
  endif()
endif()


set(QT_BUILD "${QT_BUILD_SYSTEM}${QT_BUILD_BITS}${QT_BUILD_SUFFIX}")



message(STATUS "QT_VERSION=${QT_VERSION}")
if(EXISTS ${QT_BASE_PATH})
  message(STATUS "QT_BASE_PATH=${QT_BASE_PATH}")
else()
  # unset(QT_BASE_PATH)
  # message("No Qt found in opt or 3rdparty")
endif()
message(STATUS "=====================================================================")



if (CMAKE_SCRIPT_DEBUG)
  message(STATUS "Qt5 cmake path: ${QT_CMAKE_PATH}")
endif()

set(CMAKE_PREFIX_PATH
  ${CMAKE_PREFIX_PATH}
  ${QT_CMAKE_PATH}
)


if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DQT_NO_DEBUG )
else()
  add_definitions(-DQT_QML_DEBUG)
endif()




string(REGEX REPLACE "\\\\" "/" WINDEPLOYQT ${QT_BASE_PATH}/bin/windeployqt.exe)

macro(installQtLibraries DESTINATION)
  # delay variable resolution to "install/cpack" step
  install(CODE "set(DESTINATION \"${DESTINATION}\")")
  install(CODE "set(WINDEPLOYQT \"${WINDEPLOYQT}\")")
  install(CODE [[ MESSAGE("Install Qt Runtime to ${CMAKE_INSTALL_PREFIX}/${DESTINATION}.") ]])
  install(CODE [[ execute_process(COMMAND ${WINDEPLOYQT} --no-quick-import --no-system-d3d-compiler ${CMAKE_INSTALL_PREFIX}/${DESTINATION}) ]] )
endmacro()
