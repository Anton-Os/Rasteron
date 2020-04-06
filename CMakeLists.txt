cmake_minimum_required(VERSION 3.6)
project(Rasteron DESCRIPTION "Image Loading and Experiments")

include(ExternalProject)

set(EXTERNAL_PROJ_DIR "${CMAKE_BINARY_DIR}/Projects")
set(EXTERNAL_INSTALL_DIR "${EXTERNAL_PROJ_DIR}/Install")

set(ZLIB_DIR "${EXTERNAL_PROJ_DIR}/zlib")
ExternalProject_Add(zlib
    GIT_REPOSITORY "https://github.com/madler/zlib"
    GIT_TAG "cacf7f1d4e3d44d871b605da3b647f07d718623f"

    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${EXTERNAL_INSTALL_DIR}

    PREFIX ${ZLIB_DIR}
    BINARY_DIR ${ZLIB_DIR}/Build
    # INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    # INSTALL_DIR ${ZLIB_DIR}/Install
)

set(LIBPNG_DIR "${EXTERNAL_PROJ_DIR}/libpng")
ExternalProject_Add(libpng
    GIT_REPOSITORY "https://github.com/glennrp/libpng.git"
    GIT_TAG "3cec1a16f5ec9ad925ce71785a2b429b02de5fec"

    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${EXTERNAL_INSTALL_DIR}

    PREFIX ${LIBPNG_DIR}
    BINARY_DIR ${LIBPNG_DIR}/Build
    # INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    # INSTALL_DIR ${LIBPNG_DIR}/Install

    DEPENDS zlib
)

set(LIBTIFF_DIR "${EXTERNAL_PROJ_DIR}/libtiff")
ExternalProject_Add(libtiff
    GIT_REPOSITORY "https://github.com/Anton-Os/libtiff.git"
    GIT_TAG "790fdb276eb8dc119c18a55dc92f202e6e0025c1"

    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${EXTERNAL_INSTALL_DIR}

    PREFIX ${LIBTIFF_DIR}
    BINARY_DIR ${LIBTIFF_DIR}/Build
    # INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    # INSTALL_DIR ${LIBTIFF_DIR}/Install

    DEPENDS zlib
)

set(RASTERON_SRC 
    src/Img.c 
    # src/ImgTIFF.c 
    src/ImgBmp.c 
    src/ImgTarga.c
    # src/ImgPng.c
)


# MIGHT NEED TO PATCH
find_package(libpng16 PATHS ${EXTERNAL_INSTALL_DIR}/lib/libpng)
if(libpng16_FOUND)
    set(PNG_SUPPORT_STR "#define USE_IMG_PNG")
    list(APPEND Rasteron src/ImgPng.c)
else(NOT libpng16_FOUND)
    set(PNG_SUPPORT_STR "// #define USE_IMG_PNG")
endif(libpng16_FOUND)

find_package(libtiff PATHS ${EXTERNAL_INSTALL_DIR}/lib/libtiff)
if(libtiff_FOUND)
    set(TIFF_SUPPORT_STR "#define USE_IMG_TIFF")
    list(APPEND Rasteron src/ImgTIFF.c)
else(NOT libtiff_FOUND)
    set(TIFF_SUPPORT_STR "// #define USE_IMG_TIFF")
endif(libtiff_FOUND)


if(WIN32) # Conditional OS Sources
    list(APPEND RASTERON_SRC src/NativeImg_win.c)
endif()

add_library(Rasteron STATIC
    ${RASTERON_SRC}
)

target_include_directories(Rasteron
    PUBLIC include
)

target_link_libraries(Rasteron
    tiff
    png
)