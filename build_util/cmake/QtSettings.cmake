#
# Qt build settings for Windows, Linux, macOS
#

if(my_module_QtSettings_included)
  return()
endif(my_module_QtSettings_included)
set(my_module_QtSettings_included true)


if (NOT SW_APP_ROOT)
  get_filename_component(SW_APP_ROOT ../.. ABSOLUTE)
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

    set(QT_CANDIDATES "6.7.2;5.15.11;5.15.4;5.15.3;5.15.2;5.14.0;5.12.6;5.12.5")

    # QT_ROOT_DIR set? (github)
    set(QT_ROOT_DIR "$ENV{QT_ROOT_DIR}")
    cmake_path(CONVERT ${QT_ROOT_DIR} TO_CMAKE_PATH_LIST QT_ROOT_DIR)
    message(STATUS "QT_ROOT_DIR = ${QT_ROOT_DIR}")
    if (NOT ${QT_ROOT_DIR} STREQUAL "")
      GetQtVersionFromString(${QT_ROOT_DIR} QT_VERSION)
      message(STATUS "GetQtVersionFromString ${QT_VERSION}")
      if (NOT EXISTS "${QT_ROOT_DIR}")
        set(QT_VERSION "")
      endif()
    endif()
    
    # look in workspace
    if (NOT QT_VERSION)
      foreach(_qt ${QT_CANDIDATES})
        message(STATUS "Looking for ${SW_APP_ROOT}/3rdparty/qt/${_qt}_${QT_BUILD_SYSTEM}${QT_BUILD_BITS}")
        if (EXISTS "${SW_APP_ROOT}/3rdparty/qt/${_qt}_${QT_BUILD_SYSTEM}${QT_BUILD_BITS}")
          set(QT_VERSION ${_qt})
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
          if (EXISTS "${_qt_prefix}/${_qt}/msvc2019_64")
            set(QT_VERSION ${_qt})
            list(APPEND CMAKE_PREFIX_PATH "${_qt_prefix}/${_qt}/msvc2019_64")
            set(qt_found TRUE)
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


#
# Can Qt System lib used? (Fallback)
#
# if(NOT EXISTS ${QT_BASE_PATH})
#   find_program(LSB_RELEASE_COMMAND lsb_release)
#   if(LSB_RELEASE_COMMAND)
#     execute_process(COMMAND ${LSB_RELEASE_COMMAND} -s -i
#       OUTPUT_VARIABLE TMP_LSB_RELEASE_ID
#       OUTPUT_STRIP_TRAILING_WHITESPACE)
#     string(TOLOWER ${TMP_LSB_RELEASE_ID} LSB_RELEASE_ID)
#     execute_process(COMMAND ${LSB_RELEASE_COMMAND} -s -c
#       OUTPUT_VARIABLE TMP_LSB_RELEASE_CODENAME
#       OUTPUT_STRIP_TRAILING_WHITESPACE)
#     string(TOLOWER ${TMP_LSB_RELEASE_CODENAME} LSB_RELEASE_CODENAME)

#     if (NOT QT_SYSTEM_PATH AND ${LSB_RELEASE_ID} STREQUAL "ubuntu" AND
#         (${LSB_RELEASE_CODENAME} STREQUAL "vivid"
#           OR ${LSB_RELEASE_CODENAME} STREQUAL "wily"
#           OR ${LSB_RELEASE_CODENAME} STREQUAL "xenial"
#           OR ${LSB_RELEASE_CODENAME} STREQUAL "zesty"
#           OR ${LSB_RELEASE_CODENAME} STREQUAL "bionic"
#           OR ${LSB_RELEASE_CODENAME} STREQUAL "eoan"
#           OR ${LSB_RELEASE_CODENAME} STREQUAL "focal"
#           OR ${LSB_RELEASE_CODENAME} STREQUAL "impish"
#           OR ${LSB_RELEASE_CODENAME} STREQUAL "jammy"
#           OR ${LSB_RELEASE_CODENAME} STREQUAL "noble"
#           ))
#       set(QT_SYSTEM_PATH "/usr/lib/*/cmake")
#       message("Selecting Qt system libraries.")
#     endif()
#     if (NOT QT_SYSTEM_PATH AND ${LSB_RELEASE_ID} STREQUAL "debian")
#       set(QT_SYSTEM_PATH "/usr/lib/*/cmake")
#       message("Selecting Qt system libraries.")
#     endif()
#   endif(LSB_RELEASE_COMMAND)
# endif()

# Guess where to look for installed Qt5 libraries:
# Debian/Ubuntu use something like that /opt/lib/x86_64-linux-gnu/cmake
# RHEL/CentOS use something loke that /opt/lib64/cmake

# Look for the Qt library files
# 1. opt/qt (or 3rdparty/qt)
# 2. /opt/lib (on Linux)
# find_path(QT_CMAKE_PATH Qt5/Qt5Config.cmake
#   ${QT_BASE_PATH}/lib/cmake
#   /opt/lib/*/cmake
#   /opt/lib64/cmake
#   /opt/lib/cmake
#   /usr/lib64/cmake
#   ${QT_SYSTEM_PATH}
#   )

# if(NOT QT_CMAKE_PATH)
#   message( FATAL_ERROR "Qt5 CMake support files not found." )
# endif()

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


# if (NOT DEFINED QT_VERSION_MAJOR)
#   string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)" MY_PROGRAM_VERSION_MATCH ${QT_VERSION})
#   set(QT_VERSION_MAJOR ${CMAKE_MATCH_1})
#   set(QT_VERSION_MINOR ${CMAKE_MATCH_2})
#   set(QT_VERSION_PATCH ${CMAKE_MATCH_3})
# endif()



string(REGEX REPLACE "\\\\" "/" WINDEPLOYQT ${QT_BASE_PATH}/bin/windeployqt.exe)

macro(installQtLibraries DESTINATION)
  # delay variable resolution to "install/cpack" step
  install(CODE "set(DESTINATION \"${DESTINATION}\")")
  install(CODE "set(WINDEPLOYQT \"${WINDEPLOYQT}\")")
  install(CODE [[ MESSAGE("Install Qt Runtime to ${CMAKE_INSTALL_PREFIX}/${DESTINATION}.") ]])
  install(CODE [[ execute_process(COMMAND ${WINDEPLOYQT} --no-quick-import --no-system-d3d-compiler ${CMAKE_INSTALL_PREFIX}/${DESTINATION}) ]] )
endmacro()
