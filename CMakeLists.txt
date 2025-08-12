cmake_minimum_required(VERSION 3.17...4.0)

project(TinkerUsd VERSION 0.1.0 LANGUAGES CXX)

#------------------------------------------------------------------------------
# Options
#------------------------------------------------------------------------------
option(ENABLE_ASAN "Enable Address Sanitizer" OFF)

#------------------------------------------------------------------------------
# Modules
#------------------------------------------------------------------------------
# Avoid noisy install messages
set(CMAKE_INSTALL_MESSAGE "NEVER")

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
)

#------------------------------------------------------------------------------
# OpenUSD Package
#------------------------------------------------------------------------------
find_package(USD REQUIRED)

#------------------------------------------------------------------------------
# Qt6 Package
#------------------------------------------------------------------------------
set(CMAKE_PREFIX_PATH ${QT_LOCATION})
find_package(Qt6 COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

#------------------------------------------------------------------------------
# Python3 Package
#------------------------------------------------------------------------------
find_package(Python3 COMPONENTS Development REQUIRED)

#------------------------------------------------------------------------------
# Includes
#------------------------------------------------------------------------------
include(cmake/compiler_config.cmake)
include(cmake/qtAdvancedDockingSystem.cmake)
include(cmake/utils.cmake)

#------------------------------------------------------------------------------
# Directories
#------------------------------------------------------------------------------
add_subdirectory(source)
add_subdirectory(resources)