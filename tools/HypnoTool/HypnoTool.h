#define RADIAL_SEGS 10.0
#define RADIAL_DIST 10.0
#define RADIAL_INVOKE 1024

#include "Rasteron.h"

// Spirals

unsigned spirals(double x, double y);
Rasteron_Image* spiralsImgOp(double s, double d);

// Swirls

unsigned swirly(double x, double y);
Rasteron_Image* swirlyImgOp(double swirlFactor, unsigned color1, unsigned color2);

// Radial

static unsigned radialMix1(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return diff_colors(diff_colors(c1, c4), c3 - c2); } // starting
static unsigned radialMix2(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return diff_colors(diff_colors(c2, c3), c4 + c1); } // reverse
static unsigned radialMix3(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return diff_colors(diff_colors(c3, c1), c2 * c4); } // odds and evens
static unsigned radialMix4(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return diff_colors(diff_colors(c4, c2), pow(c1, 1.0 / (double)(c3 & 0xFF))); } // evens and odds
static unsigned radialMix5(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return diff_colors(c3 - c2, diff_colors(c1, c4)); } // starting alt
static unsigned radialMix6(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return diff_colors(c4 + c1, diff_colors(c2, c3)); } // reverse alt
static unsigned radialMix7(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return diff_colors(c2 * c4, diff_colors(c3, c1)); } // odds and evens alt
static unsigned radialMix8(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return diff_colors(pow(c1, 1.0 / (double)(c3 & 0xFF)), diff_colors(c4, c2)); } // evens and odds alt
static unsigned radialMix9(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return (c1 + c2 > c3 + c4)? blend_colors(c1, c2, 0.5) : fuse_colors(c3, c4, 0.5); } // inbetween

Rasteron_Image* radialImgOp(unsigned colors[4], mixCallback4 mix_callback);

// Hypnosis

static unsigned hypnoticMix1(unsigned color1, unsigned color2) {return sqroot_colors(color1, color2); }
static unsigned hypnoticMix2(unsigned color1, unsigned color2) { return mult_colors(color1, color2); }
static unsigned hypnoticMix3(unsigned color1, unsigned color2) { return mult_rgb(color1, color2); }

Rasteron_Image* hypnosisImgOp(unsigned color1, unsigned color2, unsigned short iters, mixCallback callback);

// Mandala

static unsigned mandalaMix1(unsigned c1, unsigned c2){ return c1 + c2; }
static unsigned mandalaMix2(unsigned c1, unsigned c2){ return c1 * c2; }
static unsigned mandalaMix3(unsigned c1, unsigned c2){ return sqroot_colors(c1, c2); }
static unsigned mandalaMix4(unsigned c1, unsigned c2){ return (c1 * 2) - (c2 / 2); }
static unsigned mandalaMix5(unsigned c1, unsigned c2){ return (c1 > c2)? color_invert(c1) : color_invert(c2); }
static unsigned mandalaMix6(unsigned c1, unsigned c2){ return (c1 * color_invert(c2) > color_invert(c1) * c2)? c1 : c2; }
static unsigned mandalaMix7(unsigned c1, unsigned c2){ return (fuse_colors(c1, c2, 0.5F) > blend_colors(c1, c2, 0.5F))? c1 : c2; }
static unsigned restoreMix(unsigned c1, unsigned c2){
    static unsigned lastColor;
    unsigned newColor = (c1 == 0xFF000000 || c1 == NO_COLOR || c1 == lastColor)? c2 : c1;
    lastColor = c1;
    return newColor;
}

static float mandalaInterp1(unsigned i, float coords[2], float a, float d){ return tan((coords[0] + coords[1]) / (i * RADIAL_INVOKE)) * pow(a, d); }
static float mandalaInterp2(unsigned i, float coords[2], float a, float d){ return tan((coords[0] + coords[1]) / (i * RADIAL_INVOKE)) * pow(d, a); }
static float mandalaInterp3(unsigned i, float coords[2], float a, float d){ return sin((coords[0] - coords[1]) / (i * RADIAL_INVOKE)) * pow(a, d); }
static float mandalaInterp4(unsigned i, float coords[2], float a, float d){ return cos((coords[0] - coords[1]) / (i * RADIAL_INVOKE)) * pow(d, a); }
static float mandalaInterp5(unsigned i, float coords[2], float a, float d){ return tan((coords[0] * coords[1]) / (i * RADIAL_INVOKE)) * pow(d, a); }
static float mandalaInterp6(unsigned i, float coords[2], float a, float d){ return sin((coords[0] / coords[1]) / (i * RADIAL_INVOKE)) * pow(a, d); }
static float mandalaInterp7(unsigned i, float coords[2], float a, float d){ return cos((pow(coords[0], coords[1])) / (i * RADIAL_INVOKE)) * pow(d, a); }

unsigned mandalaMap(double x, double y);

Rasteron_Image* mandalaImgOp(coordCallback coord_callback, mixCallback mix_callback, float (*interp_callback)(unsigned, float[2], float, float));