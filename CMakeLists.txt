cmake_minimum_required(VERSION 3.16)
project(Sura VERSION 3.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dynamically get version from git tag
execute_process(COMMAND git describe --tags --abbrev=0
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_VERSION
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_QUIET)
if (NOT GIT_VERSION)
    set(GIT_VERSION "v2.1.0")
endif()

add_compile_definitions(SURA_VERSION="${GIT_VERSION}" NDEBUG)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

find_package(Qt6 COMPONENTS Core Gui Widgets Svg SvgWidgets REQUIRED)
find_package(exiv2 REQUIRED)

add_executable(Sura
    src/main.cpp
    src/Sura.cpp
    src/Sura.h
    src/Cli.cpp
    src/Cli.h
    src/Theme.cpp
    src/Theme.h
    src/CropBoxItem.cpp
    src/CropBoxItem.h
    resources/resources.qrc
)

target_link_libraries(Sura PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Svg
    Qt6::SvgWidgets
    exiv2lib
)
