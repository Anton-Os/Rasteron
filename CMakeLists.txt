cmake_minimum_required(VERSION 3.6)
project(Rasteron DESCRIPTION "Rasteron Image Library")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/INSTALL" CACHE PATH "Install Path" FORCE)
    message("Install Prefix ${CMAKE_INSTALL_PREFIX}")
endif()

# Core Sources

set(core_src
    src/typeinit.c

    src/Toolbox.c
    src/Loader.c

    src/core/Basics.c
    src/core/Filtering.c
    # src/core/Seeding.c
    src/core/Mixing.c
    src/core/Procedural.c
    src/core/Cellwise.c
    src/core/Noise.c
)

list(APPEND core_src src/ImgBmp.c) # BMP does not require dependencies

# ext Sources

set(ext_src
    src/ext/Spacial.c
    src/ext/Queue.c
)

# Utility Sources

if(WIN32) # Windows Specific
    list(APPEND util_src src/util/Util_Win.c)
elseif(UNIX AND NOT APPLE) # Unix Specific
    include(FindX11)
    list(APPEND util_src src/util/Util_Unix.c)
endif()



### External Libraries

include(externalTargets.cmake)



### Rasteron Library

# add_library(Rasteron STATIC ${core_src} ${ext_src} "src/Experimental.c") # library only
add_library(Rasteron STATIC ${core_src} ${ext_src} ${util_src} "src/Experimental.c") # library with examples

target_include_directories(Rasteron PRIVATE include ${EXTERNAL_INSTALL_DIR}/include) # include necessary external libs

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

# Test Applications

add_executable(Read test/Read.c)
target_include_directories(Read PUBLIC include)
target_link_libraries(Read Rasteron)

add_executable(Write test/Write.c)
target_include_directories(Write PUBLIC include)
target_link_libraries(Write Rasteron)

# Demo Applications

add_executable(Canvas demos/Canvas.c)
target_include_directories(Canvas PUBLIC include demos)
target_link_libraries(Canvas Rasteron)

add_executable(EasyGUI demos/EasyGUI.c)
target_include_directories(EasyGUI PUBLIC include demos)
target_link_libraries(EasyGUI Rasteron)

add_executable(DrawTool demos/DrawTool.c)
target_include_directories(DrawTool PUBLIC include demos)
target_link_libraries(DrawTool Rasteron)

add_executable(TexTool demos/TexTool.c)
target_include_directories(TexTool PUBLIC include demos)
target_link_libraries(TexTool Rasteron)

add_executable(NaturalSim demos/NaturalSim.c)
target_include_directories(NaturalSim PUBLIC include demos)
target_link_libraries(NaturalSim Rasteron)


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
