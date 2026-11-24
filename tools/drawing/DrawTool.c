#include "DrawTool.h"

// --------------------------------   State    -------------------------------- //

static const Draw_State _drawDefaults = {
    { DRAW_CANVAS_COLOR, 0xFF333333 }, // canvas
    BACKDROP_Solid, // backdrop
    10, // grid
    1.0, // progress
    { // pen
        DRAW_Path, // kind
        { 0xFFFF8800, 0xFF00CCFF }, // colors
        GRADIENT_None, // gradient
        0.02, // width
        1.0, // opacity
        0.0, // softness
        BLEND_Normal, // blend
        DOT_Disc, // dot
        LINE_Straight, // line
        0.0, // wave
        6.0, // frequency
        0.0, // cut
        FALSE, // fill
        BRUSH_Round // brush
    },
    -1, // selected
    0 // layerCount, layers start empty
};

Draw_State _draw;

static const char* const _kindNames[] = { "points", "path", "brush", NULL };
static const char* const _dotNames[] = { "disc", "ring", "soft", "square", "diamond", "cross", "star", "half", "spray", NULL };
static const char* const _lineNames[] = { "straight", "curve", "closed", "loop", NULL };
static const char* const _textureNames[] = { "round", "flat", "power", "bulge", "bristle", "chalk", "spray", "stamp", NULL };
static const char* const _gradientNames[] = { "none", "along", "across", NULL };
static const char* const _blendNames[] = { "normal", "add", "multiply", "screen", "diff", "erase", NULL };
static const char* const _backdropNames[] = { "solid", "checker", "gradient", NULL };

static const Tool_Param _drawParams[] = {
    TOOL_PARAM("canvas", PARAM_Color, Draw_State, canvas, 2, 0, 0, NULL, "backdrop colors (an applied image replaces the backdrop)"),
    TOOL_PARAM("backdrop", PARAM_Enum, Draw_State, backdrop, 1, 0, 0, _backdropNames, "backdrop style using the canvas colors"),
    TOOL_PARAM("grid", PARAM_Int, Draw_State, grid, 1, 1, 128, NULL, "checker squares across the backdrop"),
    TOOL_PARAM("progress", PARAM_Float, Draw_State, progress, 1, 0, 1, NULL, "fraction of the drawing shown, layer by layer, animated by frames/encode"),
    // everything below is the pen used by new layers, or the selected layer after 'layer <number>'
    TOOL_PARAM("kind", PARAM_Enum, Draw_State, pen.kind, 1, 0, 0, _kindNames, "what a selected layer draws from its points (new layers take it from their command)"),
    TOOL_PARAM("colors", PARAM_Color, Draw_State, pen.colors, 2, 0, 0, NULL, "layer color and gradient end color"),
    TOOL_PARAM("gradient", PARAM_Enum, Draw_State, pen.gradient, 1, 0, 0, _gradientNames, "blend toward the second color along the stroke or away from its center"),
    TOOL_PARAM("width", PARAM_Float, Draw_State, pen.width, 1, 0.001, 0.5, NULL, "stroke width and dot diameter as a fraction of the image height"),
    TOOL_PARAM("opacity", PARAM_Float, Draw_State, pen.opacity, 1, 0, 1, NULL, "layer opacity"),
    TOOL_PARAM("softness", PARAM_Float, Draw_State, pen.softness, 1, 0, 1, NULL, "feathering of edges"),
    TOOL_PARAM("blend", PARAM_Enum, Draw_State, pen.blend, 1, 0, 0, _blendNames, "how the layer combines with the layers below (erase uncovers the backdrop)"),
    TOOL_PARAM("dot", PARAM_Enum, Draw_State, pen.dot, 1, 0, 0, _dotNames, "dot shape of points layers"),
    TOOL_PARAM("line", PARAM_Enum, Draw_State, pen.line, 1, 0, 0, _lineNames, "how paths and brushes connect their points (closed and loop return to the start)"),
    TOOL_PARAM("wave", PARAM_Float, Draw_State, pen.wave, 1, 0, 20, NULL, "sideways wobble of paths and brushes in stroke widths"),
    TOOL_PARAM("frequency", PARAM_Float, Draw_State, pen.frequency, 1, 0, 200, NULL, "repetitions along the stroke for wave, bulge, and stamp"),
    TOOL_PARAM("cut", PARAM_Float, Draw_State, pen.cut, 1, 0, 50, NULL, "dash length of paths in stroke widths, 0 is solid"),
    TOOL_PARAM("fill", PARAM_Bool, Draw_State, pen.fill, 1, 0, 0, NULL, "fill closed and loop paths"),
    TOOL_PARAM("texture", PARAM_Enum, Draw_State, pen.brush, 1, 0, 0, _textureNames, "texture of brush layers"),
};

