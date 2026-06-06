#include "Rasteron.h"

extern int mode;
extern int rayColor1;
extern int rayColor2;

extern mixCallback vecMixFunc;

// Ray Functions

static unsigned rayFunc1(double x, double y, double z) {
    unsigned color = blend_colors(rayColor1, rayColor2, pow(x + y + z, x * y * z) * 0.5);
    return vecMixFunc((mode < 0)? rayColor1 : color, color);
}
static unsigned rayFunc2(double x, double y, double z) {
    unsigned color = blend_colors(rayColor1, rayColor2, pow(x * y * z, x + y + z) * 0.5);
    return vecMixFunc((mode < 0)? rayColor1 : color, color);
}
static unsigned rayFunc3(double x, double y, double z) {
    unsigned color = blend_colors(rayColor1, rayColor2, pow(x - (y + z), x + y + z) * 0.5);
    return vecMixFunc((mode < 0)? rayColor1 : color, color);
}
static unsigned rayFunc4(double x, double y, double z) {
    unsigned color = blend_colors(rayColor1, rayColor2, pow(x + (y - z), -x + y + z) * 0.5);
    return vecMixFunc((mode < 0)? rayColor1 : color, color);
}
static unsigned rayFunc5(double x, double y, double z) {
    unsigned color = blend_colors(rayColor1, rayColor2, pow((x / y) * z, x * (y / z)) * 0.5);
    return vecMixFunc((mode < 0)? rayColor1 : color, color);
}
static unsigned rayFunc6(double x, double y, double z) {
    unsigned color = blend_colors(rayColor1, rayColor2, pow(x / (y + z), y / (x - z)) * 0.5);
    return vecMixFunc((mode < 0)? rayColor1 : color, color);
}
static unsigned rayFunc7(double x, double y, double z) {
    unsigned color = blend_colors(rayColor1, rayColor2, pow(sin((x + y + z) * 10), cos(x * y * z * 10)) * 0.5);
    return vecMixFunc((mode < 0)? rayColor1 : color, color);
}
static unsigned rayFunc8(double x, double y, double z) {
    unsigned color = blend_colors(rayColor1, rayColor2, pow(cos((x + y + z) * 10), sin(x * y * z * 10)) * 0.5);
    return vecMixFunc((mode < 0)? rayColor1 : color, color);
}
static unsigned rayFunc9(double x, double y, double z) {
    unsigned color = root_colors(rayColor1, rayColor2, (x * y) / z);
    return vecMixFunc((mode < 0)? rayColor1 : color, color);
}
static unsigned rayFunc10(double x, double y, double z) {
    unsigned color = root_colors(rayColor1, rayColor2, (x - y) + z);
    return vecMixFunc((mode < 0)? rayColor1 : color, color);
}

// Mix Functions

static unsigned invertMix1(unsigned color1, unsigned color2) { return (0xFFFFFFFF - ((color1 > color2)? color_invert(color1 - color2) : color_invert(color2 - color1))) | 0xFF000000; }

static unsigned invertMix2(unsigned color1, unsigned color2) { return (0xFFFFFFFF - ((color1 > color2)? color_invert(color1 + color2) : color_invert(color2 - color1))) | 0xFF000000; }

static unsigned invertMix3(unsigned color1, unsigned color2) { return (0xFFFFFFFF - ((color1 > color2)? color_invert(color1 + color2) : color_invert(color2 * color1))) | 0xFF000000; }
