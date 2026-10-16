### Tests

set(test "tests/Test.c")

add_executable(LabTest tests/LabTest/LabTest.c tests/LabTest/Lab.c ${test})
add_executable(DrawTest tests/DrawTest/DrawTest.c tests/DrawTest/Draw.c ${test})
add_executable(TexTest tests/TexTest/TexTest.c tests/TexTest/Tex.c ${test})
add_executable(TileTest tests/TileTest/TileTest.c tests/TileTest/Tile.c ${test})
add_executable(SimTest tests/SimTest/SimTest.c tests/SimTest/Sim.c ${test})
add_executable(HypnoTest tests/HypnoTest/HypnoTest.c tests/HypnoTest/Hypno.c ${test})
add_executable(RayTest tests/RayTest/RayTest.c tests/RayTest/Ray.c ${test})
add_executable(UITest tests/UITest/UITest.c ${test})
if(SUPPORT_CUDA)
    set(test_cuda "tests/Test.cu")
    add_executable(CUDATest tests/CUDATest/CUDATest.cu ${test_cuda})
endif()

list(APPEND test_targets LabTest DrawTest TexTest TileTest SimTest HypnoTest RayTest UITest)
if(SUPPORT_CUDA)
    list(APPEND test_targets CUDATest)
endif()
foreach(test_target IN LISTS test_targets)
    set_target_properties(${test_target} PROPERTIES FOLDER "Tests")
    target_include_directories(${test_target} PUBLIC loader support core plugin util)
    target_link_libraries(${test_target} Rasteron)
endforeach()

### Tools

set(tool "tools/Tool.c" "tools/ToolArgs.c")

add_executable(FilterTool tools/filters/FilterTool.c tools/filters/Filter.c ${tool})
add_executable(RadialTool tools/radial/RadialTool.c tools/radial/Radial.c ${tool})
add_executable(NoiseTool tools/noise/NoiseTool.c tools/noise/Noise.c ${tool})
add_executable(TileTool tools/tiling/TileTool.c tools/tiling/Tile.c ${tool})
add_executable(RayTool tools/rays/RayTool.c tools/rays/Ray.c ${tool})
add_executable(SimTool tools/simulation/SimTool.c tools/simulation/Sim.c ${tool})

list(APPEND tool_targets FilterTool RadialTool NoiseTool TileTool RayTool SimTool)
foreach(tool_target IN LISTS tool_targets)
    set_target_properties(${tool_target} PROPERTIES FOLDER "Tools")
    target_include_directories(${tool_target} PUBLIC loader support core plugin util tools)
    target_link_libraries(${tool_target} Rasteron)
    if(UNIX)
        target_link_libraries(${tool_target} m)
    endif()
endforeach()