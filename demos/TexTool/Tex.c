static float levelsNoiseMod(float value){ return (value < 0.25)? 0.0 : (value > 0.75)? 1.0 : 0.5; }

static float rangeNoiseMod(float value){ return fabs((value - 0.5F) * 2.0F); }

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

unsigned invertMix(unsigned color1, unsigned color2){ return (0xFFFFFFFF - ((color1 > color2)? color_invert(color1 - color2) : color_invert(color2 - color1))) | 0xFF000000; }

unsigned wavyMix(unsigned color1, unsigned color2){ return fuse_colors(color1 + color2, color1 - color2, 0.5); }

uint32_t asm_rgb(uint32_t color1, uint32_t color2){ 
	uint8_t red = ((color1 & RED_CHANNEL) >> 16) + ((color2 & RED_CHANNEL) >> 16);
	uint8_t green = ((color1 & GREEN_CHANNEL) >> 8) - ((color2 & GREEN_CHANNEL) >> 8);
	uint8_t blue = (color1 & BLUE_CHANNEL) * (color2 & BLUE_CHANNEL);

	uint32_t result = ((0xFF << 24) | (red << 16) | (green << 8) | blue);
    return result;
}

Rasteron_Image* texImgOp(char mode, ColorGrid* grid){
    // ColorGrid grid = { pow(2, _dimens[0]), pow(2, _dimens[1]),  _swatch.colors[SWATCH_Light],  _swatch.colors[SWATCH_Dark] };

    switch(tolower(mode)){
        case 'a': return noiseImgOp((ImageSize){ 1024, 1024 }, *grid); break;
        // case 's': return noiseImgOp_crossed((ImageSize){ 1024, 1024 }, *grid); break;
        // case 'd': return noiseImgOp_stepped((ImageSize){ 1024, 1024 }, *grid); break;
        case 's': return noiseExtImgOp((ImageSize){ 1024, 1024 }, *grid, powerNoiseMod); break;
        case 'd': return noiseExtImgOp((ImageSize){ 1024, 1024 }, *grid, trigNoiseMod); break;
        case 'f': return noiseExtImgOp((ImageSize){ 1024, 1024 }, *grid, rangeNoiseMod); break;
        case 'g': return noiseImgOp_add((ImageSize){ 1024, 1024 }, *grid, TEXTOOL_OCTAVES); break;
        case 'h': return noiseImgOp_diff((ImageSize){ 1024, 1024 }, *grid, TEXTOOL_OCTAVES); break;
        case 'j': return noiseImgOp_low((ImageSize){ 1024, 1024 }, *grid, TEXTOOL_OCTAVES); break;
        case 'k': return noiseImgOp_hi((ImageSize){ 1024, 1024 }, *grid, TEXTOOL_OCTAVES); break;
        case 'l': return noiseExtImgOp_octave((ImageSize){ 1024, 1024 }, *grid, TEXTOOL_OCTAVES, asm_rgb); break;
        //case 'j': return noiseExtImgOp((ImageSize){ 1024, 1024 }, *grid, levelsNoiseMod); break;
        //case 'k': return noiseExtImgOp((ImageSize){ 1024, 1024 }, *grid, cosMod); break;
        //case 'l': return noiseExtImgOp((ImageSize){ 1024, 1024 }, *grid, tanMod); break;
        default: return noiseImgOp((ImageSize){ 1024, 1024 }, *grid); break;
    }
}