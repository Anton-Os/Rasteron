cmake_minimum_required(VERSION 3.6)
project(Rasteron DESCRIPTION "Image Loading and Experiments")

set(default_build_type "Debug")
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
message(STATUS "Build Type ${CMAKE_BUILD_TYPE}")

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


# Rasteron project sources

set(RASTERON_SRC
    src/Toolbox.c
    src/Loader.c
    src/ImgBmp.c # BMP does not require dependencies

    src/core/ImageGen.c
    src/core/PrimitiveGen.c
    src/plugin/Cellwise.c
    src/plugin/Lattice.c
)

# find_package(zlib PATHS ${EXTERNAL_INSTALL_DIR}/lib)
find_package(zlib PATHS ${EXTERNAL_INSTALL_DIR}/lib)
if(zlib_FOUND)
    message(STATUS "Zlib located")
else()
    message(STATUS "Zlib not located, may cause issues")
endif(zlib_FOUND)

find_package(libpng PATHS ${EXTERNAL_INSTALL_DIR}/lib)
if(libpng_FOUND)
    set(PNG_SUPPORT_STR "#define USE_IMG_PNG")

    message(STATUS "PNG Support Enabled")
    list(APPEND RASTERON_SRC src/ImgPng.c) # Add png functionality
else(NOT libpng_FOUND)
    set(PNG_SUPPORT_STR "// #define USE_IMG_PNG")

    message(STATUS "PNG Support Disabled, build png")
endif(libpng_FOUND)

find_package(libtiff PATHS ${EXTERNAL_INSTALL_DIR}/lib/libtiff)
if(libtiff_FOUND)
    set(TIFF_SUPPORT_STR "#define USE_IMG_TIFF")

    message(STATUS "TIFF Support Enabled")
    list(APPEND RASTERON_SRC src/ImgTIFF.c) # add tiff functionality
else(NOT libtiff_FOUND)
    set(TIFF_SUPPORT_STR "// #define USE_IMG_TIFF")

    message(STATUS "TIFF Support Disabled, build tiff")
endif(libtiff_FOUND)



if(WIN32) # Conditional OS Sources
    list(APPEND RASTERON_SRC src/Win_NativeImg.c)
endif()

add_library(RasteronLib STATIC  ${RASTERON_SRC})

target_include_directories(RasteronLib PRIVATE include ${EXTERNAL_INSTALL_DIR}/include) #  include necessary external libs
target_link_libraries(RasteronLib PUBLIC tiff png zlibstatic)

target_include_directories(RasteronLib
    INTERFACE
    $<BUILD_INTERFACE:${interfaceHeaders}>
    $<INSTALL_INTERFACE:include/Rasteron>
)

if(WIN32)
    add_executable(Cellwise demos/Win_Cellwise.c)
    target_include_directories(Cellwise PUBLIC include)
    target_link_libraries(Cellwise RasteronLib)

    add_executable(Primitives demos/Win_Primitives.c)
    target_include_directories(Primitives PUBLIC include)
    target_link_libraries(Primitives RasteronLib)
endif()


# Install Procedures
install(TARGETS RasteronLib
        EXPORT Rasteron
        RUNTIME DESTINATION bin
	    LIBRARY DESTINATION lib
	    ARCHIVE DESTINATION lib
	    FRAMEWORK DESTINATION lib
)

add_dependencies(RasteronLib zlib libpng libtiff)

file(GLOB interfaceHeaders ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
install(FILES ${interfaceHeaders} DESTINATION include/Rasteron)
install(EXPORT Rasteron DESTINATION lib/Rasteron FILE RasteronConfig.cmake)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/findDepends.cmake.in
    ${CMAKE_INSTALL_PREFIX}/lib/Rasteron/findDepends.cmake
    @ONLY
)
