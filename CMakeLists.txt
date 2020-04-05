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
    GIT_TAG "24ec5574b8eb700912f5b7f0ded373e59d3596d5"

    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${EXTERNAL_INSTALL_DIR}

    PREFIX ${LIBTIFF_DIR}
    BINARY_DIR ${LIBTIFF_DIR}/Build
    # INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    # INSTALL_DIR ${LIBTIFF_DIR}/Install

    DEPENDS zlib
)

set(RASTERON_SRC 
    src/Img.c 
    src/ImgTIFF.c 
    src/ImgBmp.c 
    src/ImgTarga.c
    src/ImgPng.c
)

if(WIN32) # Conditional sources
    list(APPEND RASTERON_SRC src/NativeImg_win.c)
endif()

add_library(Rasteron STATIC
    ${RASTERON_SRC}
)

target_include_directories(Rasteron
    PUBLIC include
    PRIVATE ${ZLIB_DIR} ${ZLIB_DIR}/Build ${LIBTIFF_DIR}/Build ${LIBPNG_DIR}
)