// --------------------------------   Layer Bookkeeping    -------------------------------- //

static void penCommit(void){ // stores the pen in the selected layer before the selection changes
    if(_draw.selected >= 0 && _draw.selected < _draw.layerCount) _draw.layers[_draw.selected].style = _draw.pen;
}

static void penSelect(int index){
    penCommit();
    _draw.selected = (index >= 0 && index < _draw.layerCount)? index : -1;
    if(_draw.selected >= 0) _draw.pen = _draw.layers[_draw.selected].style;
}

static Draw_Layer* layerNew(int kind){ // new top layer drawn with the current pen
    if(_draw.layerCount >= DRAW_LAYERS_MAX){
        printf("All %d layers are in use, run apply to flatten the drawing or remove a layer\n", DRAW_LAYERS_MAX);
        return NULL;
    }
    penCommit();
    Draw_Layer* layer = &_draw.layers[_draw.layerCount];
    memset(layer, 0, sizeof(Draw_Layer));
    _draw.pen.kind = kind;
    layer->style = _draw.pen;
    layer->visible = TRUE;
    _draw.layerCount++;
    _draw.selected = -1; // the pen stays free for the next layer, see cmd_layer
    return layer;
}

static int layerArg(int argc, char** argv, int* index){ // optional 1 based layer number, defaults to the selected layer or else the top layer
    *index = (_draw.selected >= 0)? _draw.selected : _draw.layerCount - 1;
    if(argc > 1){
        int number;
        if(toolArgs_equals(argv[1], "top")) *index = _draw.layerCount - 1;
        else if(toolArgs_equals(argv[1], "bottom")) *index = 0;
        else if(toolArgs_int(argv[1], &number)) *index = number - 1;
        else *index = -1;
    }
    if(*index < 0 || *index >= _draw.layerCount){
        if(_draw.layerCount == 0) printf("There are no layers yet, add one with points, path, or brush\n");
        else printf("Layers are numbered 1 to %d\n", _draw.layerCount);
        return FALSE;
    }
    return TRUE;
}

static void printLayer(int index){
    const Draw_Layer* layer = &_draw.layers[index];
    const Draw_Style* style = drawLayerStyle(&_draw, index);
    const char* shape = (style->kind == DRAW_Points)? _dotNames[style->dot] : (style->kind == DRAW_Brush)? _textureNames[style->brush] : _lineNames[style->line];
    printf("  %c%2d %-6s %2d point%s  %-8s %08X  width %g  %s%s\n", (index == _draw.selected)? '*' : ' ', index + 1, _kindNames[style->kind],
        layer->pointCount, (layer->pointCount == 1)? " " : "s", shape, style->colors[0], style->width, _blendNames[style->blend], layer->visible? "" : "  (hidden)");
}

// --------------------------------   Point Lists    -------------------------------- //

static int addPoint(Draw_Layer* layer, double x, double y){
    if(layer->pointCount >= DRAW_POINTS_MAX) return FALSE;
    layer->points[layer->pointCount][0] = x;
    layer->points[layer->pointCount][1] = y;
    layer->pointCount++;
    return TRUE;
}

