### Tests

set(test "demos/tests/_Test.c")

add_executable(LabTest demos/tests/LabTest/LabTest.c demos/tests/LabTest/Lab.c ${test})
add_executable(DrawTest demos/tests/DrawTest/DrawTest.c demos/tests/DrawTest/Draw.c ${test})
add_executable(TexTest demos/tests/TexTest/TexTest.c demos/tests/TexTest/Tex.c ${test})
add_executable(TileTest demos/tests/TileTest/TileTest.c demos/tests/TileTest/TileRules.c demos/tests/TileTest/Tile.c ${test})
add_executable(SimTest demos/tests/SimTest/SimTest.c demos/tests/SimTest/SimRules.c demos/tests/SimTest/Sim.c ${test})
add_executable(HypnoTest demos/tests/HypnoTest/HypnoTest.c demos/tests/HypnoTest/Hypno.c ${test})
add_executable(RayTest demos/tests/RayTest/RayTest.c demos/tests/RayTest/Ray.c ${test})
add_executable(UITest demos/tests/UITest/UITest.c ${test})

list(APPEND test_targets LabTest DrawTest TexTest TileTest SimTest HypnoTest RayTest UITest)
foreach(test_target IN LISTS test_targets)
    set_target_properties(${test_target} PROPERTIES FOLDER "Tests")
    target_include_directories(${test_target} PUBLIC loader support core ext util)
    target_link_libraries(${test_target} Rasteron)
endforeach()

### Tools # TODO: add tools for testing and debugging that accept command line arguments