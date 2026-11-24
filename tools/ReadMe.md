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
| IllusionTool | `illusions` | new | 23 optical illusions with adjustable strength and a `reveal` switch |
| DrawTool | `drawing` | DrawTest | layers of points, paths, and brushes |

Shared code lives in `Tool.h`, `Tool.c` (history, rendering, window and console loop), and `ToolArgs.c` (parsing).

## Running

```
RadialTool                                  opens the window and a prompt
RadialTool "preset tunnel; symmetry 16"     runs startup commands, then stays interactive
RadialTool -b "mode star; save png star"    batch: run, save, exit (saves a bmp if nothing was saved)
RadialTool -n                               console only, no window
FilterTool path/to/image.png                a bare image path is loaded as the reference
RadialTool --encode a.png 2 b.png 0.5       saved images to a video, see Encoding saved images
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
| `save [bmp\|png\|tiff\|jpg] [name]` | write the image, never overwrites existing files |
| `frames [n]` | write an animation as numbered bmp files |
| `encode [n]` | Windows builds with CUDA only: encode the animation to `<tool>.mp4` through `encodeQueue` |
| `help [name]` | everything, or one command or parameter |
| `quit` | exit |

Every tool also has `size <w> [h]` (output size) and `seed <n|rand>` (random seed used by each render, so results repeat).

Colors accept `ff8800`, `#ff8800`, `0xFFFF8800`, `f80`, names such as `red` or `sand`, and `rand`.

## Encoding saved images

Every tool accepts `--encode` (or `-e`) as its first argument. It takes a queue of saved images and writes an mp4 in which
each image stays on screen for a timed interval. Like the `encode` command it is only enabled on Windows builds where CMake
detected CUDA (`TOOL_ENCODE_ENABLED`), other builds print a notice and exit.

```
<Tool> --encode [-o name.mp4] [--fps n] <image|seconds>...

RadialTool --encode a.png b.png c.png                        one second per image
RadialTool --encode 0.5 a.png b.png c.png                    a number before the first image sets the interval for all
RadialTool --encode title.png 3 radial_frame*.bmp 0.04       a number after an image or wildcard times those images
RadialTool --encode -o intro.mp4 --fps 60 a.jpg 2 b.jpg 2
```

Images are bmp, png, tiff, or jpg files and are encoded in the order given, wildcards are expanded in name order. The first
image sets the video size, later images are resized to it. Intervals are in seconds and are rounded to whole frames of the
chosen frame rate (30 by default). Existing videos are never overwritten.

## IllusionTool

`preset <name>` switches illusion and loads its recommended settings, `type <name>` switches but keeps the current ones.
`about` says what to look for, `next` steps through the catalogue, and `reveal on` removes or marks the misleading part so
you can check what is really drawn. `strength 0` also removes the effect for most figures.

cafewall, hermann, scintillating, zollner, hering, poggendorff, mullerlyer, ponzo, ebbinghaus, jastrow, kanizsa, white,
munker, contrast, chevreul, cornsweet, fraser, pinna, drift, ouchi, bulge, moire, lilac

## DrawTool

`points`, `path`, and `brush` each add a layer on top, drawn with the current parameters (the pen). Points are `x y` pairs
as fractions of the image with the origin at the top left, or a generator: `rand <n>`, `circle <n> [radius] [x y]`,
`grid <cols> <rows>`, `spiral <n> [turns]`. Parameters set after that only affect later layers. To change a layer that
already exists select it with `layer <number>`, parameters then edit that layer until `layer none` or the next new layer.
`layers` lists them, `remove`, `raise`, `lower`, `toggle`, and `nudge` rearrange them, and `apply` flattens the drawing
into the backdrop so there is room for more. `frames` and `encode` draw the layers one after another.

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
illusions> preset zollner; strength 0.9; reveal on
illusions> preset lilac; frames 12              (the gap moves once around)
draw>    colors ff8800 00ccff; gradient along; width 0.03; line curve; path 0.1 0.8 0.3 0.2 0.6 0.7 0.9 0.3
draw>    texture bristle; width 0.06; brush 0.1 0.5 0.5 0.3 0.9 0.5
draw>    dot star; width 0.05; points circle 8 0.3; blend add; dot soft; points rand 20
draw>    layer 1; colors red; cut 2; layer none (recolors and dashes the first layer)
```