static int parsePoints(Draw_Layer* layer, int argc, char** argv){ // argv starts at the first value, appends to the layer
    int count = 0, more = 0;
    double values[3] = { 0.35, 0.5, 0.5 }; // radius and center of a circle unless given
    if(argc < 1) return FALSE;

    int generator = toolArgs_equals(argv[0], "rand") || toolArgs_equals(argv[0], "random") || toolArgs_equals(argv[0], "circle")
        || toolArgs_equals(argv[0], "grid") || toolArgs_equals(argv[0], "spiral");
    if(generator){
        if(argc < 2 || !toolArgs_int(argv[1], &count) || count < 1 || count > DRAW_POINTS_MAX) return FALSE;
        for(int a = 2; a < argc && a < 5; a++) if(!toolArgs_float(argv[a], &values[a - 2])) return FALSE;
    }

    if(toolArgs_equals(argv[0], "rand") || toolArgs_equals(argv[0], "random")){ // rand <count>
        for(int p = 0; p < count; p++) if(!addPoint(layer, 0.05 + (0.9 * tool_randf()), 0.05 + (0.9 * tool_randf()))) more++;
    }
    else if(toolArgs_equals(argv[0], "circle")){ // circle <count> [radius] [x y]
        for(int p = 0; p < count; p++){
            double angle = (DRAW_TAU * p / count) - (DRAW_TAU / 4.0);
            if(!addPoint(layer, values[1] + (values[0] * cos(angle)), values[2] + (values[0] * sin(angle)))) more++;
        }
    }
    else if(toolArgs_equals(argv[0], "grid")){ // grid <columns> <rows>
        int rows = (int)values[0];
        if(argc < 3 || rows < 1 || rows > DRAW_POINTS_MAX) return FALSE;
        for(int r = 0; r < rows; r++)
            for(int c = 0; c < count; c++) if(!addPoint(layer, (c + 0.5) / count, (r + 0.5) / rows)) more++;
    }
    else if(toolArgs_equals(argv[0], "spiral")){ // spiral <count> [turns]
        double turns = (argc > 2)? values[0] : 2.0;
        for(int p = 0; p < count; p++){
            double t = (count > 1)? (double)p / (count - 1) : 0.0;
            if(!addPoint(layer, 0.5 + (0.42 * t * cos(DRAW_TAU * turns * t)), 0.5 + (0.42 * t * sin(DRAW_TAU * turns * t)))) more++;
        }
    }
    else { // x y pairs
        if(argc % 2 != 0){ printf("Points are x y pairs, %d values were given\n", argc); return FALSE; }
        for(int a = 0; a < argc; a += 2){
            double x, y;
            if(!toolArgs_float(argv[a], &x) || !toolArgs_float(argv[a + 1], &y) || x < -1.0 || x > 2.0 || y < -1.0 || y > 2.0){
                printf("Invalid point '%s %s', x and y are fractions of the image such as 0.25 0.8\n", argv[a], argv[a + 1]);
                return FALSE;
            }
            if(!addPoint(layer, x, y)) more++;
        }
    }
    if(more > 0) printf("  a layer holds %d points, %d were left out\n", DRAW_POINTS_MAX, more);
    return TRUE;
}

// --------------------------------   Generation    -------------------------------- //

static Rasteron_Image* drawGenerate(ImageSize size, const Rasteron_Image* baseImg){ return drawImgOp(size, &_draw, baseImg); }

static void drawAnimate(unsigned frame, unsigned frameCount){ _draw.progress = (double)(frame + 1) / (double)frameCount; } // the drawing appears stroke by stroke

static int drawApply(void){ // layers are now part of the applied image, which becomes the backdrop
    penCommit();
    if(_draw.layerCount > 0) printf("  %d layer(s) flattened into the applied image, new layers draw on top of it\n", _draw.layerCount);
    _draw.layerCount = 0;
    _draw.selected = -1;
    return TRUE;
}

// --------------------------------   Commands    -------------------------------- //

static int newLayerCommand(int kind, int argc, char** argv){
    if(argc < 2) return TOOL_Fail;
    Draw_Layer* layer = layerNew(kind);
    if(layer == NULL) return TOOL_Fail;
    if(!parsePoints(layer, argc - 1, argv + 1)) return TOOL_Fail; // history restores the state
    printLayer(_draw.layerCount - 1);
    return TOOL_Render;
}

static int cmd_points(int argc, char** argv){ return newLayerCommand(DRAW_Points, argc, argv); }
static int cmd_path(int argc, char** argv){ return newLayerCommand(DRAW_Path, argc, argv); }
static int cmd_brush(int argc, char** argv){ return newLayerCommand(DRAW_Brush, argc, argv); }

static int cmd_add(int argc, char** argv){
    int index;
    if(argc < 2 || !layerArg(1, argv, &index)) return TOOL_Fail;
    if(!parsePoints(&_draw.layers[index], argc - 1, argv + 1)) return TOOL_Fail;
    printLayer(index);
    return TOOL_Render;
}

