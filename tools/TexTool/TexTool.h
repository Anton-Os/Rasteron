#define TEXTOOL_OCTAVES 3
#define TEXTOOL_COUNT 10
#define TEXTOOL_POWER 3
#define TEXTOOL_PURTURB_X 0.0 // replace with variable
#define TEXTOOL_PURTURB_Y 0.0 // replace with variable

#include "Rasteron.h"

// Noise Function Modifiers

static float levelsNoiseMod(float value){ return (value < 0.25)? 0.0 : (value > 0.75)? 1.0 : 0.5; }

static float rangeNoiseMod(float value){ return fabs((value - 0.5F) * 1.5F); }

static float powerNoiseMod(float value){ return pow(value, TEXTOOL_POWER); }

static float trigNoiseMod(float value){ return abs(sin(value * 1.57079632 * TEXTOOL_POWER)); }

static float quiltNoiseMod(float value){ 
	static float m = 0.0;
	float newMod = (value / m > 1.0)? value + (m * 0.5) : value - (m * 0.5);
	m = value;
	return newMod;
}

/* static float sinMod(float value){ return sin(value); }
static float cosMod(float value){ return cos(value); }
static float tanMod(float value){ return tan(value); } */

// Mix Functions

static unsigned invertMix(unsigned color1, unsigned color2){ return (0xFFFFFFFF - ((color1 > color2)? color_invert(color1 - color2) : color_invert(color2 - color1))) | 0xFF000000; }

static unsigned wavyMix(unsigned color1, unsigned color2){ return fuse_colors(color1 + color2, color1 - color2, 0.5); }

static unsigned powerMix(unsigned color1, unsigned color2) { return (unsigned)pow((double)color1, (double)((color2 % 8) + 1)); }

static uint32_t assimMix_rgb(uint32_t color1, uint32_t color2){ 
	uint8_t red = ((color1 & RED_CHANNEL) >> 16) + ((color2 & RED_CHANNEL) >> 16);
	uint8_t green = ((color1 & GREEN_CHANNEL) >> 8) - ((color2 & GREEN_CHANNEL) >> 8);
	uint8_t blue = (color1 & BLUE_CHANNEL) * (color2 & BLUE_CHANNEL);

	uint32_t result = ((0xFF << 24) | (red << 16) | (green << 8) | blue);
    return result;
}

// Image Generation Functions

Rasteron_Image* purturbNoiseImgOp(ImageSize size, ColorGrid grid, noiseCallback callback, double xOff, double yOff);

// Tool Argment Image Funcions

Rasteron_Image* texArgs(char* args);