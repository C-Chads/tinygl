cmake_minimum_required(VERSION 3.14)
project(tinygl
    DESCRIPTION "tinygl: The ultimate portable graphics library"
    HOMEPAGE_URL "https://github.com/C-Chads/tinygl"
    LANGUAGES C
)

# Options
option(TINYGL_BUILD_EXAMPLES "Build Examples" OFF)
option(TINYGL_BUILD_STATIC "Build Static Library" ON)
option(TINYGL_BUILD_SHARED "Build Shared Library" ON)

# Variables
set(TINYGL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TINYGL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB TINYGL_SOURCES ${TINYGL_SOURCE_DIR}/*.c)

# Libraries
if(TINYGL_BUILD_STATIC)
    add_library(tinygl-static STATIC ${TINYGL_SOURCES})
    target_include_directories(tinygl-static PUBLIC ${TINYGL_INCLUDE_DIR})
endif()
if(TINYGL_BUILD_SHARED)
    add_library(tinygl SHARED ${TINYGL_SOURCES})
    target_include_directories(tinygl PUBLIC ${TINYGL_INCLUDE_DIR})
endif()

# Examples
if(TINYGL_BUILD_EXAMPLES AND TINYGL_BUILD_STATIC)
    set(TINYGL_DEMOS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Raw_Demos)
    file(GLOB TINYGL_DEMOS ${TINYGL_DEMOS_DIR}/*.c)
    foreach(DEMO ${TINYGL_DEMOS})
        get_filename_component(DEMO_NAME ${DEMO} NAME_WE)
        add_executable(${DEMO_NAME} ${DEMO})
        target_link_libraries(${DEMO_NAME} tinygl-static m)
    endforeach()
    configure_file(${TINYGL_DEMOS_DIR}/asciifractal.sh asciifractal.sh)
    configure_file(${TINYGL_DEMOS_DIR}/char.txt char.txt)

    find_package(SDL QUIET)
    if(SDL_FOUND)
        set(TINYGL_SDL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/SDL_Examples)
        file(GLOB TINYGL_SDL_DEMOS ${TINYGL_SDL_DIR}/*.c)
        foreach(DEMO ${TINYGL_SDL_DEMOS})
            get_filename_component(DEMO_NAME ${DEMO} NAME_WE)
            set(DEMO_NAME "sdl_${DEMO_NAME}")
            add_executable(${DEMO_NAME} ${DEMO})
            target_link_libraries(${DEMO_NAME} tinygl-static m ${SDL_LIBRARY})
            target_include_directories(${DEMO_NAME} PUBLIC ${SDL_INCLUDE_DIR})
        endforeach()

        file(GLOB TINYGL_RESOURCES
            ${TINYGL_SDL_DIR}/*.png
            ${TINYGL_SDL_DIR}/*.obj
            ${TINYGL_SDL_DIR}/*.mp3
            ${TINYGL_SDL_DIR}/*.jpg
        )
        foreach(FILE ${TINYGL_RESOURCES})
            get_filename_component(FILE_NAME ${FILE} NAME)
            configure_file(${FILE} ${FILE_NAME} COPYONLY)
        endforeach()
    else()
        message(STATUS "tinygl: SDL not found")
    endif()
endif()
