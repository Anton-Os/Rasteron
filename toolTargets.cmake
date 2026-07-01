set(prog_src "tools/_Tool.c")

add_executable(LabTool tools/LabTool/LabTool.c tools/LabTool/Lab.c ${prog_src})
target_include_directories(LabTool PUBLIC loader support core ext util)
target_link_libraries(LabTool Rasteron)

add_executable(DrawTool tools/DrawTool/DrawTool.c tools/DrawTool/Draw.c ${prog_src})
target_include_directories(DrawTool PUBLIC loader support core ext util)
target_link_libraries(DrawTool Rasteron)

add_executable(TexTool tools/TexTool/TexTool.c tools/TexTool/Tex.c tools/TexTool/TexArgs.c ${prog_src})
target_include_directories(TexTool PUBLIC loader support core ext util)
target_link_libraries(TexTool Rasteron)

add_executable(TileTool tools/TileTool/TileTool.c tools/TileTool/TileRules.c tools/TileTool/Tile.c ${prog_src})
target_include_directories(TileTool PUBLIC loader support core ext util)
target_link_libraries(TileTool Rasteron)

add_executable(SimTool tools/SimTool/SimTool.c tools/SimTool/SimRules.c tools/SimTool/Sim.c ${prog_src}) # TODO: add Sim.c when implemented
target_include_directories(SimTool PUBLIC loader support core ext util)
target_link_libraries(SimTool Rasteron)

add_executable(HypnoTool tools/HypnoTool/HypnoTool.c tools/HypnoTool/Hypno.c ${prog_src})
target_include_directories(HypnoTool PUBLIC loader support core ext util demos)
target_link_libraries(HypnoTool Rasteron)

add_executable(RayTool tools/RayTool/RayTool.c tools/RayTool/Ray.c ${prog_src})
target_include_directories(RayTool PUBLIC loader support core ext util demos)
target_link_libraries(RayTool Rasteron)

add_executable(UITool tools/UITool/UITool.c ${prog_src})
target_include_directories(UITool PUBLIC loader support core ext util)
target_link_libraries(UITool Rasteron)