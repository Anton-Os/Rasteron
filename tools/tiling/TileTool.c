#include "TileTool.h"

// --------------------------------   State    -------------------------------- //

static const Tile_State _tileDefaults = {
    ALGO_Voronoi, 0, // algorithm, callback
    POINTS_Hexagon, { 6, 6 }, 64, 0, 0.6, 0.0, // points, reps, count, expand, jitter, phase
    DIST_Euclid, 3.0, // distance, exponent
    SHADE_Stained, 0.08, // shading, edge
    PALETTE_Swatch, { 0xFF2E86AB, 0xFFF6F5AE, 0xFF1B1B1E }, // palette, colors
    0.25, 0.25, 1, // xarg, yarg, mode
    TRUCHET_Arcs, // truchet
    5, 1.0, // subdivisions, zoom
    TMIX_None
};

Tile_State _tile;

double xArg = 0.0; // TileTest callback globals
double yArg = 0.0;
int mode = 1;

static PixelPointTable _pointTable; // large tables stay static
static ColorPointTable _colorTable;

static const char* const _algorithmNames[] = { "field", "mosaic", "voronoi", "truchet", "penrose", "checker", "lined", NULL };
static const char* const _callbackNames[] = { "eq", "soft", "hard", "dot1", "dot2", "dot3", "stripe1", "stripe2", "stripe3", "break", "shine",
    "lumen", "flash", "amorph", "focal", "lined1", "lined2", "complex1", "complex2", NULL };
static const fieldCallback3 _callbacks[] = { eqTiling, softTiling, hardTiling, dotTiling1, dotTiling2, dotTiling3, stripeTiling1, stripeTiling2, stripeTiling3,
    breakTiling, shineTiling, lumenTiling, flashTiling, amorphTiling, focalTiling, linedTiling1, linedTiling2, complexTiling1, complexTiling2 };
static const char* const _pointNames[] = { "square", "triangle", "hexagon", "diagonal", "perimeter", "random", "spiral", "circle", NULL };
static const char* const _distanceNames[] = { "euclid", "manhattan", "chebyshev", "minkowski", "octagonal", "hexagonal", NULL };
static const char* const _shadingNames[] = { "flat", "shaded", "edges", "stained", "ridges", NULL };
static const char* const _paletteNames[] = { "random", "gradient", "two", "swatch", "position", NULL };
static const char* const _truchetNames[] = { "sharp", "arcs", "diagonals", "triangles", "applied", NULL };
static const char* const _mixNames[] = { "none", "blend", "xor", "mult", "add", "diff", NULL };

