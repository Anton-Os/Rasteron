cmake_minimum_required(VERSION 3.5)
project(Rasteron DESCRIPTION "Rasteron Image Library")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/INSTALL" CACHE PATH "Install Path" FORCE)
    message("Install Prefix ${CMAKE_INSTALL_PREFIX}")
endif()

# Sources

set(support_src 
    support/type_def.c
    support/func_def.c
)

set(core_src
    core/Basics.c
    core/Filtering.c
    core/Mixing.c
    core/Procedural.c
    core/Cellwise.c
    core/Noise.c
)

set(loader_src 
    loader/Loader.c
    loader/ImgBmp.c # BMP does not require dependencies
)

set(ext_src
    ext/Spacial.c
    ext/Queue.c
)

if(WIN32) # Windows Specific
    list(APPEND util_src util/Util_Win.c)
elseif(UNIX AND NOT APPLE) # Unix Specific
    include(FindX11)
    list(APPEND util_src util/Util_Unix.c)
    SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -lm")
endif()


### External Libraries

include(externalTargets.cmake)


### Rasteron Library

# add_library(Rasteron STATIC ${core_src} ${ext_src} "demos/_Catalouge.c") # library only
add_library(Rasteron STATIC 
    ${support_src} ${loader_src} ${core_src} ${ext_src} ${util_src} 
    "demos/_Catalouge.c"
    "demos/_Demo.c"
)

target_include_directories(Rasteron PRIVATE loader support core ext util ${EXTERNAL_INSTALL_DIR}/include) # loader support core ext util necessary external libs

if(WIN32)
    target_link_libraries(Rasteron PUBLIC tiff png ${zlibstatic})
elseif(UNIX AND NOT APPLE) # Unix Specific
    target_link_libraries(Rasteron PUBLIC tiff png ${zlib})
endif()

if(SUPPORT_FONT_BAKING AND NOT freetype_lib STREQUAL "freetype_lib-NOTFOUND")
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

add_executable(Playground demos/Playground.c)
target_include_directories(Playground PUBLIC loader support core ext util demos)
target_link_libraries(Playground Rasteron)

add_executable(GUI demos/GUI/GUI.c)
target_include_directories(GUI PUBLIC loader support core ext util demos)
target_link_libraries(GUI Rasteron)

add_executable(DrawTool demos/DrawTool/DrawTool.c)
target_include_directories(DrawTool PUBLIC loader support core ext util demos)
target_link_libraries(DrawTool Rasteron)

add_executable(TexTool demos/TexTool/TexTool.c)
target_include_directories(TexTool PUBLIC loader support core ext util demos)
target_link_libraries(TexTool Rasteron)

add_executable(TileTool demos/TileTool/TileTool.c)
target_include_directories(TileTool PUBLIC loader support core ext util demos)
target_link_libraries(TileTool Rasteron)

add_executable(SimTool demos/SimTool/SimTool.c)
target_include_directories(SimTool PUBLIC loader support core ext util demos)
target_link_libraries(SimTool Rasteron)

add_executable(RadTool demos/RadTool/RadTool.c)
target_include_directories(RadTool PUBLIC loader support core ext util demos)
target_link_libraries(RadTool Rasteron)


### Installation

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
