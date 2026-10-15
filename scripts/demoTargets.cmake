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

# TODO: add tools for testing and debugging that accept command line arguments