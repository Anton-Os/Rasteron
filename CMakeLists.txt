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
)

set(LIBPNG_DIR "${EXTERNAL_PROJ_DIR}/libpng")
ExternalProject_Add(libpng
    GIT_REPOSITORY "https://github.com/Anton-Os/libpng.git"
    GIT_TAG "a2446b1e4becfeaf480ffc315c44e194bf7c114a"

    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${EXTERNAL_INSTALL_DIR}

    PREFIX ${LIBPNG_DIR}
    BINARY_DIR ${LIBPNG_DIR}/Build

    DEPENDS zlib
)

add_custom_target(pngconfig
    COMMAND ${CMAKE_COMMAND} 
        -E copy ${EXTERNAL_INSTALL_DIR}/lib/libpng/libpng16.cmake ${EXTERNAL_INSTALL_DIR}/lib/libpng/libpng-config.cmake
    DEPENDS libpng
)


set(LIBTIFF_DIR "${EXTERNAL_PROJ_DIR}/libtiff")
ExternalProject_Add(libtiff
    GIT_REPOSITORY "https://github.com/Anton-Os/libtiff.git"
    GIT_TAG "790fdb276eb8dc119c18a55dc92f202e6e0025c1"

    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${EXTERNAL_INSTALL_DIR}

    PREFIX ${LIBTIFF_DIR}
    BINARY_DIR ${LIBTIFF_DIR}/Build

    DEPENDS zlib
)

set(RASTERON_SRC 
    src/Img.c 
    src/ImgTIFF.c 
    src/ImgBmp.c 
    src/ImgTarga.c
    src/ImgPng.c

    src/Cellwise/Cellwise.c
)

find_package(libpng PATHS ${EXTERNAL_INSTALL_DIR}/lib/libpng)
if(libpng_FOUND)
    set(PNG_SUPPORT_STR "#define USE_IMG_PNG")
    list(APPEND Rasteron src/ImgPng.c)
else(NOT libpng_FOUND)
    set(PNG_SUPPORT_STR "// #define USE_IMG_PNG")
endif(libpng_FOUND)

find_package(libtiff PATHS ${EXTERNAL_INSTALL_DIR}/lib/libtiff)
if(libtiff_FOUND)
    set(TIFF_SUPPORT_STR "#define USE_IMG_TIFF")
    list(APPEND Rasteron src/ImgTIFF.c)
else(NOT libtiff_FOUND)
    set(TIFF_SUPPORT_STR "// #define USE_IMG_TIFF")
endif(libtiff_FOUND)

# Probably need a GLOB expression
#add_custom_target(copySharedLibs
#)



if(WIN32) # Conditional OS Sources
    list(APPEND RASTERON_SRC src/NativeImg_win.c)
endif()

add_library(Rasteron STATIC  ${RASTERON_SRC})
target_include_directories(Rasteron PUBLIC include )
target_link_libraries(Rasteron tiff png) # FIX THIS!


if(WIN32)
    file(GLOB sLibTarg ${EXTERNAL_INSTALL_DIR}/bin/*.dll)

    foreach(file IN ITEMS ${sLibTarg})
        add_custom_command(TARGET Rasteron POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy 
            "${file}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/")
    endforeach()
endif()


if(WIN32)
    add_executable(DemoImaging demos/DemoImaging_win.c)
    target_include_directories(DemoImaging PUBLIC include)
    target_link_libraries(DemoImaging Rasteron)

    add_executable(DemoCellwise demos/DemoCellwise_win.c)
    target_include_directories(DemoCellwise PUBLIC include)
    target_link_libraries(DemoCellwise Rasteron)
endif()