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

static unsigned invertMix(unsigned color1, unsigned color2){ return (0xFFFFFFFF - ((color1 > color2)? color_invert(color1 - color2) : color_invert(color2 - color1))) | 0xFF000000; }

static unsigned wavyMix(unsigned color1, unsigned color2){ return fuse_colors(color1 + color2, color1 - color2, 0.5); }

static unsigned powerMix(unsigned color1, unsigned color2) { return (unsigned)pow((double)color1, (double)((color2 % 8) + 1)); }

static uint32_t asm_rgb(uint32_t color1, uint32_t color2){ 
	uint8_t red = ((color1 & RED_CHANNEL) >> 16) + ((color2 & RED_CHANNEL) >> 16);
	uint8_t green = ((color1 & GREEN_CHANNEL) >> 8) - ((color2 & GREEN_CHANNEL) >> 8);
	uint8_t blue = (color1 & BLUE_CHANNEL) * (color2 & BLUE_CHANNEL);

	uint32_t result = ((0xFF << 24) | (red << 16) | (green << 8) | blue);
    return result;
}

Rasteron_Image* purturbNoiseImgOp(ImageSize size, ColorGrid grid, noiseCallback callback, double xOff, double yOff){
    if(xOff == 0.0 && yOff == 0.0) return noiseExtImgOp(size, grid, callback);

    Rasteron_Image* noiseImg = RASTERON_ALLOC("purturb_noise", size.height, size.width);

	// generating grid cells
    const unsigned xCellPoints = grid.xCells + 1; // includes leftmost and rightmost vertices +1
    const unsigned yCellPoints = grid.yCells + 1; // includes topmost and bottommost vertices +1

	Rasteron_Image* gridImg = RASTERON_ALLOC("grid", yCellPoints, xCellPoints);
	for (unsigned p = 0; p < gridImg->width * gridImg->height; p++) {
		double noiseVal = (double)rand() / (double)RAND_MAX; // random value between 0 and 1
		*(gridImg->data + p) = blend_colors(grid.color1, grid.color2, noiseVal); // blending value between grid colors
	}

	// grid cell values
	unsigned* topLeft; unsigned* topRight; unsigned* botLeft; unsigned* botRight;
	const unsigned xSwitch = noiseImg->width / grid.xCells;
	const unsigned ySwitch = noiseImg->height / grid.yCells;

    int xShift = 0; int yShift = 0;
    for(unsigned p = 0; p < noiseImg->width * noiseImg->height; p++){
        unsigned xOffset = p % noiseImg->width; // absolute X pixel offset
        unsigned yOffset = p / noiseImg->width; // absolute Y pixel offset
		
		// setting grid cell values
		if(xOffset == 0){ // repositions to correct row inside grid
			unsigned short row = (yOffset / ySwitch) * gridImg->width;

			topLeft = gridImg->data + row;
			topRight = gridImg->data + 1 + row;
			botLeft = gridImg->data + gridImg->width + row;
			botRight = gridImg->data + gridImg->width + 1 + row;
		}
		else if (xOffset % xSwitch == 0 && p > 0) { // increment to next column inside grid
            xShift = ((double)rand() / (double)RAND_MAX) * xOff * ((rand() % 2 == 0)? size.width : -size.width);
            yShift = ((double)rand() / (double)RAND_MAX) * yOff * ((rand() % 2 == 0)? size.height : -size.height);
			topLeft++; topRight++; botLeft++; botRight++;
		}

		// color determination
		double xFrac = callback((double)(xOffset % xSwitch) / (double)xSwitch); // relative X offset inside grid cell
        xFrac += (((double)rand() / (double)RAND_MAX) - 0.5F) * xOff * 2.0; // random purturb X offset
		double yFrac = callback((double)(yOffset % ySwitch) / (double)ySwitch); // relative Y offset inside grid cell
        yFrac += (((double)rand() / (double)RAND_MAX) - 0.5F) * yOff * 2.0; // random purturb Y offset

		unsigned newColor = blend_colors(
			blend_colors(*topLeft, *topRight, xFrac),
			blend_colors(*botLeft, *botRight, xFrac),
			yFrac
		);
        
        *(noiseImg->data + p) = newColor;
    }

	RASTERON_DEALLOC(gridImg);
    return noiseImg;
}