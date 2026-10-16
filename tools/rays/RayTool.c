#include "RayTool.h"

// --------------------------------   State    -------------------------------- //

static const Ray_State _rayDefaults = {
    4, ARRANGE_Test, 0.25, 0.0, 360.0, 30.0, // count, arrange, radius, angle, arc, tilt
    0, 1, // func ray1, raymix xor
    { 0, 0, 0 }, { 1.0, 1.0, 1.0 }, { 0.0, 0.0, 0.0 }, // map, scale, offset
    1.0, // dist
    { 0xFF101040, 0xFFFFD27F }, // colors
    BLEND_Cast, 0, // blend, mix
    0, { { 0.0, 0.0, 0.0 } } // custom rays
};

Ray_State _ray;

unsigned rayColor1 = 0xFF000000;
unsigned rayColor2 = 0xFFFFFFFF;

static const char* const _arrangeNames[] = { "circle", "spiral", "line", "grid", "random", "test", "custom", NULL };
static const char* const _funcNames[] = { "ray1", "ray2", "ray3", "ray4", "ray5", "ray6", "ray7", "ray8", "ray9", "ray10", "rings", "beams", NULL };
static const coordCallback3 _funcs[] = { rayFunc1, rayFunc2, rayFunc3, rayFunc4, rayFunc5, rayFunc6, rayFunc7, rayFunc8, rayFunc9, rayFunc10, rayFunc11, rayFunc12 };
static const char* const _rayMixNames[] = { "or", "xor", "addrgb", "add", "diff", "diffrgb", "invert1", "invert2", "invert3", "blend", "fuse", NULL };
static const mixCallback _rayMixes[] = { bit_colors_or, bit_colors_xor, add_rgb, add_colors, diff_colors, diff_rgb, invertMix1, invertMix2, invertMix3, blend_colors_eq, fuse_colors_eq };
static const char* const _mapNames[] = { "abs", "linear", "sin", "cos", "tan", "square", "sqrt", "exp", "log", "saw", "inverse", "step", NULL };
static const char* const _blendNames[] = { "cast", "blend", NULL };
static const char* const _mixNames[] = { "none", "mult", "multrgb", "add", "addrgb", "and", "or", "sqroot", NULL };
static const mixCallback _mixes[] = { NULL, mult_colors, mult_rgb, add_colors, add_rgb, bit_colors_and, bit_colors_or, sqroot_colors };

static const Tool_Param _rayParams[] = {
    TOOL_PARAM("count", PARAM_Int, Ray_State, count, 1, 0, RAY_MAX, NULL, "number of rays, 0 renders the bare vector field"),
    TOOL_PARAM("arrange", PARAM_Enum, Ray_State, arrange, 1, 0, 0, _arrangeNames, "placement of ray origins ('custom' uses addray)"),
    TOOL_PARAM("radius", PARAM_Float, Ray_State, radius, 1, 0, 4, NULL, "spread of ray origins"),
    TOOL_PARAM("angle", PARAM_Float, Ray_State, angle, 1, -3600, 3600, NULL, "rotation of the arrangement in degrees, animated by frames/encode"),
    TOOL_PARAM("arc", PARAM_Float, Ray_State, arc, 1, 1, 360, NULL, "degrees covered by the circle arrangement"),
    TOOL_PARAM("tilt", PARAM_Float, Ray_State, tilt, 1, -90, 90, NULL, "elevation of rays toward z in degrees"),
    TOOL_PARAM("func", PARAM_Enum, Ray_State, func, 1, 0, 0, _funcNames, "RayTest ray function (q to p keys) plus rings and beams"),
    TOOL_PARAM("raymix", PARAM_Enum, Ray_State, raymix, 1, 0, 0, _rayMixNames, "combines the colors of each ray (RayTest a to l keys)"),
    TOOL_PARAM("map", PARAM_Enum, Ray_State, map, 3, 0, 0, _mapNames, "mapping algorithm for x y z, e.g. map sin abs linear"),
    TOOL_PARAM("mapx", PARAM_Enum, Ray_State, map[0], 1, 0, 0, _mapNames, "mapping algorithm for x"),
    TOOL_PARAM("mapy", PARAM_Enum, Ray_State, map[1], 1, 0, 0, _mapNames, "mapping algorithm for y"),
    TOOL_PARAM("mapz", PARAM_Enum, Ray_State, map[2], 1, 0, 0, _mapNames, "mapping algorithm for z"),
    TOOL_PARAM("scale", PARAM_Float, Ray_State, scale, 3, -50, 50, NULL, "scale of x y z before mapping"),
    TOOL_PARAM("offset", PARAM_Float, Ray_State, offset, 3, -10, 10, NULL, "offset of x y z after mapping"),
    TOOL_PARAM("dist", PARAM_Float, Ray_State, dist, 1, 0.01, 20, NULL, "eye distance, larger flattens the projection"),
    TOOL_PARAM("colors", PARAM_Color, Ray_State, colors, 2, 0, 0, NULL, "the two ray colors"),
    TOOL_PARAM("blend", PARAM_Enum, Ray_State, blend, 1, 0, 0, _blendNames, "cast (raycastImgOp) or blend (rayblendImgOp, mix on each ray)"),
    TOOL_PARAM("mix", PARAM_Enum, Ray_State, mix, 1, 0, 0, _mixNames, "combine with the applied image (RayTest z to m keys)"),
};