static const Tool_Param _tileParams[] = {
    TOOL_PARAM("algorithm", PARAM_Enum, Tile_State, algorithm, 1, 0, 0, _algorithmNames, "tiling function, periodic (checker, lined, truchet) or aperiodic (penrose, voronoi)"),
    TOOL_PARAM("callback", PARAM_Enum, Tile_State, callback, 1, 0, 0, _callbackNames, "TileTest color callback for field and mosaic"),
    TOOL_PARAM("points", PARAM_Enum, Tile_State, points, 1, 0, 0, _pointNames, "pixel point arrangement"),
    TOOL_PARAM("reps", PARAM_Int, Tile_State, reps, 2, 1, 64, NULL, "repetitions across x y (grid points, truchet cells, checker squares)"),
    TOOL_PARAM("count", PARAM_Int, Tile_State, count, 1, 1, 4000, NULL, "points for random, spiral, and circle arrangements"),
    TOOL_PARAM("expand", PARAM_Int, Tile_State, expand, 1, 0, 8, NULL, "extra points inserted between neighbors"),
    TOOL_PARAM("jitter", PARAM_Float, Tile_State, jitter, 1, 0, 2, NULL, "random displacement of points, in cells"),
    TOOL_PARAM("phase", PARAM_Float, Tile_State, phase, 1, -1000, 1000, NULL, "orbits points and turns penrose, animated by frames/encode"),
    TOOL_PARAM("distance", PARAM_Enum, Tile_State, distance, 1, 0, 0, _distanceNames, "distance equation for field and voronoi"),
    TOOL_PARAM("exponent", PARAM_Float, Tile_State, exponent, 1, 0.2, 10, NULL, "minkowski exponent (1 manhattan, 2 euclid)"),
    TOOL_PARAM("shading", PARAM_Enum, Tile_State, shading, 1, 0, 0, _shadingNames, "voronoi cell shading"),
    TOOL_PARAM("edge", PARAM_Float, Tile_State, edge, 1, 0, 1, NULL, "edge and line width"),
    TOOL_PARAM("palette", PARAM_Enum, Tile_State, palette, 1, 0, 0, _paletteNames, "how points and tiles get colors"),
    TOOL_PARAM("colors", PARAM_Color, Tile_State, colors, 3, 0, 0, NULL, "first, second, and edge colors"),
    TOOL_PARAM("xarg", PARAM_Float, Tile_State, xarg, 1, -2, 2, NULL, "TileTest x argument used by callbacks and mosaic"),
    TOOL_PARAM("yarg", PARAM_Float, Tile_State, yarg, 1, -2, 2, NULL, "TileTest y argument used by callbacks and mosaic"),
    TOOL_PARAM("mode", PARAM_Int, Tile_State, mode, 1, -20, 20, NULL, "TileTest mode argument used by callbacks"),
    TOOL_PARAM("truchet", PARAM_Enum, Tile_State, truchet, 1, 0, 0, _truchetNames, "truchet tile, 'applied' uses the applied image as the tile"),
    TOOL_PARAM("subdivisions", PARAM_Int, Tile_State, subdivisions, 1, 0, 9, NULL, "penrose subdivision passes"),
    TOOL_PARAM("zoom", PARAM_Float, Tile_State, zoom, 1, 0.1, 20, NULL, "penrose scale"),
    TOOL_PARAM("mix", PARAM_Enum, Tile_State, mix, 1, 0, 0, _mixNames, "combine with the applied image"),
};

// --------------------------------   Generation    -------------------------------- //

static mixCallback mixFunction(int mix){
    switch(mix){
        case TMIX_Blend: return blend_colors_eq;
        case TMIX_Xor: return bit_colors_xor;
        case TMIX_Mult: return mult_rgb;
        case TMIX_Add: return add_rgb;
        case TMIX_Diff: return diff_rgb;
        default: return NULL;
    }
}

static int needsPoints(int algorithm){ return algorithm == ALGO_Field || algorithm == ALGO_Mosaic || algorithm == ALGO_Voronoi; }

static Rasteron_Image* tileGenerate(ImageSize size, const Rasteron_Image* baseImg){
    Tile_State* s = &_tile;
    xArg = s->xarg; yArg = s->yarg; mode = s->mode;
    unsigned short wDiv = (unsigned short)s->reps[0], hDiv = (unsigned short)s->reps[1];
    Rasteron_Image* image = NULL;

    if(needsPoints(s->algorithm)){
        tilePoints(&_pointTable);
        tileColors(&_colorTable, &_pointTable);
        if(_colorTable.pointCount == 0){
            printf("No points for this arrangement, increase reps or count\n");
            return solidImgOp(size, s->colors[0]);
        }
        printf("  %u points\n", _colorTable.pointCount);
    }

    switch(s->algorithm){
    case ALGO_Field: image = tileFieldImgOp(size, &_colorTable, _callbacks[s->callback]); break;
    case ALGO_Mosaic: image = mosaicImgOp(size, &_colorTable, _callbacks[s->callback]); break;
    case ALGO_Voronoi: image = tileVoronoiImgOp(size, &_colorTable); break;
    case ALGO_Truchet:
        if(s->truchet == TRUCHET_Sharp) image = truschetImgOp(size, NULL, wDiv, hDiv);
        else if(s->truchet == TRUCHET_Applied){
            if(baseImg == NULL){ printf("Truchet 'applied' needs an applied image, render something and run apply first\n"); image = truschetImgOp(size, NULL, wDiv, hDiv); }
            else {
                Rasteron_Image* tileImg = resizeImgOp((ImageSize){ size.height / hDiv, size.width / wDiv }, baseImg);
                image = truschetImgOp(size, tileImg, wDiv, hDiv);
                RASTERON_DEALLOC(tileImg);
            }
        }
        else image = truchetRandomImgOp(size, s->truchet, wDiv, hDiv);
        break;
    case ALGO_Penrose: image = penroseImgOp(size, (unsigned short)s->subdivisions); break;
    case ALGO_Checker: image = checkeredImgOp(size, (ColorGrid){ (unsigned)wDiv, (unsigned)hDiv, s->colors[0], s->colors[1] }); break;
    case ALGO_Lined: image = linedImgOp(size, s->colors[0], s->colors[1], wDiv, s->xarg); break;
    }

    int usesApplied = (s->algorithm == ALGO_Truchet && s->truchet == TRUCHET_Applied);
    if(image != NULL && baseImg != NULL && s->mix != TMIX_None && !usesApplied){
        Rasteron_Image* mixedImg = mixImgOp(image, baseImg, mixFunction(s->mix));
        RASTERON_DEALLOC(image);
        image = mixedImg;
    }
    return image;
}

