#define RAY_MAX 64 // most rays in one image
#define RAY_CUSTOM_MAX 32 // rays added with addray
#define XYZ_ARGS double x, double y, double z

#include "../Tool.h"

// --------------------------------   Ray State    -------------------------------- //

enum RAY_Arrange { ARRANGE_Circle = 0, ARRANGE_Spiral = 1, ARRANGE_Line = 2, ARRANGE_Grid = 3, ARRANGE_Random = 4, ARRANGE_Test = 5, ARRANGE_Custom = 6 };
enum RAY_Map { MAP_Abs = 0, MAP_Linear = 1, MAP_Sin = 2, MAP_Cos = 3, MAP_Tan = 4, MAP_Square = 5, MAP_Sqrt = 6, MAP_Exp = 7, MAP_Log = 8, MAP_Saw = 9, MAP_Inverse = 10, MAP_Step = 11 };
enum RAY_Blend { BLEND_Cast = 0, BLEND_Blend = 1 }; // raycastImgOp and rayblendImgOp from RayTest

typedef struct {
    int count; // number of rays
    int arrange; // how ray origins are placed
    double radius; // spread of ray origins
    double angle; // rotation of the arrangement in degrees, animated by frames/encode
    double arc; // degrees covered by circle arrangement
    double tilt; // elevation of rays toward z in degrees
    int func; // RayTest ray function
    int raymix; // combines the colors of each ray
    int map[3]; // mapping algorithm for x y z
    double scale[3]; // scale of x y z before mapping
    double offset[3]; // offset of x y z after mapping
    double dist; // eye distance, larger flattens the projection
    unsigned colors[2];
    int blend; // cast or blend (applies mix to every ray)
    int mix; // combination with applied image, and rays in blend mode (RayTest bottom row)
    int customCount;
    double custom[RAY_CUSTOM_MAX][3]; // rays added with addray
} Ray_State;

extern Ray_State _ray;

extern unsigned rayColor1; // used by RayTest ray functions, synced from _ray
extern unsigned rayColor2;

// --------------------------------   RayTest Functions    -------------------------------- //
//
//      Same formulas as RayTest, with rayPow guarding negative bases that would produce NaN (black) pixels

static double rayPow(double base, double exponent){
    double result = pow(fabs(base), exponent);
    return (result != result || result > 1e6)? 1e6 : result; // NaN or overflow saturates
}

static unsigned rayFunc1(XYZ_ARGS) { return blend_colors(rayColor1, rayColor2, rayPow(x + y + z, x * y * z) * 0.5); }
static unsigned rayFunc2(XYZ_ARGS) { return blend_colors(rayColor1, rayColor2, rayPow(x * y * z, x + y + z) * 0.5); }
static unsigned rayFunc3(XYZ_ARGS) { return blend_colors(rayColor1, rayColor2, rayPow(x - (y + z), x + y + z) * 0.5); }
static unsigned rayFunc4(XYZ_ARGS) { return blend_colors(rayColor1, rayColor2, rayPow(x + (y - z), -x + y + z) * 0.5); }
static unsigned rayFunc5(XYZ_ARGS) { return blend_colors(rayColor1, rayColor2, rayPow((x / y) * z, x * (y / z)) * 0.5); }
static unsigned rayFunc6(XYZ_ARGS) { return blend_colors(rayColor1, rayColor2, rayPow(x / (y + z), y / (x - z)) * 0.5); }
static unsigned rayFunc7(XYZ_ARGS) { return blend_colors(rayColor1, rayColor2, rayPow(sin((x + y + z) * 10), cos(x * y * z * 10)) * 0.5);}
static unsigned rayFunc8(XYZ_ARGS) { return blend_colors(rayColor1, rayColor2, rayPow(cos((x + y + z) * 10), sin(x * y * z * 10)) * 0.5); }
static unsigned rayFunc9(XYZ_ARGS) { return root_colors(rayColor1, rayColor2, (x * y) / z); }
static unsigned rayFunc10(XYZ_ARGS) { return root_colors(rayColor1, rayColor2, fabs(x - y) + tan(z * 10)); }
static unsigned rayFunc11(XYZ_ARGS) { return blend_colors(rayColor1, rayColor2, 0.5 + 0.5 * sin((x * x + y * y) * 40.0 / (z + 0.05))); } // rings
static unsigned rayFunc12(XYZ_ARGS) { return blend_colors(rayColor1, rayColor2, fabs(sin(atan2(y, x) * 8.0 + z * 20.0))); } // beams

static unsigned invertMix1(unsigned color1, unsigned color2) { return (0xFFFFFFFF - ((color1 > color2)? color_invert(color1 - color2) : color_invert(color2 - color1))) | 0xFF000000; }
static unsigned invertMix2(unsigned color1, unsigned color2) { return (0xFFFFFFFF - ((color1 > color2)? color_invert(color1 + color2) : color_invert(color2 - color1))) | 0xFF000000; }
static unsigned invertMix3(unsigned color1, unsigned color2) { return (0xFFFFFFFF - ((color1 > color2)? color_invert(color1 + color2) : color_invert(color2 * color1))) | 0xFF000000; }

// --------------------------------   Image Operations (see Ray.c)    -------------------------------- //

unsigned rayPoints(float* points, unsigned maxRays); // fills x y z per ray from _ray settings, returns ray count
double rayMap(int map, double value); // mapping algorithm for one axis
Rasteron_Image* raymapImgOp(ImageSize size, const float* points, unsigned pointCount, coordCallback3 rayFunc, mixCallback rayMixFunc, mixCallback mixFunc);
