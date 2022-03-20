# Adding Internal Libraries

include(ExternalProject)

set(EXTERNAL_PROJ_DIR "${CMAKE_BINARY_DIR}/Projects")
set(EXTERNAL_INSTALL_DIR "${EXTERNAL_PROJ_DIR}/Install")

list(APPEND EXTERNAL_ARGS "-DCMAKE_INSTALL_PREFIX:PATH=${EXTERNAL_INSTALL_DIR}")
list(APPEND EXTERNAL_ARGS "-DCMAKE_BUILD_TYPE:STRING=Debug") # Temporary fix

set(ZLIB_DIR "${EXTERNAL_PROJ_DIR}/zlib")
ExternalProject_Add(zlib
    GIT_REPOSITORY "https://github.com/Anton-Os/zlib.git"
    GIT_TAG "43b390add01360a3d4decde6edf441f920145e33"

    CMAKE_ARGS ${EXTERNAL_ARGS}

    PREFIX ${ZLIB_DIR}
    BINARY_DIR ${ZLIB_DIR}/Build
)

set(LIBPNG_DIR "${EXTERNAL_PROJ_DIR}/libpng")
ExternalProject_Add(libpng
    GIT_REPOSITORY "https://github.com/Anton-Os/libpng.git"
    GIT_TAG "396519eff95064233ca5d90a3ec19789f12d912f"

    CMAKE_ARGS ${EXTERNAL_ARGS}

    PREFIX ${LIBPNG_DIR}
    BINARY_DIR ${LIBPNG_DIR}/Build

    DEPENDS zlib
)

add_custom_target(pngconfig # Renames the configuration file from libpng16 to libpng
    COMMAND ${CMAKE_COMMAND}
        -E copy ${EXTERNAL_INSTALL_DIR}/lib/libpng16.cmake ${EXTERNAL_INSTALL_DIR}/lib/libpng-config.cmake
    DEPENDS libpng
)


set(LIBTIFF_DIR "${EXTERNAL_PROJ_DIR}/libtiff")
ExternalProject_Add(libtiff
    GIT_REPOSITORY "https://github.com/Anton-Os/libtiff.git"
    GIT_TAG "5430ec2523004c6d054baa9982c4183709099a57"

    CMAKE_ARGS ${EXTERNAL_ARGS}

    PREFIX ${LIBTIFF_DIR}
    BINARY_DIR ${LIBTIFF_DIR}/Build

    DEPENDS zlib
)

set(SUPPORT_FONT_BAKING true CACHE BOOL "Include font baking module" FORCE)
# set(SUPPORT_FONT_BAKING false CACHE BOOL "Include font baking module" FORCE)
if(SUPPORT_FONT_BAKING)
ExternalProject_Add(FreeType # Font Loading
    GIT_REPOSITORY "https://gitlab.freedesktop.org/freetype/freetype.git"
    GIT_TAG "801cd842e27c85cb1d5000f6397f382ffe295daa"

    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

    PREFIX ${EXTERNAL_PROJ_DIR}/FreeType
    INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
)
endif()

# Checks on Status of External Libraries