#define RADIAL_SEGS 10.0
#define RADIAL_DIST 10.0
#define RADIAL_INVOKE 1024
#define AB_ARGS unsigned a, unsigned b
#define ABC_ARGS unsigned a, unsigned b, unsigned c

#include "../Tool.h"

// --------------------------------   Radial State    -------------------------------- //

enum RADIAL_Mode {
    RADIAL_Rings = 0, RADIAL_Spiral = 1, RADIAL_Star = 2, RADIAL_Tunnel = 3, RADIAL_Petals = 4, RADIAL_Vortex = 5, // periodic modes
    RADIAL_Swirl = 6, RADIAL_Spirals = 7, RADIAL_Hypnosis = 8, RADIAL_Radial = 9, RADIAL_Mandala = 10 // modes from HypnoTest
};

enum RADIAL_Wave { WAVE_Sin = 0, WAVE_Cos = 1, WAVE_Tan = 2, WAVE_Saw = 3, WAVE_Triangle = 4, WAVE_Square = 5, WAVE_Pulse = 6, WAVE_Arc = 7 };

enum RADIAL_Effect { EFFECT_None = 0, EFFECT_Invert = 1, EFFECT_Grey = 2, EFFECT_Posterize = 3, EFFECT_Smooth = 4, EFFECT_Vignette = 5, EFFECT_Glow = 6, EFFECT_Channel = 7 };

enum RADIAL_Mix { MIX_None = 0, MIX_Blend = 1, MIX_Fuse = 2, MIX_Add = 3, MIX_Diff = 4, MIX_Mult = 5, MIX_Xor = 6, MIX_And = 7, MIX_Or = 8, MIX_Warp = 9 };

typedef struct {
    int mode;
    unsigned colors[3]; // inner, outer, and distance colors
    int wave; // periodic function
    int symmetry; // symmetry lines around the center
    double frequency; // ring density
    double twist; // angular twist with depth
    double recede; // depth falloff toward the center, 0 is flat and 1 is a perspective tunnel
    double phase; // radians, animated by encode and frames
    double center[2]; // offset of the vanishing point
    double zoom;
    double fog; // fades the center into the distance color
    int variant; // mix or interpolation function for HypnoTest modes
    int iterations; // hypnosis iterations
    int effect;
    double strength; // effect strength
    int mix; // combination with applied image
} Radial_State;

extern Radial_State _radial;

// --------------------------------   Periodic Modes (see Radial.c)    -------------------------------- //

double radialWave(int wave, double t); // periodic function mapped to [0, 1]
Rasteron_Image* periodicImgOp(ImageSize size); // rings, spiral, star, tunnel, petals, vortex

// --------------------------------   HypnoTest Modes (see Radial.c)    -------------------------------- //

// Spirals

unsigned spirals(double x, double y);
Rasteron_Image* spiralsImgOp(ImageSize size, double s, double d);

// Swirls

unsigned swirly(double x, double y);
Rasteron_Image* swirlyImgOp(ImageSize size, double swirlFactor, unsigned color1, unsigned color2);

// Radial

static unsigned d = 0xFF666666;
static unsigned radialMix1(ABC_ARGS){ return diff_colors(diff_colors(a, d), c - b); } // starting
static unsigned radialMix2(ABC_ARGS){ return diff_colors(diff_colors(b, c), d + a); } // reverse
static unsigned radialMix3(ABC_ARGS){ return diff_colors(diff_colors(c, a), b * d); } // odds and evens
static unsigned radialMix4(ABC_ARGS){ return diff_colors(diff_colors(d, b), (unsigned)pow(a, 1.0 / (double)((c & 0xFF) + 1))); } // evens and odds
static unsigned radialMix5(ABC_ARGS){ return diff_colors(c - b, diff_colors(a, d)); } // starting alt
static unsigned radialMix6(ABC_ARGS){ return diff_colors(d + a, diff_colors(b, c)); } // reverse alt
static unsigned radialMix7(ABC_ARGS){ return diff_colors(b * d, diff_colors(c, a)); } // odds and evens alt
static unsigned radialMix8(ABC_ARGS){ return diff_colors((unsigned)pow(a, 1.0 / (double)((c & 0xFF) + 1)), diff_colors(d, b)); } // evens and odds alt
static unsigned radialMix9(ABC_ARGS){ return (a + b > c + d)? blend_colors(a, b, 0.5) : fuse_colors(c, d, 0.5); } // inbetween

Rasteron_Image* radialImgOp(ImageSize size, unsigned colors[3], mixCallback3 mix_callback);

// Hypnosis

static unsigned hypnoticMix1(AB_ARGS) { return sqroot_colors(a, b); }
static unsigned hypnoticMix2(AB_ARGS) { return mult_colors(a, b); }
static unsigned hypnoticMix3(AB_ARGS) { return mult_rgb(a, b); }

Rasteron_Image* hypnosisImgOp(ImageSize size, unsigned color1, unsigned color2, unsigned short iters, mixCallback callback);

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

static float mandalaInterp1(unsigned i, float coords[2], float a, float d){ return tan((coords[0] + coords[1]) / ((i + 1) * RADIAL_INVOKE)) * pow(a, d); }
static float mandalaInterp2(unsigned i, float coords[2], float a, float d){ return tan((coords[0] + coords[1]) / ((i + 1) * RADIAL_INVOKE)) * pow(d, a); }
static float mandalaInterp3(unsigned i, float coords[2], float a, float d){ return sin((coords[0] - coords[1]) / ((i + 1) * RADIAL_INVOKE)) * pow(a, d); }
static float mandalaInterp4(unsigned i, float coords[2], float a, float d){ return cos((coords[0] - coords[1]) / ((i + 1) * RADIAL_INVOKE)) * pow(d, a); }
static float mandalaInterp5(unsigned i, float coords[2], float a, float d) { return (float)fabs(a / d) - fabs(coords[0] / coords[1]); }
static float mandalaInterp6(unsigned i, float coords[2], float a, float d) { return (float)fabs(a / d) + fabs(coords[0] / coords[1]); }
static float mandalaInterp7(unsigned i, float coords[2], float a, float d) { return (float)fabs(a / d) * fabs(coords[0] / coords[1]); }

typedef float (*mandalaInterpCallback)(unsigned, float[2], float, float);

unsigned mandalaMap(double x, double y);
Rasteron_Image* mandalaImgOp(ImageSize size, coordCallback coord_callback, mixCallback mix_callback, mandalaInterpCallback interp_callback);

// Variant lookups, variant counts from 1

mixCallback3 radialMixVariant(int variant); // 9 variants
mixCallback hypnoticMixVariant(int variant); // 3 variants
mixCallback mandalaMixVariant(int variant); // 7 variants
mandalaInterpCallback mandalaInterpVariant(int variant); // 7 variants

// --------------------------------   Post Processing (see Radial.c)    -------------------------------- //

Rasteron_Image* spinImgOp(ref_image_t refImage, double angle, double xCenter, double yCenter); // rotates about a point
Rasteron_Image* radialEffectImgOp(ref_image_t refImage, int effect, double strength, double xCenter, double yCenter);
