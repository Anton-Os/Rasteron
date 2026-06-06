#include "TexTool.h"

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
