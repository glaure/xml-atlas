#
# XML Atlas CMake Project
#
cmake_minimum_required(VERSION 3.14)

set(APPNAME XMLAtlas)
project(${APPNAME})


#
# define SW_APP_ROOT == Projects root directory
get_filename_component(APP_ROOT . ABSOLUTE)

# expand cmake search path to check for project settings
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${APP_ROOT}/build_util/cmake
)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Whether to handle rcc automatically for Qt targets.
set(CMAKE_AUTORCC ON)

include(QtSettings)
find_package(Qt5Core REQUIRED)
find_package(Qt5Quick)
find_package(Qt5Widgets)


set(XA_SOURCE_FILES
  src/xml_atlas.cpp
  src/xml_atlas_app.cpp
  src/xml_atlas_app.h
  src/xml_atlas_data.cpp
  src/xml_atlas_data.h
  src/xml_atlas_editor.cpp
  src/xml_atlas_editor.h
)


# set(XA_RESOURCE_FILES
#   xa_resources.qrc
# )
# qt5_add_resources(XA_RESOURCES ${QT_RESOURCE_FILES})


add_executable(${APPNAME} WIN32
  ${XA_SOURCE_FILES}
  ${XA_RESOURCE_FILES}
  ${XA_RESOURCES}
)

qt5_use_modules(${APPNAME} Widgets Quick)
