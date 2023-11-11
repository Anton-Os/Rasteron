cmake_minimum_required(VERSION 3.6)
project(Rasteron DESCRIPTION "Rasteron Image Library")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/INSTALL" CACHE PATH "Install Path" FORCE)
    message("Install Prefix ${CMAKE_INSTALL_PREFIX}")
endif()

include(externalTargets.cmake)
include(FindX11)

# Core Sources

set(core_src
    src/typeinit.c

    src/Toolbox.c
    src/Loader.c

    src/core/Basics.c
    src/core/Filtering.c
    src/core/Seeding.c
    src/core/Mixing.c
    src/core/Procedural.c
    src/core/Cellwise.c
    src/core/Noise.c
)

list(APPEND core_src src/ImgBmp.c) # BMP does not require dependencies

find_package(Zlib PATHS ${EXTERNAL_INSTALL_DIR}/lib)
if(Zlib_FOUND)
    message(STATUS "Zlib located")
else()
    message(STATUS "Zlib not located, may cause issues")
endif(Zlib_FOUND)

find_package(libpng PATHS ${EXTERNAL_INSTALL_DIR}/lib)
if(libpng_FOUND)
    set(PNG_SUPPORT_STR "#define USE_IMG_PNG")
    message(STATUS "PNG Support Enabled")
    list(APPEND core_src src/ImgPng.c) # Add png functionality
else(NOT libpng_FOUND)
    set(PNG_SUPPORT_STR "// #define USE_IMG_PNG")
    message(STATUS "PNG Support Disabled, build png")
endif(libpng_FOUND)

find_package(libtiff PATHS ${EXTERNAL_INSTALL_DIR}/lib/libtiff)
if(libtiff_FOUND)
    set(TIFF_SUPPORT_STR "#define USE_IMG_TIFF")
    message(STATUS "TIFF Support Enabled")
    list(APPEND core_src src/ImgTIFF.c) # add tiff functionality
else(NOT libtiff_FOUND)
    set(TIFF_SUPPORT_STR "// #define USE_IMG_TIFF")

    message(STATUS "TIFF Support Disabled, build tiff")
endif(libtiff_FOUND)

# Plugin Sources

list(APPEND feat_src src/feature/Feat_Queue.c)

list(APPEND feat_src src/feature/Feat_Spatial.c)

if(SUPPORT_FONT_BAKING)
    set(freetype_found TRUE)
    find_library(freetype_lib NAMES freetype freetyped PATHS ${CMAKE_INSTALL_PREFIX}/lib)
    set(freetype_h "${CMAKE_INSTALL_PREFIX}/include/freetype2")
    message(STATUS "freetype_h variable: ${freetype_h}")
    message(STATUS "freetype_lib variable: ${freetype_lib}")
    list(APPEND feat_src src/feature/Feat_Text.c)
else()
    set(freetype_found FALSE)
endif()

# Helper Sources

if(WIN32) # Windows Specific
    list(APPEND util_src src/util/Util_Win.c)
elseif(UNIX AND NOT APPLE) # Unix Specific
    list(APPEND util_src src/util/Util_Unix.c)
endif()

# Rasteron Library

# add_library(Rasteron STATIC ${core_src} ${feat_src}) # library only
add_library(Rasteron STATIC ${core_src} ${feat_src} ${util_src}) # library with examples

target_include_directories(Rasteron PRIVATE include ${EXTERNAL_INSTALL_DIR}/include) #  include necessary external libs

if(WIN32)
    target_link_libraries(Rasteron PUBLIC tiff png ${zlibstatic})
elseif(UNIX AND NOT APPLE) # Unix Specific
    target_link_libraries(Rasteron PUBLIC tiff png ${zlib})
endif()

if(SUPPORT_FONT_BAKING AND NOT freetype_lib EQUAL "freetype_lib-NOTFOUND")
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

# Demo Applications

add_executable(ReadTest demos/ReadTest.c)
target_include_directories(ReadTest PUBLIC include)
target_link_libraries(ReadTest Rasteron)

add_executable(WriteTest demos/WriteTest.c)
target_include_directories(WriteTest PUBLIC include)
target_link_libraries(WriteTest Rasteron)

add_executable(Canvas 
    demos/Canvas.c 
    demos/catalouge/Catalouge.c
)
target_include_directories(Canvas PUBLIC include demos)
target_link_libraries(Canvas Rasteron)

# add_executable(Painter demos/Painter.c)
# target_include_directories(Painter PUBLIC include)
# target_link_libraries(Painter Rasteron)

# Install Procedures

install(TARGETS Rasteron
    EXPORT Rasteron
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    FRAMEWORK DESTINATION lib
)

if(UNIX)
    add_dependencies(Rasteron zlib libpng libtiff)
    if(SUPPORT_FONT_BAKING)
        add_dependencies(Rasteron FreeType)
    endif()
endif()

file(GLOB interfaceHeaders ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
install(FILES ${interfaceHeaders} DESTINATION include/Rasteron)
install(EXPORT Rasteron DESTINATION lib/Rasteron FILE RasteronConfig.cmake)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/findDepends.cmake.in
    ${CMAKE_INSTALL_PREFIX}/lib/Rasteron/findDepends.cmake
    @ONLY
)

set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
configure_file("${CMAKE_SOURCE_DIR}/include/support_def.h.in" "${CMAKE_SOURCE_DIR}/include/support_def.h")
