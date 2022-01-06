cmake_minimum_required(VERSION 3.6)
project(Rasteron DESCRIPTION "Image Loading and Experiments")

set(default_build_type "Debug")
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
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

set(SUPPORT_FONT_BAKING true CACHE BOOL "Include font baking module" FORCE)
if(SUPPORT_FONT_BAKING)
ExternalProject_Add(FreeType # Font Loading
    GIT_REPOSITORY "https://gitlab.freedesktop.org/freetype/freetype.git"
    GIT_TAG "801cd842e27c85cb1d5000f6397f382ffe295daa"

    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}

    PREFIX ${EXTERNAL_PROJ_DIR}/FreeType
    INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
)
endif()

# Rasteron project sources

set(Rasteron_Src
    src/Toolbox.c
    src/Loader.c
    src/ImgBmp.c # BMP does not require dependencies

    src/core/Image.c
    src/core/Primitive.c
    src/plugin/Cellwise.c
    src/plugin/Heightmap.c
    src/plugin/Noise.c
)
if(SUPPORT_FONT_BAKING)
    list(APPEND Rasteron_Src src/plugin/Font.c)
endif()

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
    list(APPEND Rasteron_Src src/ImgPng.c) # Add png functionality
else(NOT libpng_FOUND)
    set(PNG_SUPPORT_STR "// #define USE_IMG_PNG")

    message(STATUS "PNG Support Disabled, build png")
endif(libpng_FOUND)

find_package(libtiff PATHS ${EXTERNAL_INSTALL_DIR}/lib/libtiff)
if(libtiff_FOUND)
    set(TIFF_SUPPORT_STR "#define USE_IMG_TIFF")

    message(STATUS "TIFF Support Enabled")
    list(APPEND Rasteron_Src src/ImgTIFF.c) # add tiff functionality
else(NOT libtiff_FOUND)
    set(TIFF_SUPPORT_STR "// #define USE_IMG_TIFF")

    message(STATUS "TIFF Support Disabled, build tiff")
endif(libtiff_FOUND)

if(SUPPORT_FONT_BAKING)
    find_library(freetype_lib NAMES freetype freetyped PATHS ${CMAKE_INSTALL_PREFIX}/lib)
    set(freetype_h "${CMAKE_INSTALL_PREFIX}/include/freetype2")
    message(STATUS "freetype_lib variable: ${freetype_lib}")
endif()



if(WIN32) # Conditional OS Sources
    list(APPEND Rasteron_Src src/Win_Util.c)
elseif(UNIX)
    list(APPEND Rasteron_Src src/Unix_Util.c)
endif()

add_library(Rasteron STATIC ${Rasteron_Src})

target_include_directories(Rasteron PRIVATE include ${EXTERNAL_INSTALL_DIR}/include) #  include necessary external libs
target_link_libraries(Rasteron PUBLIC tiff png zlibstatic)
if(SUPPORT_FONT_BAKING)
    # target_link_libraries(Rasteron PUBLIC freetype)
    target_include_directories(Rasteron PUBLIC ${freetype_h})
    target_link_libraries(Rasteron PUBLIC ${freetype_lib})
endif()
if(X11_FOUND)
    target_link_libraries(Rasteron PUBLIC ${X11_LIBRARIES})
endif()

target_include_directories(Rasteron
    INTERFACE
    $<BUILD_INTERFACE:${interfaceHeaders}>
    $<INSTALL_INTERFACE:include/Rasteron>
)

if(WIN32)
    add_executable(Cellwise demos/Cellwise.c)
    target_include_directories(Cellwise PUBLIC include)
    target_link_libraries(Cellwise Rasteron)

    add_executable(Generation demos/Generation.c)
    target_include_directories(Generation PUBLIC include)
    target_link_libraries(Generation Rasteron)

    add_executable(Noise demos/Noise.c)
    target_include_directories(Noise PUBLIC include)
    target_link_libraries(Noise Rasteron)

    add_executable(Typo demos/Typo.c)
    target_include_directories(Typo PUBLIC include)
    target_link_libraries(Typo Rasteron)
endif()


# Install Procedures
install(TARGETS Rasteron
        EXPORT Rasteron
        RUNTIME DESTINATION bin
	    LIBRARY DESTINATION lib
	    ARCHIVE DESTINATION lib
	    FRAMEWORK DESTINATION lib
)

# add_dependencies(Rasteron zlib libpng libtiff)

file(GLOB interfaceHeaders ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
install(FILES ${interfaceHeaders} DESTINATION include/Rasteron)
install(EXPORT Rasteron DESTINATION lib/Rasteron FILE RasteronConfig.cmake)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/findDepends.cmake.in
    ${CMAKE_INSTALL_PREFIX}/lib/Rasteron/findDepends.cmake
    @ONLY
)

set(IMAGE_DIR "${CMAKE_SOURCE_DIR}/assets/image")
set(FONTS_DIR "${CMAKE_SOURCE_DIR}/assets/fonts")
configure_file("${CMAKE_SOURCE_DIR}/include/support.h.in" "${CMAKE_SOURCE_DIR}/include/support.h")
