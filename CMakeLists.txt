#
# XML Atlas CMake Project
#
cmake_minimum_required(VERSION 3.14)

#
# C++17 or better
set(CMAKE_CXX_STANDARD 17)

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
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)


add_subdirectory(3rdparty/pugixml-1.12)


set(XA_SOURCE_FILES
  src/xa_app.cpp
  src/xa_app.h
  src/xa_data.cpp
  src/xa_data.h
  src/xa_editor.cpp
  src/xa_editor.h
  src/xa_highlighter_xml.cpp
  src/xa_highlighter_xml.h
  src/xa_window.cpp
  src/xa_window.h
  src/xa_tree_dock.cpp
  src/xa_tree_dock.h
  src/xa_xml_tree_item.cpp
  src/xa_xml_tree_item.h
  src/xa_xml_tree_model.cpp
  src/xa_xml_tree_model.h
  src/xml_atlas.cpp
)

set(XA_RESOURCE_FILES
  xa_resources.qrc
)

qt5_add_resources(XA_RESOURCE ${XA_RESOURCE_FILES})

add_executable(${APPNAME} WIN32
  ${XA_SOURCE_FILES}
  ${XA_RESOURCE}
)

set_target_properties(${APPNAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(${APPNAME} PUBLIC
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    pugixml-static
)

