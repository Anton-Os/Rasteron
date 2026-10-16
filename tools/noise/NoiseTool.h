#define TEXTOOL_OCTAVES 3
#define TEXTOOL_COUNT 10
#define NOISE_OCTAVES_MAX 10

#include "../Tool.h"

// --------------------------------   Noise State    -------------------------------- //

enum NOISE_Type {
    NOISE_Value = 0, NOISE_Perlin = 1, NOISE_Worley = 2, NOISE_White = 3, // base noises
    NOISE_Octave = 4, NOISE_Add = 5, NOISE_Diff = 6, NOISE_Mult = 7, NOISE_Low = 8, NOISE_Hi = 9, NOISE_Rand = 10, // library octave noises
    NOISE_Tiled = 11, NOISE_Crossed = 12, NOISE_Stepped = 13, NOISE_Perturb = 14 // library variants and TexTest perturb
};

enum NOISE_Material {
    MATERIAL_None = 0, MATERIAL_Grass = 1, MATERIAL_Ooze = 2, MATERIAL_Sand = 3, MATERIAL_Metal = 4, MATERIAL_Fabric = 5,
    MATERIAL_Wood = 6, MATERIAL_Marble = 7, MATERIAL_Stone = 8, MATERIAL_Water = 9, MATERIAL_Lava = 10, MATERIAL_Clouds = 11, MATERIAL_Rust = 12
};

enum NOISE_Modifier { NMOD_None = 0, NMOD_Levels = 1, NMOD_Range = 2, NMOD_Power = 3, NMOD_Trig = 4, NMOD_Quilt = 5 };

enum NOISE_Mix { NMIX_None = 0, NMIX_Mult = 1, NMIX_Qdroot = 2, NMIX_Assim = 3, NMIX_And = 4, NMIX_Or = 5, NMIX_Xor = 6, NMIX_Warp = 7, NMIX_Blend = 8 };

typedef struct {
    int type;
    int material; // callback shaping the noise into a surface, none shows raw noise
    unsigned colors[3]; // low, high, and accent colors of the range
    int octaves;
    double persistence; // amplitude kept per octave
    double lacunarity; // frequency growth per octave
    int cells[2]; // lattice cells across x and y
    int wrap; // lattice repeats so the texture tiles
    int gradients; // gradient directions on the perlin lattice
    double jitter; // worley feature point jitter inside each lattice cell
    double stretch[2]; // lattice scale along x and y
    double offset[2]; // lattice offset, animated by frames/encode
    double warp; // domain warp strength
    double perturb[2]; // TexTest perturb offsets for x and y
    int modifier; // TexTest noise modifier callbacks
    double power; // exponent for the power and trig modifiers
    double contrast;
    int mix; // combination with applied image, as in TexTest bottom row keys
} Noise_State;

extern Noise_State _noise;

// --------------------------------   TexTest Callbacks    -------------------------------- //

float levelsNoiseMod(float value);
float rangeNoiseMod(float value);
float powerNoiseMod(float value); // uses _noise.power
float trigNoiseMod(float value); // uses _noise.power
float quiltNoiseMod(float value);

static unsigned invertMix(unsigned color1, unsigned color2){ return (0xFFFFFFFF - ((color1 > color2)? color_invert(color1 - color2) : color_invert(color2 - color1))) | 0xFF000000; }
static unsigned wavyMix(unsigned color1, unsigned color2){ return fuse_colors(color1 + color2, color1 - color2, 0.5); }
static uint32_t assimMix_rgb(uint32_t color1, uint32_t color2){
	uint8_t red = ((color1 & RED_CHANNEL) >> 16) + ((color2 & RED_CHANNEL) >> 16);
	uint8_t green = ((color1 & GREEN_CHANNEL) >> 8) - ((color2 & GREEN_CHANNEL) >> 8);
	uint8_t blue = (color1 & BLUE_CHANNEL) * (color2 & BLUE_CHANNEL);
	return ((0xFFu << 24) | (red << 16) | (green << 8) | blue);
}

// --------------------------------   Lattice Noise (see Noise.c)    -------------------------------- //

void latticeSeed(int gradients); // shuffles the permutation table from rand(), sets gradient directions
double perlinNoise(double x, double y, int xPeriod, int yPeriod); // gradient noise in [-1, 1], period 0 disables wrapping
double fbmNoise(double x, double y); // octaves of perlin using _noise lattice settings, in [-1, 1]
double worleyNoise(double x, double y, double* secondDist); // nearest feature distance, optional second distance

// --------------------------------   Image Operations (see Noise.c)    -------------------------------- //

Rasteron_Image* purturbNoiseImgOp(ImageSize size, ColorGrid grid, noiseCallback callback, double xOff, double yOff); // from TexTest
Rasteron_Image* latticeImgOp(ImageSize size); // perlin, worley, and materials
noiseCallback noiseModifier(int modifier); // NULL for none