static void tileAnimate(unsigned frame, unsigned frameCount){
    double t = (double)frame / (double)frameCount;
    _tile.phase += t * 6.28318530717958647692; // point orbits and penrose rotation loop
    _tile.xarg += 0.1 * sin(t * 6.28318530717958647692); // mosaic and callback arguments loop
    _tile.yarg += 0.1 * cos(t * 6.28318530717958647692) - 0.1;
}

// --------------------------------   Commands    -------------------------------- //

static int cmd_points(int argc, char** argv){ // arrangement plus repetitions at once
    if(argc < 2) return TOOL_Fail;
    int arrangement = toolArgs_choice(argv[1], _pointNames);
    if(arrangement < 0) return TOOL_Fail;
    _tile.points = arrangement;
    if(argc > 2){
        int x, y;
        int countOnly = (arrangement == POINTS_Random || arrangement == POINTS_Spiral);
        if(!toolArgs_int(argv[2], &x) || x < 1 || x > (countOnly? 4000 : 64)) return TOOL_Fail;
        y = x;
        if(argc > 3 && (!toolArgs_int(argv[3], &y) || y < 1 || y > 64)) return TOOL_Fail;
        if(countOnly) _tile.count = x;
        else { _tile.reps[0] = x; _tile.reps[1] = y; }
    }
    printf("  points %s, reps %d %d, count %d\n", _pointNames[_tile.points], _tile.reps[0], _tile.reps[1], _tile.count);
    return TOOL_Render;
}

static int cmd_tile(int argc, char** argv){ // TileTest ',' key: current output becomes the truchet tile
    const Rasteron_Image* output = tool_getOutput();
    if(output == NULL) return TOOL_Fail;
    tool_setBase(copyImgOp(output));
    _tile.algorithm = ALGO_Truchet;
    _tile.truchet = TRUCHET_Applied;
    if(argc > 1){
        int reps;
        if(!toolArgs_int(argv[1], &reps) || reps < 1 || reps > 64) return TOOL_Fail;
        _tile.reps[0] = _tile.reps[1] = reps;
    }
    puts("  current image applied and used as truchet tile");
    return TOOL_Render;
}

static int cmd_reroll(int argc, char** argv){ // new random colors and points by changing the seed
    _toolCommon.seed = (int)(tool_rand() & 0x7FFFFFFF);
    printf("  seed = %d\n", _toolCommon.seed);
    return TOOL_Render;
}

static const Tool_Command _tileCommands[] = {
    { "points", "<arrangement> [x] [y]", "arrangement with reps x y (circle: points, rings), or count for random/spiral", cmd_points },
    { "tile", "[reps]", "use the current image as the truchet tile (TileTest ',' key)", cmd_tile },
    { "reroll", "", "new seed for random points and colors", cmd_reroll },
};

// --------------------------------   Tool Description    -------------------------------- //

static const Tool_Desc _tileTool = {
    "tiling", "TileTool", "periodic and aperiodic tilings with point arrangements and distance equations, based on TileTest",
    &_tile, &_tileDefaults, sizeof(Tile_State),
    _tileParams, TOOL_COUNT(_tileParams),
    _tileCommands, TOOL_COUNT(_tileCommands),
    tileGenerate, tileAnimate, NULL, NULL, NULL
};

int main(int argc, char** argv) { return tool_run(&_tileTool, argc, argv); }
