set(prog_src "tools/_Tool.c")

add_executable(LabTool tools/LabTool/LabTool.c tools/LabTool/Lab.c ${prog_src})
target_include_directories(LabTool PUBLIC loader support core ext util)
target_link_libraries(LabTool Rasteron)

add_executable(UITool tools/UITool/UITool.c ${prog_src})
target_include_directories(UITool PUBLIC loader support core ext util)
target_link_libraries(UITool Rasteron)

add_executable(DrawTool tools/DrawTool/DrawTool.c ${prog_src})
target_include_directories(DrawTool PUBLIC loader support core ext util)
target_link_libraries(DrawTool Rasteron)

add_executable(TexTool tools/TexTool/TexTool.c ${prog_src})
target_include_directories(TexTool PUBLIC loader support core ext util)
target_link_libraries(TexTool Rasteron)

add_executable(TileTool tools/TileTool/TileTool.c ${prog_src})
target_include_directories(TileTool PUBLIC loader support core ext util)
target_link_libraries(TileTool Rasteron)

add_executable(SimTool tools/SimTool/SimTool.c ${prog_src})
target_include_directories(SimTool PUBLIC loader support core ext util)
target_link_libraries(SimTool Rasteron)

add_executable(HypnoTool tools/HypnoTool/HypnoTool.c ${prog_src})
target_include_directories(HypnoTool PUBLIC loader support core ext util demos)
target_link_libraries(HypnoTool Rasteron)

add_executable(RayTool tools/RayTool/RayTool.c ${prog_src})
target_include_directories(RayTool PUBLIC loader support core ext util demos)
target_link_libraries(RayTool Rasteron)