// --------------------------------   Generation    -------------------------------- //

static Rasteron_Image* rayGenerate(ImageSize size, const Rasteron_Image* baseImg){
    float points[RAY_MAX * 3];
    unsigned count = rayPoints(points, RAY_MAX);
    rayColor1 = _ray.colors[0];
    rayColor2 = _ray.colors[1];

    mixCallback mixFunc = _mixes[_ray.mix];
    Rasteron_Image* image = raymapImgOp(size, points, count, _funcs[_ray.func], _rayMixes[_ray.raymix], (_ray.blend == BLEND_Blend)? mixFunc : NULL);

    if(mixFunc != NULL && baseImg != NULL && _ray.blend == BLEND_Cast){ // RayTest mixes with the saved image
        Rasteron_Image* mixedImg = mixImgOp(image, baseImg, mixFunc);
        RASTERON_DEALLOC(image);
        image = mixedImg;
    }
    return image;
}

static void rayAnimate(unsigned frame, unsigned frameCount){ _ray.angle += 360.0 * frame / frameCount; }

// --------------------------------   Commands    -------------------------------- //

static int cmd_addray(int argc, char** argv){
    double xyz[3] = { 0.0, 0.0, 0.0 };
    if(argc < 3) return TOOL_Fail;
    for(int a = 1; a < argc && a <= 3; a++) if(!toolArgs_float(argv[a], &xyz[a - 1]) || fabs(xyz[a - 1]) > 10.0) return TOOL_Fail;
    if(_ray.customCount >= RAY_CUSTOM_MAX){ printf("  at most %d custom rays, clearrays to start over\n", RAY_CUSTOM_MAX); return TOOL_Ok; }
    for(int c = 0; c < 3; c++) _ray.custom[_ray.customCount][c] = xyz[c];
    _ray.customCount++;
    _ray.arrange = ARRANGE_Custom;
    printf("  ray %d at %g %g %g\n", _ray.customCount, xyz[0], xyz[1], xyz[2]);
    return TOOL_Render;
}

static int cmd_clearrays(int argc, char** argv){
    _ray.customCount = 0;
    puts("  custom rays cleared");
    return TOOL_Render;
}

static int cmd_rays(int argc, char** argv){ // lists ray origins
    float points[RAY_MAX * 3];
    srand((unsigned)_toolCommon.seed);
    unsigned count = rayPoints(points, RAY_MAX);
    for(unsigned r = 0; r < count; r++) printf("  ray %2u: %7.3f %7.3f %7.3f\n", r + 1, points[r * 3], points[r * 3 + 1], points[r * 3 + 2]);
    if(count == 0) puts("  no rays, vector field only");
    return TOOL_Ok;
}

static int cmd_recolor(int argc, char** argv){ // RayTest ',' key
    _ray.colors[0] = RAND_COLOR();
    _ray.colors[1] = color_invert(_ray.colors[0]);
    printf("  colors = %08X %08X\n", _ray.colors[0], _ray.colors[1]);
    return TOOL_Render;
}

static const Tool_Command _rayCommands[] = {
    { "addray", "<x> <y> [z]", "add a custom ray origin and switch to arrange custom", cmd_addray },
    { "clearrays", "", "remove custom rays", cmd_clearrays },
    { "rays", "", "list the current ray origins", cmd_rays },
    { "recolor", "", "random color with its inverse (RayTest ',' key)", cmd_recolor },
};

// --------------------------------   Tool Description    -------------------------------- //

static const Tool_Desc _rayTool = {
    "rays", "RayTool", "ray fields with configurable ray count, angles, and x y z mapping, based on RayTest",
    &_ray, &_rayDefaults, sizeof(Ray_State),
    _rayParams, TOOL_COUNT(_rayParams),
    _rayCommands, TOOL_COUNT(_rayCommands),
    rayGenerate, rayAnimate, NULL, NULL, NULL
};

int main(int argc, char** argv) { return tool_run(&_rayTool, argc, argv); }
