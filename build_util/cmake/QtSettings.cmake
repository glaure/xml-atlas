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


if(NOT QT_VERSION)
  
  # set default (last lts)
  set(QT_VERSION "5.15.2")

  if (WIN32)
    if (MSVC_VERSION EQUAL 1800)
      set(QT_CANDIDATES "5.9.2;5.6.0")
    else()
      set(QT_CANDIDATES "5.15.4;5.15.3;5.15.2;5.14.0;5.12.6;5.12.5")
    endif()

    foreach(_qt ${QT_CANDIDATES})
      message(STATUS "Looking for ${SW_APP_ROOT}/3rdparty/qt/${_qt}_${QT_BUILD_SYSTEM}${QT_BUILD_BITS}")
      if (EXISTS "${SW_APP_ROOT}/3rdparty/qt/${_qt}_${QT_BUILD_SYSTEM}${QT_BUILD_BITS}")
        set(QT_VERSION ${_qt})
        break()
      endif()
    endforeach()   
    message("Qt version detected (${QT_VERSION})")
  endif()
endif()


if(QT_VERSION VERSION_LESS 5.9.0)
  if(QT_SPECIAL_VERSION)
    set(QT_BUILD_SUFFIX "_${QT_SPECIAL_VERSION}")
  else()
    set(QT_BUILD_SUFFIX "")
  endif()
else()
  set(QT_BUILD_SUFFIX "")
endif()


set(QT_BUILD "${QT_BUILD_SYSTEM}${QT_BUILD_BITS}${QT_BUILD_SUFFIX}")



message("=Qt==================================================================")
if(NOT QT_BASE_PATH)
  foreach(_QT_PATH
      ${OPT_ROOT}/qt/${QT_VERSION}_${QT_BUILD}
      ${SW_APP_ROOT}/3rdparty/qt/${QT_VERSION}_${QT_BUILD}
      /opt/qt/${QT_VERSION}_${QT_BUILD}
      /opt/Qt${QT_VERSION}/${QT_VERSION}/gcc_64
      $ENV{HOME}/Qt${QT_VERSION}/${QT_VERSION}/gcc_64
      /opt/Qt/${QT_VERSION}/gcc_64
      $ENV{HOME}/Qt/${QT_VERSION}/gcc_64
      C:/_Qt/${QT_VERSION}/msvc2019_64
      )
                
    message("Qt Discovery: Testing ${_QT_PATH}")
    if(EXISTS ${_QT_PATH})
      set(QT_BASE_PATH ${_QT_PATH})
      get_filename_component(QT_INSTALL_PATH ${QT_BASE_PATH} DIRECTORY)
      break()
    endif()
  endforeach()
endif()


message("QT_VERSION=${QT_VERSION} (requested)")
if(EXISTS ${QT_BASE_PATH})
  message("QT_BASE_PATH=${QT_BASE_PATH}")
else()
  unset(QT_BASE_PATH)
  message("No Qt found in opt or 3rdparty")
endif()
message("=====================================================================")