static int cmd_layer(int argc, char** argv){
    if(argc > 1 && toolArgs_equals(argv[1], "none")){ penSelect(-1); puts("  no layer selected, parameters set the pen for new layers"); return TOOL_Ok; }
    int index;
    if(!layerArg(argc, argv, &index)) return TOOL_Fail;
    penSelect(index);
    printLayer(index);
    puts("  parameters now change this layer, 'layer none' or a new layer ends that");
    return TOOL_Ok;
}

static int cmd_layers(int argc, char** argv){
    if(_draw.layerCount == 0) puts("  no layers yet, add one with points, path, or brush (demo loads an example)");
    for(int l = _draw.layerCount - 1; l >= 0; l--) printLayer(l); // top layer first
    return TOOL_Ok;
}

static int cmd_remove(int argc, char** argv){
    int index;
    if(!layerArg(argc, argv, &index)) return TOOL_Fail;
    penCommit();
    for(int l = index; l + 1 < _draw.layerCount; l++) _draw.layers[l] = _draw.layers[l + 1];
    _draw.layerCount--;
    _draw.selected = -1;
    printf("  removed layer %d, %d left\n", index + 1, _draw.layerCount);
    return TOOL_Render;
}

static int cmd_clear(int argc, char** argv){
    penCommit();
    _draw.layerCount = 0;
    _draw.selected = -1;
    puts("  all layers removed");
    return TOOL_Render;
}

static int moveLayer(int argc, char** argv, int step){ // swaps a layer with its neighbor
    int index;
    if(!layerArg(argc, argv, &index)) return TOOL_Fail;
    int target = index + step;
    if(target < 0 || target >= _draw.layerCount){ printf("  layer %d is already at the %s\n", index + 1, (step > 0)? "top" : "bottom"); return TOOL_Ok; }
    penCommit();
    Draw_Layer swapped = _draw.layers[index];
    _draw.layers[index] = _draw.layers[target];
    _draw.layers[target] = swapped;
    if(_draw.selected == index) _draw.selected = target;
    else if(_draw.selected == target) _draw.selected = index;
    if(_draw.selected >= 0) _draw.pen = _draw.layers[_draw.selected].style;
    printf("  layer %d is now layer %d\n", index + 1, target + 1);
    return TOOL_Render;
}

static int cmd_raise(int argc, char** argv){ return moveLayer(argc, argv, 1); }
static int cmd_lower(int argc, char** argv){ return moveLayer(argc, argv, -1); }

static int cmd_toggle(int argc, char** argv){
    int index;
    if(!layerArg(argc, argv, &index)) return TOOL_Fail;
    _draw.layers[index].visible = !_draw.layers[index].visible;
    printLayer(index);
    return TOOL_Render;
}

static int cmd_nudge(int argc, char** argv){ // moves every point of the selected layer
    int index;
    double dx, dy;
    if(argc < 3 || !toolArgs_float(argv[1], &dx) || !toolArgs_float(argv[2], &dy) || fabs(dx) > 2.0 || fabs(dy) > 2.0) return TOOL_Fail;
    if(!layerArg(1, argv, &index)) return TOOL_Fail;
    for(int p = 0; p < _draw.layers[index].pointCount; p++){
        _draw.layers[index].points[p][0] += dx;
        _draw.layers[index].points[p][1] += dy;
    }
    printf("  layer %d moved by %g %g\n", index + 1, dx, dy);
    return TOOL_Render;
}

static void demoLayer(const Draw_Style* style, const double* coords, int count){ // adds a layer from x y pairs
    Draw_Layer* layer = layerNew(style->kind);
    if(layer == NULL) return;
    layer->style = *style;
    for(int p = 0; p < count; p++) addPoint(layer, coords[p * 2], coords[(p * 2) + 1]);
}

