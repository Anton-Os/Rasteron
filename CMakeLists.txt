
cmake_minimum_required(VERSION 3.5...3.10)

if(POLICY CMP0104)
    cmake_policy(SET CMP0104 NEW) # CMAKE_CUDA_ARCHITECTURES sets the architectures of CUDA targets
endif()

project(Rasteron DESCRIPTION "Rasteron Image Library" LANGUAGES C)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/INSTALL" CACHE PATH "Install Path" FORCE)
    message("Install Prefix ${CMAKE_INSTALL_PREFIX}")
endif()

### Language Support

if(WIN32)
    enable_language(CXX) # util/Util_Win.cpp is C++, without it the window and video functions are never compiled
endif()

# CUDA is switched on only when a working CUDA compiler is found, it must be detected after project()
option(RASTERON_USE_CUDA "Build CUDA support when a CUDA compiler is detected" ON)
set(cuda_detected FALSE)
if(RASTERON_USE_CUDA)
    if(CMAKE_VERSION VERSION_LESS 3.18)
        message(STATUS "CUDA support needs CMake 3.18 or newer")
    else()
        include(CheckLanguage)
        check_language(CUDA)
        if(CMAKE_CUDA_COMPILER)
            if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
                set(CMAKE_CUDA_ARCHITECTURES 86) # Replace 86 with your target (e.g., 75, 80, 89, 90)
            endif()
            enable_language(CXX) # host compiler used by nvcc
            enable_language(CUDA)
            find_package(CUDAToolkit)
            if(CUDAToolkit_FOUND)
                set(cuda_detected TRUE)
            endif()
        endif()
    endif()
endif()
set(SUPPORT_CUDA ${cuda_detected} CACHE BOOL "CUDA compiler and toolkit detected, set RASTERON_USE_CUDA OFF to build without CUDA" FORCE)
if(SUPPORT_CUDA)
    message(STATUS "CUDA support enabled with toolkit ${CUDAToolkit_VERSION} for architectures ${CMAKE_CUDA_ARCHITECTURES}")
else()
    message(STATUS "CUDA support disabled")
endif()

# Sources

file(GLOB support_src support/*.c)
file(GLOB core_src core/*.c)

list(APPEND loader_src loader/Loader.c loader/ImgBmp.c loader/ImgJpeg.c) # Other file formats have dependencies
list(APPEND plugin_targets plugin/Spacial.c plugin/Queue.c) # Font extension has dependency

# list(APPEND util_src util/Test.c)
if(WIN32) # Windows Specific
    list(APPEND util_src util/Util_Win.cpp)
elseif(UNIX AND NOT APPLE) # Unix Specific
    include(FindX11)
    list(APPEND util_src util/Util_Unix.c)
    # SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -lm")
endif()


### External Libraries

include(scripts/extTargets.cmake)

## Rasterlib Library for bindings

add_library(Rasterlib SHARED ${support_src} ${core_src})
target_include_directories(Rasterlib PRIVATE support plugin core)
if(UNIX)
    target_link_libraries(Rasterlib PUBLIC m) # core uses the math library
endif()

## Rasterutil Library for running tests

add_library(Rasterutil SHARED ${util_src})
target_include_directories(Rasterutil PUBLIC support loader core plugin util)

### Rasteron Library

# add_library(Rasteron STATIC ${core_src} ${plugin_targets} "tests/_Catalouge.c") # library only
add_library(Rasteron STATIC
    ${support_src} ${loader_src} ${core_src} ${plugin_targets} ${util_src}
)
target_include_directories(Rasteron PRIVATE loader support core plugin util ${EXTERNAL_INSTALL_DIR}/include) # loader support core plugin util necessary external libs
set_target_properties(Rasteron PROPERTIES POSITION_INDEPENDENT_CODE ON) # also linked into the Rasterutil shared library

# Rasterutil calls queue, loader, and core functions, a shared library needs them resolved when it links
target_link_libraries(Rasterutil PRIVATE Rasteron)

# target_link_libraries(Rasterutil PUBLIC Rasteron)
# target_link_libraries(Rasteron PUBLIC Rasteron)

if(WIN32)
    target_link_libraries(Rasteron PUBLIC tiff png ${zlibstatic})
elseif(UNIX AND NOT APPLE) # Unix Specific
    target_link_libraries(Rasteron PUBLIC tiff png ${zlib} m)
endif()

list(APPEND library_targets Rasteron Rasterlib Rasterutil)
if(SUPPORT_FONT_BAKING AND NOT freetype_lib STREQUAL "freetype_lib-NOTFOUND")
    message(STATUS "Linking Freetype to Rasteron libraries with header ${freetype_h} and lib ${freetype_lib}")
    foreach(lib_target IN LISTS library_targets)
        target_include_directories(${lib_target} PUBLIC ${freetype_h})
        target_link_libraries(${lib_target} PUBLIC ${freetype_lib})
        set_target_properties(${lib_target} PROPERTIES FOLDER "Libraries")
    endforeach()
endif()
if(X11_FOUND)
    message(STATUS "Linking libraries to X11 for Linux")
    foreach(lib_target IN LISTS library_targets)
        target_include_directories(${lib_target} PUBLIC ${X11_INCLUDE_DIR})
        target_link_libraries(${lib_target} PUBLIC ${X11_LIBRARIES})
    endforeach()
endif()
foreach(lib_target IN LISTS library_targets)
    set_target_properties(${lib_target} PROPERTIES FOLDER "Libraries")
endforeach()

target_include_directories(Rasteron
    INTERFACE
    $<BUILD_INTERFACE:${interfaceHeaders}>
    $<INSTALL_INTERFACE:include/Rasteron>
)

### Demos and Tests

include(scripts/demoTargets.cmake)

### Installation

include(scripts/installTargets.cmake)

### Dependencies

add_dependencies(Rasteron zlib libpng libtiff)

if(UNIX)
    if(SUPPORT_FONT_BAKING AND freetype_found)
        add_dependencies(Rasteron FreeType)
    endif()
    # if(SUPPORT_MEDIA_EXPORT)
        # target_link_libraries(Rasteron PUBLIC ffmpeg)
    # endif()
endif()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts/findDepends.cmake.in
    ${CMAKE_INSTALL_PREFIX}/lib/Rasteron/findDepends.cmake
    @ONLY
)

set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
configure_file("${CMAKE_SOURCE_DIR}/support/support_def.h.in" "${CMAKE_SOURCE_DIR}/support/support_def.h")
