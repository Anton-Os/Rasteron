cmake_minimum_required(VERSION 3.6)
project(PolyArt_Extlib)

include(ExternalProject)

set(EXTERNAL_PROJ_DIR "${CMAKE_BINARY_DIR}/Projects")

set(ZLIB_DIR "${EXTERNAL_PROJ_DIR}/zlib")
ExternalProjectAdd(zlib
    GIT_REPOSITORY "https://github.com/madler/zlib"
    GIT_TAG "cacf7f1d4e3d44d871b605da3b647f07d718623f"

    # CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

    PREFIX ${ZLIB_DIR}
    BINARY_DIR ${ZLIB_DIR}/Build
    INSTALL_DIR ${ZLIB_DIR}/Install
)

set(LIBPNG_DIR "${EXTERNAL_PROJ_DIR}/libpng")
ExternalProjectAdd(libpng
    GIT_REPOSITORY "https://github.com/glennrp/libpng.git"
    GIT_TAG "3cec1a16f5ec9ad925ce71785a2b429b02de5fec"

    # CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

    PREFIX ${LIBPNG_DIR}
    BINARY_DIR ${LIBPNG_DIR}/Build
    INSTALL_DIR ${LIBPNG_DIR}/Install

    DEPENDS zlib
)

set(LIBTIFF_DIR "${EXTERNAL_PROJ_DIR}/libtiff")
ExternalProjectAdd(libtiff
    GIT_REPOSITORY "https://github.com/LuaDist/libtiff.git"
    GIT_TAG "43d5bd6d2da90e9bf254cd42c377e4d99008f00b"

    # CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

    PREFIX ${LIBTIFF_DIR}
    BINARY_DIR ${LIBTIFF_DIR}/Build
    INSTALL_DIR ${LIBTIFF_DIR}/Install
)