static int cmd_demo(int argc, char** argv){ // example drawing that uses every kind of layer
    static const double sweep[] = { 0.05, 0.8, 0.3, 0.6, 0.6, 0.85, 0.95, 0.65 };
    static const double dashes[] = { 0.05, 0.3, 0.3, 0.15, 0.55, 0.35, 0.9, 0.12 };
    static const double stroke[] = { 0.1, 0.55, 0.3, 0.42, 0.5, 0.6, 0.72, 0.45 };
    static const double stars[] = { 0.15, 0.12, 0.4, 0.08, 0.78, 0.2, 0.9, 0.4 };
    double ring[14];
    for(int p = 0; p < 7; p++){
        ring[p * 2] = 0.68 + (0.2 * cos(DRAW_TAU * p / 7.0));
        ring[(p * 2) + 1] = 0.32 + (0.2 * sin(DRAW_TAU * p / 7.0));
    }

    _draw = _drawDefaults;
    _draw.canvas[0] = 0xFF101820; _draw.canvas[1] = 0xFF1C2C3C; _draw.backdrop = BACKDROP_Gradient;
    Draw_Style style = _drawDefaults.pen;

    style.kind = DRAW_Brush; style.brush = BRUSH_Chalk; style.colors[0] = 0xFF2A6F97; style.colors[1] = 0xFF61A5C2; // wide chalk sweep
    style.gradient = GRADIENT_Along; style.width = 0.16; style.opacity = 0.55; style.line = LINE_Curve;
    demoLayer(&style, sweep, 4);

    style = _drawDefaults.pen; // translucent filled loop
    style.kind = DRAW_Path; style.line = LINE_Loop; style.fill = TRUE; style.colors[0] = 0xFFFFD166; style.colors[1] = 0xFFEF476F;
    style.gradient = GRADIENT_Across; style.width = 0.01; style.opacity = 0.35; style.blend = BLEND_Screen;
    demoLayer(&style, ring, 7);

    style = _drawDefaults.pen; // glowing dots on the corners of the loop
    style.kind = DRAW_Points; style.dot = DOT_Soft; style.colors[0] = 0xFF06D6A0; style.width = 0.07; style.blend = BLEND_Add;
    demoLayer(&style, ring, 7);

    style = _drawDefaults.pen; // dashed wavy path
    style.kind = DRAW_Path; style.line = LINE_Curve; style.colors[0] = 0xFFFFFFFF; style.colors[1] = 0xFF61A5C2; style.gradient = GRADIENT_Along;
    style.width = 0.008; style.cut = 2.0; style.wave = 1.5; style.frequency = 9.0;
    demoLayer(&style, dashes, 4);

    style = _drawDefaults.pen; // bristle brush stroke
    style.kind = DRAW_Brush; style.brush = BRUSH_Bristle; style.line = LINE_Curve; style.colors[0] = 0xFFEF476F; style.colors[1] = 0xFFFFD166;
    style.gradient = GRADIENT_Along; style.width = 0.05;
    demoLayer(&style, stroke, 4);

    style = _drawDefaults.pen; // stars
    style.kind = DRAW_Points; style.dot = DOT_Star; style.colors[0] = 0xFFFFFFFF; style.width = 0.035;
    demoLayer(&style, stars, 4);

    cmd_layers(0, NULL);
    return TOOL_Render;
}

static const Tool_Command _drawCommands[] = {
    { "points", "<x y ...|generator>", "new layer of dots (generators: rand <n>, circle <n> [radius] [x y], grid <cols> <rows>, spiral <n> [turns])", cmd_points },
    { "path", "<x y ...|generator>", "new layer with a line through the points", cmd_path },
    { "brush", "<x y ...|generator>", "new layer with a textured brush stroke through the points", cmd_brush },
    { "add", "<x y ...|generator>", "append points to the selected layer, else the top one", cmd_add },
    { "layer", "<number|top|bottom|none>", "select a layer so parameters change it instead of the pen", cmd_layer },
    { "layers", "", "list layers from top to bottom, * marks the selected one", cmd_layers },
    { "remove", "[number]", "remove a layer (default: the selected one, else the top one)", cmd_remove },
    { "clear", "", "remove all layers", cmd_clear },
    { "raise", "[number]", "move a layer one step up", cmd_raise },
    { "lower", "[number]", "move a layer one step down", cmd_lower },
    { "toggle", "[number]", "hide or unhide a layer", cmd_toggle },
    { "nudge", "<dx> <dy>", "move the selected layer, else the top one", cmd_nudge },
    { "demo", "", "replace the drawing with an example", cmd_demo },
};

// --------------------------------   Tool Description    -------------------------------- //

static const Tool_Desc _drawTool = {
    "draw", "DrawTool", "layers of points, paths, and brushes, based on DrawTest (demo loads an example)",
    &_draw, &_drawDefaults, sizeof(Draw_State),
    _drawParams, TOOL_COUNT(_drawParams),
    _drawCommands, TOOL_COUNT(_drawCommands),
    drawGenerate, drawAnimate, drawApply, NULL, NULL
};

int main(int argc, char** argv) { return tool_run(&_drawTool, argc, argv); }
