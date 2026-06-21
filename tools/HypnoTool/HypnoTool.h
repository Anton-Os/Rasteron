#define RADIAL_SEGS 10.0
#define RADIAL_DIST 10.0
#define RADIAL_INVOKE 1024
#define AB_ARGS unsigned a, unsigned b
#define ABCD_ARGS unsigned a, unsigned b, unsigned c, unsigned d

#include "Rasteron.h"

// Spirals

unsigned spirals(double x, double y); // See Hypno.c
Rasteron_Image* spiralsImgOp(double s, double d); // See Hypno.c

// Swirls

unsigned swirly(double x, double y); // See Hypno.c
Rasteron_Image* swirlyImgOp(double swirlFactor, unsigned color1, unsigned color2); // See Hypno.c

// Radial

static unsigned radialMix1(ABCD_ARGS){ return diff_colors(diff_colors(a, d), c - b); } // starting
static unsigned radialMix2(ABCD_ARGS){ return diff_colors(diff_colors(b, c), d + a); } // reverse
static unsigned radialMix3(ABCD_ARGS){ return diff_colors(diff_colors(c, a), b * d); } // odds and evens
static unsigned radialMix4(ABCD_ARGS){ return diff_colors(diff_colors(d, b), pow(a, 1.0 / (double)(c & 0xFF))); } // evens and odds
static unsigned radialMix5(ABCD_ARGS){ return diff_colors(c - b, diff_colors(a, d)); } // starting alt
static unsigned radialMix6(ABCD_ARGS){ return diff_colors(d + a, diff_colors(b, c)); } // reverse alt
static unsigned radialMix7(ABCD_ARGS){ return diff_colors(b * d, diff_colors(c, a)); } // odds and evens alt
static unsigned radialMix8(ABCD_ARGS){ return diff_colors(pow(a, 1.0 / (double)(c & 0xFF)), diff_colors(d, b)); } // evens and odds alt
static unsigned radialMix9(ABCD_ARGS){ return (a + b > c + d)? blend_colors(a, b, 0.5) : fuse_colors(c, d, 0.5); } // inbetween

Rasteron_Image* radialImgOp(unsigned colors[4], mixCallback4 mix_callback); // See Hypno.c

// Hypnosis

static unsigned hypnoticMix1(AB_ARGS) {return sqroot_colors(a, b); }
static unsigned hypnoticMix2(AB_ARGS) { return mult_colors(a, b); }
static unsigned hypnoticMix3(AB_ARGS) { return mult_rgb(a, b); }

Rasteron_Image* hypnosisImgOp(unsigned color1, unsigned color2, unsigned short iters, mixCallback callback); // See Hypno.c

// Mandala

static unsigned mandalaMix1(AB_ARGS){ return a + b; }
static unsigned mandalaMix2(AB_ARGS){ return a * b; }
static unsigned mandalaMix3(AB_ARGS){ return sqroot_colors(a, b); }
static unsigned mandalaMix4(AB_ARGS){ return (a * 2) - (b / 2); }
static unsigned mandalaMix5(AB_ARGS){ return (a > b)? color_invert(a) : color_invert(b); }
static unsigned mandalaMix6(AB_ARGS){ return (a * color_invert(b) > color_invert(a) * b)? a : b; }
static unsigned mandalaMix7(AB_ARGS){ return (fuse_colors(a, b, 0.5F) > blend_colors(a, b, 0.5F))? a : b; }
static unsigned restoreMix(AB_ARGS){
    static unsigned lastColor;
    unsigned newColor = (a == 0xFF000000 || a == NO_COLOR || a == lastColor)? b : a;
    lastColor = a;
    return newColor;
}

static float mandalaInterp1(unsigned i, float coords[2], float a, float d){ return tan((coords[0] + coords[1]) / (i * RADIAL_INVOKE)) * pow(a, d); }
static float mandalaInterp2(unsigned i, float coords[2], float a, float d){ return tan((coords[0] + coords[1]) / (i * RADIAL_INVOKE)) * pow(d, a); }
static float mandalaInterp3(unsigned i, float coords[2], float a, float d){ return sin((coords[0] - coords[1]) / (i * RADIAL_INVOKE)) * pow(a, d); }
static float mandalaInterp4(unsigned i, float coords[2], float a, float d){ return cos((coords[0] - coords[1]) / (i * RADIAL_INVOKE)) * pow(d, a); }
static float mandalaInterp5(unsigned i, float coords[2], float a, float d){ return tan((coords[0] * coords[1]) / (i * RADIAL_INVOKE)) * pow(d, a); }
static float mandalaInterp6(unsigned i, float coords[2], float a, float d){ return sin((coords[0] / coords[1]) / (i * RADIAL_INVOKE)) * pow(a, d); }
static float mandalaInterp7(unsigned i, float coords[2], float a, float d){ return cos((pow(coords[0], coords[1])) / (i * RADIAL_INVOKE)) * pow(d, a); }

unsigned mandalaMap(double x, double y); // See Hypno.c

Rasteron_Image* mandalaImgOp(coordCallback coord_callback, mixCallback mix_callback, float (*interp_callback)(unsigned, float[2], float, float)); // See Hypno.c