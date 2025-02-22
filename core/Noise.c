#include "Rasteron.h"

#define OCTAVE_MAX 8
#define OCTAVE_BLEND 0.5

// Noise Image Operations

Rasteron_Image* noiseImgOp_white(ImageSize size, uint32_t color1, uint32_t color2){
    Rasteron_Image* noiseImg = RASTERON_ALLOC("white_noise", size.height, size.width);

    // Noise Generation Logic
    double noiseVal;
    for (unsigned p = 0; p < noiseImg->width * noiseImg->height; p++){
        noiseVal = (double)rand() / (double)RAND_MAX;
		*(noiseImg->data + p) = fuse_colors(color1, color2, noiseVal);
    }

    return noiseImg;
}

Rasteron_Image* noiseExtImgOp(ImageSize size, ColorGrid grid, noiseCallback callback){
	assert(grid.xCells > 0 && grid.yCells > 0);

    Rasteron_Image* noiseImg = RASTERON_ALLOC("gradient_noise", size.height, size.width);

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
			topLeft++; topRight++; botLeft++; botRight++;
		}

		// color determination
		double xFrac = callback((double)(xOffset % xSwitch) / (double)xSwitch); // relative X offset inside grid cell
		double yFrac = callback((double)(yOffset % ySwitch) / (double)ySwitch); // relative Y offset inside grid cell

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

static float valueNoiseMod(float value){ return value; }
static float tiledNoiseMod(float value){ return value / value; }
static float crossedNoiseMod(float value){ return (value > 0.5)? value - 0.25 : value + 0.25; }
static float steppedNoiseMod(float value){ return (value * 2.0) - floor(value * 2.0); }
// static float tiledNoiseMod(float value){ return (value > 0.5)? pow(value, 1.0 + ((value - 0.5) * 2.0)) : pow(value, 1.0 - ((0.5 - value) * 2.0)); }
// static float tiledNoiseMod(float value){ return pow(value - 0.5, 0.5); }
// static float crossedNoiseMod(float value){ return (sin(value * 10) > 0.0)? sin(value * 10) : cos(value * 10); }
// static float expNoiseMod(float value){ if(value < 0.15) return 0.15; else if(value > 0.85) return 0.85; else return 0.5; }

Rasteron_Image* noiseImgOp(ImageSize size, ColorGrid grid){
	return noiseExtImgOp(size, grid, valueNoiseMod);
}

Rasteron_Image* noiseImgOp_tiled(ImageSize size, ColorGrid grid){
	return noiseExtImgOp(size, grid, tiledNoiseMod);
}

Rasteron_Image* noiseImgOp_crossed(ImageSize size, ColorGrid grid){
	return noiseExtImgOp(size, grid, crossedNoiseMod);
}

Rasteron_Image* noiseImgOp_stepped(ImageSize size, ColorGrid grid){
	return noiseExtImgOp(size, grid, steppedNoiseMod);
}

Rasteron_Image* noiseExtImgOp_octave(ImageSize size, ColorGrid grid, unsigned short octaves, mixCallback callback){
	assert(grid.xCells > 0 && grid.yCells > 0);
	if(octaves > OCTAVE_MAX) octaves = OCTAVE_MAX;

	Rasteron_Image* noiseImg = noiseImgOp(size, grid);

	if(octaves > 1)
		for(unsigned o = 0; o < octaves; o++){
			Rasteron_Image* octaveImg = resizeImgOp((ImageSize){ noiseImg->height / pow(2, o + 1), noiseImg->width / pow(2, o + 1)}, noiseImg);
			for(unsigned p = 0; p < noiseImg->width * noiseImg->width; p++){
				unsigned xOffset = (p % noiseImg->width) % octaveImg->width;
				unsigned yOffset = (p / noiseImg->width) % octaveImg->height;
				*(noiseImg->data + p) = callback(
					*(noiseImg->data + p), 
					*(octaveImg->data + (yOffset * octaveImg->width) + xOffset)
				);
			}
			RASTERON_DEALLOC(octaveImg);
		}

	return noiseImg;
}

static unsigned blendOctaves(unsigned color1, unsigned color2){ return blend_colors_eq(color1, color2); } // 0.5); }
static unsigned addOctaves(unsigned color1, unsigned color2){ return add_colors(color1, color2); }
static unsigned subOctaves(unsigned color1, unsigned color2){ return diff_colors(color1, color2); }
static unsigned multOctaves(unsigned color1, unsigned color2){ return mult_rgb(color1, color2); }
static unsigned fuseOctaves_low(unsigned color1, unsigned color2){ return fuse_colors(color1, color2, 0.0); }
static unsigned fuseOctaves_hi(unsigned color1, unsigned color2){ return fuse_colors(color1, color2, 1.0); }
static unsigned fuseOctaves_rand(unsigned color1, unsigned color2){ return fuse_colors(color1, color2, (float)rand() / (float)RAND_MAX); }

Rasteron_Image* noiseImgOp_octave(ImageSize size, ColorGrid grid, unsigned short octaves){
	return noiseExtImgOp_octave(size, grid, octaves, blendOctaves);
}

Rasteron_Image* noiseImgOp_add(ImageSize size, ColorGrid grid, unsigned short octaves){
	return noiseExtImgOp_octave(size, grid, octaves, addOctaves);
}

Rasteron_Image* noiseImgOp_diff(ImageSize size, ColorGrid grid, unsigned short octaves){
	return noiseExtImgOp_octave(size, grid, octaves, subOctaves);
}

Rasteron_Image* noiseImgOp_mult(ImageSize size, ColorGrid grid, unsigned short octaves){
	return noiseExtImgOp_octave(size, grid, octaves, multOctaves);
}

Rasteron_Image* noiseImgOp_low(ImageSize size, ColorGrid grid, unsigned short octaves){
	return noiseExtImgOp_octave(size, grid, octaves, fuseOctaves_low);
}

Rasteron_Image* noiseImgOp_hi(ImageSize size, ColorGrid grid, unsigned short octaves){
	return noiseExtImgOp_octave(size, grid, octaves, fuseOctaves_hi);
}

Rasteron_Image* noiseImgOp_rand(ImageSize size, ColorGrid grid, unsigned short octaves){
	return noiseExtImgOp_octave(size, grid, octaves, fuseOctaves_rand);
}