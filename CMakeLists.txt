#
# XML Atlas CMake Project
#
cmake_minimum_required(VERSION 3.16)

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

# QT6
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Whether to handle rcc automatically for Qt targets.
set(CMAKE_AUTORCC ON)

include(QtSettings)

find_package(Qt6 COMPONENTS Core Gui Widgets QUIET)
if (NOT ${Qt6_FOUND})
  message(STATUS "No Qt6 found")
endif()

if (NOT ${Qt6_FOUND})
  find_package(Qt5 COMPONENTS Core Gui Widgets)
  if (NOT ${Qt5_FOUND})
    message(STATUS "No Qt5 found")
  endif()
endif()

add_subdirectory(3rdparty/pugixml-1.12 EXCLUDE_FROM_ALL)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(QT_GUI_FORM_FILES
  qt_ui/xa_indent_options.ui
  qt_ui/xa_tab_options.ui
  qt_ui/xa_theme_options.ui
  qt_ui/xa_window.ui
)
if (${Qt6_FOUND})
 QT6_WRAP_UI(QT_GUI_FORMS_HEADERS ${QT_GUI_FORM_FILES})
endif()
if (${Qt5_FOUND})
 QT5_WRAP_UI(QT_GUI_FORMS_HEADERS ${QT_GUI_FORM_FILES})
endif()

set(XA_SOURCE_FILES
  src/xa_app.cpp
  src/xa_app.h
  src/xa_data.cpp
  src/xa_data.h
  src/xa_editor.cpp
  src/xa_editor.h
  src/xa_hidpi.cpp
  src/xa_hidpi.h
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

if (${Qt6_FOUND})
  qt6_add_resources(XA_RESOURCE ${XA_RESOURCE_FILES})
endif()
if (${Qt5_FOUND})
  qt5_add_resources(XA_RESOURCE ${XA_RESOURCE_FILES})
endif()

add_executable(${APPNAME} WIN32
  ${QT_GUI_FORMS_HEADERS}
  ${XA_SOURCE_FILES}
  ${XA_RESOURCE}
)

set_target_properties(${APPNAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

if (${Qt6_FOUND})
  target_link_libraries(${APPNAME} PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets)
endif()

if (${Qt5_FOUND})
  target_link_libraries(${APPNAME} PUBLIC
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets)
endif()

target_link_libraries(${APPNAME} PUBLIC
    pugixml-static
)

#
# Install
#
install(TARGETS ${APPNAME} RUNTIME DESTINATION bin)

if(WIN32)
  #installQtLibraries(bin)
endif()


#
# Packaging
#
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_SOURCE_GENERATOR "TGZ")

set(CPACK_WIX_UPGRADE_GUID "F93537BC-4988-4471-ACF2-E779BFD9A336")


include(CPack)