#
# Can Qt System lib used? (Fallback)
#
if(NOT EXISTS ${QT_BASE_PATH})
  find_program(LSB_RELEASE_COMMAND lsb_release)
  if(LSB_RELEASE_COMMAND)
    execute_process(COMMAND ${LSB_RELEASE_COMMAND} -s -i
      OUTPUT_VARIABLE TMP_LSB_RELEASE_ID
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(TOLOWER ${TMP_LSB_RELEASE_ID} LSB_RELEASE_ID)
    execute_process(COMMAND ${LSB_RELEASE_COMMAND} -s -c
      OUTPUT_VARIABLE TMP_LSB_RELEASE_CODENAME
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(TOLOWER ${TMP_LSB_RELEASE_CODENAME} LSB_RELEASE_CODENAME)

    if (NOT QT_SYSTEM_PATH AND ${LSB_RELEASE_ID} STREQUAL "ubuntu" AND
        (${LSB_RELEASE_CODENAME} STREQUAL "vivid"
          OR ${LSB_RELEASE_CODENAME} STREQUAL "wily"
          OR ${LSB_RELEASE_CODENAME} STREQUAL "xenial"
          OR ${LSB_RELEASE_CODENAME} STREQUAL "zesty"
          OR ${LSB_RELEASE_CODENAME} STREQUAL "bionic"
          OR ${LSB_RELEASE_CODENAME} STREQUAL "eoan"
          OR ${LSB_RELEASE_CODENAME} STREQUAL "focal"
          OR ${LSB_RELEASE_CODENAME} STREQUAL "impish"
          OR ${LSB_RELEASE_CODENAME} STREQUAL "jammy"
          ))
      set(QT_SYSTEM_PATH "/usr/lib/*/cmake")
      message("Selecting Qt system libraries.")
    endif()
    if (NOT QT_SYSTEM_PATH AND ${LSB_RELEASE_ID} STREQUAL "debian")
      set(QT_SYSTEM_PATH "/usr/lib/*/cmake")
      message("Selecting Qt system libraries.")
    endif()
  endif(LSB_RELEASE_COMMAND)
endif()

# Guess where to look for installed Qt5 libraries:
# Debian/Ubuntu use something like that /opt/lib/x86_64-linux-gnu/cmake
# RHEL/CentOS use something loke that /opt/lib64/cmake

# Look for the Qt library files
# 1. opt/qt (or 3rdparty/qt)
# 2. /opt/lib (on Linux)
find_path(QT_CMAKE_PATH Qt5/Qt5Config.cmake
  ${QT_BASE_PATH}/lib/cmake
  /opt/lib/*/cmake
  /opt/lib64/cmake
  /opt/lib/cmake
  /usr/lib64/cmake
  ${QT_SYSTEM_PATH}
  )

if(NOT QT_CMAKE_PATH)
  message( FATAL_ERROR "Qt5 CMake support files not found." )
endif()

if (CMAKE_SCRIPT_DEBUG)
  message("Qt5 cmake path: ${QT_CMAKE_PATH}")
endif()

set(CMAKE_PREFIX_PATH
  ${CMAKE_PREFIX_PATH}
  ${QT_CMAKE_PATH}
)

# add a windows sdk path the cmake prefix path which was needed for some qt builds
# does not work in all cases and seems to be no longer required for new qt releases
#if(WIN32)
#
##find installed windows sdk
#find_path(WIN_SDK_PATH lib/GlU32.Lib
#  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows;CurrentInstallFolder]"
#  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Microsoft SDKs\\Windows;CurrentInstallFolder]"
#  )
#
#if(NOT WIN_SDK_PATH)
#  message( FATAL_ERROR "Windows SDK not found. (Required for Qt5 OpenGL support)" )
#endif()
#
#message("Windows SDK: ${WIN_SDK_PATH}")
#
#if(CMAKE_SIZEOF_VOID_P EQUAL 8)
#  set(WIN_SDK_LIB_PATH "${WIN_SDK_PATH}\\lib\\x64")
#else()
#  set(WIN_SDK_LIB_PATH "${WIN_SDK_PATH}\\lib")
#endif()
#
#set(CMAKE_PREFIX_PATH
#  ${CMAKE_PREFIX_PATH}
#  ${WIN_SDK_LIB_PATH}
#)
#
#endif() #win32

#add_definitions(-DQT_NO_KEYWORDS)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DQT_NO_DEBUG )
else()
  add_definitions(-DQT_QML_DEBUG)
endif()


if (NOT DEFINED QT_VERSION_MAJOR)
  string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)" MY_PROGRAM_VERSION_MATCH ${QT_VERSION})
  set(QT_VERSION_MAJOR ${CMAKE_MATCH_1})
  set(QT_VERSION_MINOR ${CMAKE_MATCH_2})
  set(QT_VERSION_PATCH ${CMAKE_MATCH_3})
endif()
