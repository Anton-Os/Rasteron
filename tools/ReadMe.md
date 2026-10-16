# Rasteron Tools

Command-driven counterparts of the keypress-driven tests. Each tool opens the same preview window as the tests,
but you type commands into the console and the window re-renders after every command.

| Tool | Folder | Based on | Purpose |
|------|--------|----------|---------|
| FilterTool | `filters` | core Basics and Filtering ops | crop, flip, mirror, resize, and tune a reference image |
| RadialTool | `radial` | HypnoTest | patterns that recede into the screen center |
| NoiseTool | `noise` | TexTest | Perlin, worley, value, and octave noises plus material callbacks |
| TileTool | `tiling` | TileTest | voronoi, penrose, truchet, field, and mosaic tilings |
| RayTool | `rays` | RayTest | ray fields with x y z mapping functions |
| SimTool | `simulation` | SimTest | life-like automata, wolfram rules, sierpinski, turing patterns |

Shared code lives in `Tool.h`, `Tool.c` (history, rendering, window and console loop), and `ToolArgs.c` (parsing).

## Running

```
RadialTool                                  opens the window and a prompt
RadialTool "preset tunnel; symmetry 16"     runs startup commands, then stays interactive
RadialTool -b "mode star; save png star"    batch: run, save, exit (saves a bmp if nothing was saved)
RadialTool -n                               console only, no window
FilterTool path/to/image.png                a bare image path is loaded as the reference
```

Separate commands with `;`. `set` is optional: `symmetry 12` is the same as `set symmetry 12`.
Typing anything unknown, or a bad value, prints the available commands and parameters.

## Shared commands

| Command | Meaning |
|---------|---------|
| `set <param> <values>` | change a parameter and re-render (`rand` works for numbers, colors, and choices) |
| `get [param]` | show values |
| `apply` | commit the current image: filters stack on it, generators use it for their `mix` parameter |
| `revert [n\|all]` | undo sets, applies, and tool commands |
| `reset [all]` | default parameters (`all` also drops the applied image) |
| `save [bmp\|png\|tiff] [name]` | write the image, never overwrites existing files |
| `frames [n]` | write an animation as numbered bmp files |
| `encode [n]` | Windows builds with CUDA only: encode the animation to `<tool>.mp4` through `encodeQueue` |
| `help [name]` | everything, or one command or parameter |
| `quit` | exit |

Every tool also has `size <w> [h]` (output size) and `seed <n|rand>` (random seed used by each render, so results repeat).

Colors accept `ff8800`, `#ff8800`, `0xFFFF8800`, `f80`, names such as `red` or `sand`, and `rand`.

## Examples

```
filters> load Gumdrops.bmp; saturation 1.6; contrast 1.2; apply; flip clock; crop left 0.5
radial>  mode tunnel; symmetry 12; wave square; recede 1.2; fog 0.8; colors 101830 e0f0ff black
noise>   material grass; cells 8; octaves 6     (material <name> also loads a palette)
noise>   lattice 16 16 wrap 4; type perlin; colors 202020 f0f0f0
tiling>  algorithm voronoi; points hexagon 8 8; distance manhattan; shading stained
tiling>  algorithm penrose; subdivisions 7; palette two; colors 2e86ab f6f5ae 1b1b1e
rays>    arrange circle; count 8; func rings; map sin cos abs; scale 2 2 1
sim>     wolfram 90                             (sierpinski triangle)
sim>     placement gun; iterations 300; trail 20
sim>     turing coral; cells 200
```
