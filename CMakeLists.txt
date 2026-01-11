cmake_minimum_required(VERSION 3.5)
project(Rasteron DESCRIPTION "Rasteron Image Library")

cmake_policy(SET CMP0035 NEW)

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

list(APPEND loader_src
    loader/Loader.c
    loader/ImgBmp.c # BMP does not require dependencies
)

set(ext_src
    ext/Spacial.c
    ext/Queue.c
)

if(WIN32) # Windows Specific
    list(APPEND util_src util/Util_Win.cpp)
elseif(UNIX AND NOT APPLE) # Unix Specific
    include(FindX11)
    list(APPEND util_src util/Util_Unix.c)
    SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -lm")
endif()


### External Libraries

include(externalTargets.cmake)

## Rasterlib Library

add_library(Rasterlib SHARED ${support_src} ${core_src})

target_include_directories(Rasterlib PRIVATE support core)

## Rasterutil Library

add_library(Rasterutil SHARED ${util_src})

target_include_directories(Rasterutil PUBLIC support core ext util)

### Rasteron Library

# add_library(Rasteron STATIC ${core_src} ${ext_src} "programs/_Catalouge.c") # library only
add_library(Rasteron STATIC
    ${support_src} ${loader_src} ${core_src} ${ext_src} ${util_src}
)

target_include_directories(Rasteron PRIVATE loader support core ext util ${EXTERNAL_INSTALL_DIR}/include) # loader support core ext util necessary external libs

# target_link_libraries(Rasteron PUBLIC Rasterutil)

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

set(prog_src "programs/_Prog.c")

add_executable(LabTool programs/LabTool/LabTool.c programs/LabTool/Lab.c ${prog_src})
target_include_directories(LabTool PUBLIC loader support core ext util programs/os)
target_link_libraries(LabTool Rasteron)

add_executable(GUI programs/GUI/GUI.c ${prog_src})
target_include_directories(GUI PUBLIC loader support core ext util programs/os)
target_link_libraries(GUI Rasteron)

add_executable(DrawTool programs/DrawTool/DrawTool.c ${prog_src})
target_include_directories(DrawTool PUBLIC loader support core ext util programs/os)
target_link_libraries(DrawTool Rasteron)

add_executable(TexTool programs/TexTool/TexTool.c ${prog_src})
target_include_directories(TexTool PUBLIC loader support core ext util programs/os)
target_link_libraries(TexTool Rasteron)

add_executable(TileTool programs/TileTool/TileTool.c ${prog_src})
target_include_directories(TileTool PUBLIC loader support core ext util programs/os)
target_link_libraries(TileTool Rasteron)

add_executable(SimTool programs/SimTool/SimTool.c ${prog_src})
target_include_directories(SimTool PUBLIC loader support core ext util programs/os)
target_link_libraries(SimTool Rasteron)

add_executable(HypnoTool programs/HypnoTool/HypnoTool.c ${prog_src})
target_include_directories(HypnoTool PUBLIC loader support core ext util demos)
target_link_libraries(HypnoTool Rasteron)


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
    if(SUPPORT_FONT_BAKING)
        # add_dependencies(Rasteron FreeType)
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
