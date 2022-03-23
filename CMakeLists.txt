cmake_minimum_required(VERSION 3.6)
project(Rasteron DESCRIPTION "Image Loading and Generation Library")

set(default_build_type "Debug")
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
message(STATUS "Build Type ${CMAKE_BUILD_TYPE}")

# include(externalTargets.cmake)
include(FindX11)

# Rasteron Core Sources

set(Core_src
    src/Toolbox.c
    src/Loader.c
    src/ImgBmp.c # BMP does not require dependencies

    src/core/Image.c
    src/core/Dimensional.c
)

if(WIN32) # OS Specific Sources
    list(APPEND Core_src src/Win_Util.c)
elseif(UNIX)
    list(APPEND Core_src src/Unix_Util.c)
endif()

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
    list(APPEND Core_src src/ImgPng.c) # Add png functionality
else(NOT libpng_FOUND)
    set(PNG_SUPPORT_STR "// #define USE_IMG_PNG")

    message(STATUS "PNG Support Disabled, build png")
endif(libpng_FOUND)

find_package(libtiff PATHS ${EXTERNAL_INSTALL_DIR}/lib/libtiff)
if(libtiff_FOUND)
    set(TIFF_SUPPORT_STR "#define USE_IMG_TIFF")

    message(STATUS "TIFF Support Enabled")
    list(APPEND Core_src src/ImgTIFF.c) # add tiff functionality
else(NOT libtiff_FOUND)
    set(TIFF_SUPPORT_STR "// #define USE_IMG_TIFF")

    message(STATUS "TIFF Support Disabled, build tiff")
endif(libtiff_FOUND)

# Rasteron Plugin Sources

set(Plugin_src
    src/plugin/Animation.c
    src/plugin/Cellwise.c
    src/plugin/Noise.c
)

if(SUPPORT_FONT_BAKING)
    find_library(freetype_lib NAMES freetype freetyped PATHS ${CMAKE_INSTALL_PREFIX}/lib)
    set(freetype_h "${CMAKE_INSTALL_PREFIX}/include/freetype2")
    message(STATUS "freetype_lib variable: ${freetype_lib}")
    
    list(APPEND Plugin_src src/plugin/Font.c)
endif()



add_library(Rasteron STATIC ${Core_src} ${Plugin_src})

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
    add_executable(Cellwise demos/win/Cellwise.c)
    target_include_directories(Cellwise PUBLIC include)
    target_link_libraries(Cellwise Rasteron)

    add_executable(Generation demos/win/Generation.c)
    target_include_directories(Generation PUBLIC include)
    target_link_libraries(Generation Rasteron)

    add_executable(Noise demos/win/Noise.c)
    target_include_directories(Noise PUBLIC include)
    target_link_libraries(Noise Rasteron)

    add_executable(Typo demos/win/Typo.c)
    target_include_directories(Typo PUBLIC include)
    target_link_libraries(Typo Rasteron)

    add_executable(Sequence demos/win/Sequence.c)
    target_include_directories(Sequence PUBLIC include)
    target_link_libraries(Sequence Rasteron)
else() # Minimalist demos on unix, testing core functionality
    add_executable(OpenSesame demos/unix/OpenSesame.c)
    target_include_directories(OpenSesame PUBLIC include)
    target_link_libraries(OpenSesame Rasteron)

    add_executable(Test demos/unix/Test.c)
    target_include_directories(Test PUBLIC include)
    target_link_libraries(Test Rasteron)
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
