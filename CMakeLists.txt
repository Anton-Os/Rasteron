
cmake_minimum_required(VERSION 3.5...3.10)
project(Rasteron DESCRIPTION "Rasteron Image Library")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/INSTALL" CACHE PATH "Install Path" FORCE)
    message("Install Prefix ${CMAKE_INSTALL_PREFIX}")
endif()

# Sources

file(GLOB support_src support/*.c)
file(GLOB core_src core/*.c)

list(APPEND loader_src loader/Loader.c loader/ImgBmp.c) # Other file formats have dependencies
list(APPEND ext_src ext/Spacial.c ext/Queue.c) # Font extension has dependency

# list(APPEND util_src util/_Tool.c)
if(WIN32) # Windows Specific
    list(APPEND util_src util/Util_Win.cpp)
elseif(UNIX AND NOT APPLE) # Unix Specific
    include(FindX11)
    list(APPEND util_src util/Util_Unix.c)
    SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -lm")
endif()


### External Libraries

include(extTargets.cmake)

## Rasterlib Library for bindings

add_library(Rasterlib SHARED ${support_src} ${core_src})
target_include_directories(Rasterlib PRIVATE support ext core)

## Rasterutil Library for running tools

add_library(Rasterutil SHARED ${util_src})
target_include_directories(Rasterutil PUBLIC support loader core ext util)

### Rasteron Library

# add_library(Rasteron STATIC ${core_src} ${ext_src} "tools/_Catalouge.c") # library only
add_library(Rasteron STATIC
    ${support_src} ${loader_src} ${core_src} ${ext_src} ${util_src}
)
target_include_directories(Rasteron PRIVATE loader support core ext util ${EXTERNAL_INSTALL_DIR}/include) # loader support core ext util necessary external libs

# target_link_libraries(Rasterutil PUBLIC Rasteron)
# target_link_libraries(Rasteron PUBLIC Rasteron)

if(WIN32)
    target_link_libraries(Rasteron PUBLIC tiff png ${zlibstatic})
elseif(UNIX AND NOT APPLE) # Unix Specific
    target_link_libraries(Rasteron PUBLIC tiff png ${zlib})
endif()

if(SUPPORT_FONT_BAKING AND NOT freetype_lib STREQUAL "freetype_lib-NOTFOUND")
    message(STATUS "Linking Freetype to Rasteron")
    target_include_directories(Rasteron PUBLIC ${freetype_h})
    target_link_libraries(Rasteron PUBLIC ${freetype_lib})
    # target_link_libraries(Rasteron PUBLIC freetype)
endif()
if(X11_FOUND)
    target_link_libraries(Rasteron PUBLIC ${X11_LIBRARIES})
endif()

target_include_directories(Rasteron
    INTERFACE
    $<BUILD_INTERFACE:${interfaceHeaders}>
    $<INSTALL_INTERFACE:include/Rasteron>
)

# Tool Targets

include(toolTargets.cmake)

### Installation

install(TARGETS Rasteron
    EXPORT Rasteron
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    FRAMEWORK DESTINATION lib
)

add_dependencies(Rasteron zlib libpng libtiff)

if(UNIX)
    if(SUPPORT_FONT_BAKING AND freetype_found)
        add_dependencies(Rasteron FreeType)
    endif()
    if(SUPPORT_MEDIA_EXPORT)
        # target_link_libraries(Rasteron PUBLIC ffmpeg)
    endif()
endif()

file(GLOB interfaceHeaders 
    ${CMAKE_CURRENT_SOURCE_DIR}/core/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/loader/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/util/*.h
)
install(FILES ${interfaceHeaders} DESTINATION include/Rasteron)
install(EXPORT Rasteron DESTINATION lib/Rasteron FILE RasteronConfig.cmake)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/findDepends.cmake.in
    ${CMAKE_INSTALL_PREFIX}/lib/Rasteron/findDepends.cmake
    @ONLY
)

set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
configure_file("${CMAKE_SOURCE_DIR}/support/support_def.h.in" "${CMAKE_SOURCE_DIR}/support/support_def.h")
