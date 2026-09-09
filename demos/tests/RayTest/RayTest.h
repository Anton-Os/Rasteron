#include "Rasteron.h"

#define POINTARRAY_COUNT 12
#define XYZ_ARGS double x, double y, double z

extern int mode;
extern unsigned rayColor1;
extern unsigned rayColor2;

extern mixCallback mixFunc;
extern mixCallback rayMixFunc;
extern coordCallback3 rayFunc;

// Ray Functions

static unsigned rayFunc1(XYZ_ARGS) { return blend_colors(rayColor1, rayColor2, pow(x + y + z, x * y * z) * 0.5); }
static unsigned rayFunc2(XYZ_ARGS) { return blend_colors(rayColor1, rayColor2, pow(x * y * z, x + y + z) * 0.5); }
static unsigned rayFunc3(XYZ_ARGS) { return blend_colors(rayColor1, rayColor2, pow(x - (y + z), x + y + z) * 0.5); }
static unsigned rayFunc4(XYZ_ARGS) { return blend_colors(rayColor1, rayColor2, pow(x + (y - z), -x + y + z) * 0.5); }
static unsigned rayFunc5(XYZ_ARGS) { return blend_colors(rayColor1, rayColor2, pow((x / y) * z, x * (y / z)) * 0.5); }
static unsigned rayFunc6(XYZ_ARGS) { return blend_colors(rayColor1, rayColor2, pow(x / (y + z), y / (x - z)) * 0.5); }
static unsigned rayFunc7(XYZ_ARGS) { return blend_colors(rayColor1, rayColor2, pow(sin((x + y + z) * 10), cos(x * y * z * 10)) * 0.5);}
static unsigned rayFunc8(XYZ_ARGS) { return blend_colors(rayColor1, rayColor2, pow(cos((x + y + z) * 10), sin(x * y * z * 10)) * 0.5); }
static unsigned rayFunc9(XYZ_ARGS) { return root_colors(rayColor1, rayColor2, (x * y) / z); }
static unsigned rayFunc10(XYZ_ARGS) { return root_colors(rayColor1, rayColor2, abs(x - y) + tan(z * 10)); }

// Mix Functions

static unsigned invertMix1(unsigned color1, unsigned color2) { return (0xFFFFFFFF - ((color1 > color2)? color_invert(color1 - color2) : color_invert(color2 - color1))) | 0xFF000000; }
static unsigned invertMix2(unsigned color1, unsigned color2) { return (0xFFFFFFFF - ((color1 > color2)? color_invert(color1 + color2) : color_invert(color2 - color1))) | 0xFF000000; }
static unsigned invertMix3(unsigned color1, unsigned color2) { return (0xFFFFFFFF - ((color1 > color2)? color_invert(color1 + color2) : color_invert(color2 * color1))) | 0xFF000000; }

// Image Generation Functions

Rasteron_Image* raycastImgOp(ImageSize size, float* points, unsigned pointCount, double dist); // See Ray.c
Rasteron_Image* rayblendImgOp(ImageSize size, float* points, unsigned pointCount, double dist); // See Ray.c
