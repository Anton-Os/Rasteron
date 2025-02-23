#include "func_def.h"

void seedRandGen(){ srand(time(NULL)); }

// --------------------------------   One Color Functions  -------------------------------- //

uint32_t color_unique(){
	static unsigned invoke = 0; // increases each invocation
	unsigned color = 0xFFFFFFFF;

	for(unsigned i = 0; i < invoke; i++){
		if(i % 0xFF == 0xFE){
			color |= 0x000000FF; // &= 0xFFFFFF00; // reset blue bit
			color -= 0x100; // += 0x100; // increment green bit
		}
		if(i % 0xFFFF == 0xFFFE){
			color |= 0x0000FF00; // &= 0xFFFF00FF; // reset green bit
			color -= 0x10000; // += 0x10000; // increment red bit
		}
		color -= 0x1; // += 0x1; // increment blue bit
	}

	invoke++;
	return color;
}

uint32_t color_level(uint32_t color, double level){
	if(level >= 1.0) return 0xFFFFFFFF;
	else if(level <= 0.0) return 0xFF000000;
	else if(level == 0.5) return color;
	else if(level > 0.5) return blend_colors(color, 0xFFFFFFFF, (level - 0.5) * 2.0);
	else if(level < 0.5) return blend_colors(color, 0xFF000000, (0.5 - level) * 2.0);
	else return color;
}

uint32_t color_invert(uint32_t refColor){
	uint8_t alpha = (refColor & ALPHA_CHANNEL) >> 24;
	uint8_t red = 0xFF - ((refColor & RED_CHANNEL) >> 16);
	uint8_t green = 0xFF - ((refColor & GREEN_CHANNEL) >> 8);
	uint8_t blue = 0xFF - (refColor & BLUE_CHANNEL);

	uint32_t result = ((alpha << 24) | (red << 16) | (green << 8) | blue);
	return result;
}

uint32_t color_frac(uint32_t refColor, double frac){
    uint8_t alpha = channel_frac((refColor & ALPHA_CHANNEL) >> 24, frac);
    uint8_t red = channel_frac((refColor & RED_CHANNEL) >> 16, frac);
    uint8_t green = channel_frac((refColor & GREEN_CHANNEL) >> 8, frac);
    uint8_t blue = channel_frac(refColor & BLUE_CHANNEL, frac);

    uint32_t result = ((alpha << 24) | (red << 16) | (green << 8) | blue);
    return result;
}

// --------------------------------   Channel Functions   -------------------------------- //

uint32_t swap_rb(uint32_t color){ return ((color & 0xFF) << 16) + (color & 0xFF00) + ((color >> 16) & 0xFF); }

uint32_t swap_gb(uint32_t color){ return ((color & 0xFF0000) + ((color & 0xFF) << 8) + ((color >> 8) & 0xFF)); }

uint32_t swap_rg(uint32_t color){ return ((color & 0xFF00) << 8) + ((color >> 8) & 0xFF00) + (color & 0xFF); }

uint8_t lo_channel(uint32_t color1, uint32_t color2, CHANNEL_Type channel){
	uint32_t mask, shift;
	switch(channel){
		case CHANNEL_Alpha: mask = ALPHA_CHANNEL; shift = 24; break;
		case CHANNEL_Red: mask = RED_CHANNEL; shift = 16; break;
		case CHANNEL_Green: mask = GREEN_CHANNEL; shift = 8; break;
		case CHANNEL_Blue: mask = BLUE_CHANNEL; shift = 0; break;
	}

	return ((color1 & mask) < (color2 & mask)) ? ((color1 & mask) >> shift) : ((color2 & mask) >> shift);
}

uint8_t hi_channel(uint32_t color1, uint32_t color2, CHANNEL_Type channel){
	uint32_t mask, shift;
	switch(channel){
		case CHANNEL_Alpha: mask = ALPHA_CHANNEL; shift = 24; break;
		case CHANNEL_Red: mask = RED_CHANNEL; shift = 16; break;
		case CHANNEL_Green: mask = GREEN_CHANNEL; shift = 8; break;
		case CHANNEL_Blue: mask = BLUE_CHANNEL; shift = 0; break;
	}

	return ((color1 & mask) > (color2 & mask)) ? ((color1 & mask) >> shift) : ((color2 & mask) >> shift);
}

/* int8_t getLightDiff(uint32_t color1, uint32_t color2){ 
	return (int8_t)((int)channel_gray(color1) - (int)channel_gray(color2)); 
} */

uint8_t diff_channel(uint32_t color1, uint32_t color2, CHANNEL_Type channel){
	uint32_t mask;
	switch(channel){
		case CHANNEL_Alpha: mask = ALPHA_CHANNEL; break;
		case CHANNEL_Red: mask = RED_CHANNEL; break;
		case CHANNEL_Green: mask = GREEN_CHANNEL; break;
		case CHANNEL_Blue: mask = BLUE_CHANNEL; break;
	}
	return (uint8_t)((int)(color1 & mask) - (int)(color2 & mask));
}

uint32_t color_gray(uint32_t refColor) {
    if (refColor == WHITE_COLOR) return WHITE_COLOR;
    if (refColor == 0 || refColor == BLACK_COLOR) return BLACK_COLOR;

    uint8_t alpha = 0xFF; // Complete opacity desired for alpha
    uint8_t avgColor = (((refColor & RED_CHANNEL) >> 16) + ((refColor & 0xFF00) >> 8) + (refColor & 0xFF)) / 3;

    uint32_t result = ((alpha << 24) | (avgColor << 16) | (avgColor << 8) | avgColor);
    return result;
}

uint8_t channel_gray(uint32_t refColor){
    if (refColor == WHITE_COLOR) return WHITE_COLOR;
    if (refColor == 0 || refColor == BLACK_COLOR) return BLACK_COLOR;

    uint8_t avgColor = (((refColor & RED_CHANNEL) >> 16) + ((refColor & 0xFF00) >> 8) + (refColor & 0xFF)) / 3;
    return avgColor;
}

uint8_t channel_frac(uint8_t refColor, double frac){
    if(frac >= 1.0) return refColor;
    else if(frac <= 0.0) return 0x00;
    else return ((double)refColor / 255.0) * frac * 255.0;
}

// --------------------------------   Multi Color Functions   -------------------------------- //

uint32_t blend_colors(uint32_t color1, uint32_t color2, double bVal){
	if(bVal <= 0.0) return color1;
	else if(bVal >= 1.0) return color2;
	
	uint32_t bColor1 = color_frac(color1, 1.0 -bVal);
	uint32_t bColor2 = color_frac(color2, bVal);

	return bColor1 + bColor2; 
}

uint32_t blend_colors_eq(uint32_t color1, uint32_t color2){ return blend_colors(color1, color2, 0.5); }

uint32_t fuse_colors(uint32_t color1, uint32_t color2, double iVal){
	uint8_t loRedBit = lo_channel(color1, color2, CHANNEL_Red); uint8_t hiRedBit = hi_channel(color1, color2, CHANNEL_Red);
    uint8_t loGreenBit = lo_channel(color1, color2, CHANNEL_Green); uint8_t hiGreenBit = hi_channel(color1, color2, CHANNEL_Green);
    uint8_t loBlueBit = lo_channel(color1, color2, CHANNEL_Blue); uint8_t hiBlueBit = hi_channel(color1, color2, CHANNEL_Blue);

	uint32_t loColor = (0 << 24) + (loRedBit << 16) + (loGreenBit << 8) + loBlueBit;
	uint32_t hiColor = (0 << 24) + (hiRedBit << 16) + (hiGreenBit << 8) + hiBlueBit;

	if(iVal <= 0.0) return loColor;
	else if(iVal >= 1.0) return hiColor;
	else {
		uint32_t diffColor = (hiColor - loColor);
		uint8_t finalRedBit = channel_frac((diffColor & RED_CHANNEL) >> 16, iVal);
		uint8_t finalGreenBit = channel_frac((diffColor & GREEN_CHANNEL) >> 8, iVal);
		uint8_t finalBlueBit = channel_frac(diffColor & BLUE_CHANNEL, iVal);

		return loColor + (uint32_t)((0xFF << 24) + (finalRedBit << 16) + (finalGreenBit << 8) + finalBlueBit);
	}
}

uint32_t fuse_colors_eq(uint32_t color1, uint32_t color2){ return fuse_colors(color1, color2, 0.5); }

uint32_t add_colors(uint32_t color1, uint32_t color2){ return (0xFF000000 | (color1 + color2)); }

uint32_t add_rgb(uint32_t color1, uint32_t color2){ 
	uint8_t red = ((color1 & RED_CHANNEL) >> 16) + ((color2 & RED_CHANNEL) >> 16);
	uint8_t green = ((color1 & GREEN_CHANNEL) >> 8) + ((color2 & GREEN_CHANNEL) >> 8);
	uint8_t blue = (color1 & BLUE_CHANNEL) + (color2 & BLUE_CHANNEL);

	uint32_t result = ((0xFF << 24) | (red << 16) | (green << 8) | blue);
    return result;
}

uint32_t diff_colors(uint32_t color1, uint32_t color2){
	if(color1 & 0x00FFFFFF > color2 & 0x00FFFFFF) return (color1 - color2) | (0xFF000000 & color1);
	else return (color2 - color1) | (0xFF000000 & color2);
}

uint32_t diff_rgb(uint32_t color1, uint32_t color2){ 
	uint8_t red = ((color1 & RED_CHANNEL) >> 16) - ((color2 & RED_CHANNEL) >> 16);
	uint8_t green = ((color1 & GREEN_CHANNEL) >> 8) - ((color2 & GREEN_CHANNEL) >> 8);
	uint8_t blue = (color1 & BLUE_CHANNEL) - (color2 & BLUE_CHANNEL);

	uint32_t result = ((0xFF << 24) | (red << 16) | (green << 8) | blue);
    return result;
}

uint32_t mult_colors(uint32_t color1, uint32_t color2){ return color1 * color2; }

uint32_t mult_rgb(uint32_t color1, uint32_t color2){ 
	uint8_t red = ((color1 & RED_CHANNEL) >> 16) * ((color2 & RED_CHANNEL) >> 16);
	uint8_t green = ((color1 & GREEN_CHANNEL) >> 8) * ((color2 & GREEN_CHANNEL) >> 8);
	uint8_t blue = (color1 & BLUE_CHANNEL) * (color2 & BLUE_CHANNEL);

	uint32_t result = ((0xFF << 24) | (red << 16) | (green << 8) | blue);
    return result;
}

uint32_t root_colors(uint32_t color1, uint32_t color2){ // enum CHANNEL_Type type){
	unsigned product = (0xFFFFFF & color1) * (0xFFFFFF & color2); // power
	unsigned color = ((unsigned)(pow((double)product, 0.5)) * 1) | 0xFF000000; // root

	// return color;
	return color * 0xFF; // test
}

uint32_t bit_colors_and(unsigned color1, unsigned color2){ return (color1 & color2) | 0xFF000000; }

uint32_t bit_colors_or(unsigned color1, unsigned color2){ return (color1 | color2) | 0xFF000000; }

uint32_t bit_colors_xor(unsigned color1, unsigned color2){ return (color1 ^ color2) | 0xFF000000; }

// --------------------------------   Pixel Point Functions   -------------------------------- //

double pix_dist(unsigned p1, unsigned p2, unsigned imageWidth){
	long int x1 = p1 % imageWidth;
	long int y1 = p1 / imageWidth;
	long int x2 = p2 % imageWidth;
	long int y2 = p2 / imageWidth;

	double w = fabs(x2 - x1); // length in pixels
	double l = fabs(y2 - y1); // width in pixels

	return (double)sqrt((l * l) + (w * w));
}

unsigned pixPoint_offset(PixelPoint pixPoint, ref_image_t refImage){
	unsigned xOffset; // clamping X
	if(pixPoint.x <= 0.0) xOffset = 0;
	else if(pixPoint.x >= 1.0) xOffset = refImage->width - 1;
	else xOffset = (unsigned)((double)refImage->width * pixPoint.x);

	unsigned yOffset; // clamping Y
	if(pixPoint.y <= 0.0) yOffset = 0;
	else if(pixPoint.y >= 1.0) yOffset = refImage->height - 1;
	else yOffset = (unsigned)((double)refImage->height * pixPoint.y);

	unsigned pixIndex = (yOffset * refImage->width) + xOffset;
	return pixIndex;
}

unsigned pixPoint_color(PixelPoint pixPos, ref_image_t refImage){
	return *(refImage->data + pixPoint_offset(pixPos, refImage));
}

unsigned pixPoint_cursorOffset(PixelPoint pixPoint, ref_image_t refImage){
	double x; // clamping X
	if(pixPoint.x <= -1.0) x = -1.0;
	else if(pixPoint.x >= 1.0) x = 1.0;
	else x = pixPoint.x;

	double y; // clamping Y
	if(pixPoint.y <= -1.0) y = -1.0;
	else if(pixPoint.y >= 1.0) y = 1.0;
	else y = pixPoint.y;
	y *= -1.0; // Y value needs to be flipped

	PixelPoint adjPoint = (PixelPoint){ (x / 2) + 0.5, (y / 2) + 0.5 };
	return pixPoint_offset(adjPoint, refImage);
}

unsigned pixPoint_cursorColor(PixelPoint cursorPos, ref_image_t refImage){
	return *(refImage->data + pixPoint_cursorOffset(cursorPos, refImage));
}

void pixPoionts_expand(PixelPointTable* table, unsigned short divs){
	if(table->pointCount < 2 || divs == 0) return; // no points

	PixelPointTable newTable;
	newTable.pointCount = 0;// table->pointCount + ((table->pointCount - 1) * divs);

	for(unsigned p = 0; p < table->pointCount - 1 && p < MAX_PIXELPOINTS; p++){
		newTable.points[newTable.pointCount] = table->points[p];
		newTable.pointCount++;

		PixelPoint pixDiff = { table->points[p].x - table->points[p + 2].x, table->points[p].y - table->points[p + 1].y }; // difference between next and current point
		PixelPoint pixInc = { pixDiff.x / (double)(divs + 1), pixDiff.y / (double)(divs + 1) }; // increment step between points
		for(unsigned short d = 0; d < divs; d++){
			newTable.points[newTable.pointCount] = (PixelPoint){ table->points[p].x + ((d + 1) * pixInc.x), table->points[p].y + ((d + 1) * pixInc.y) };
			newTable.pointCount++;
		}
	}

	table->pointCount = newTable.pointCount;
	for(unsigned p = 0; p < newTable.pointCount; p++) table->points[p] = newTable.points[p]; // copyiing new data over
}

void pixPoints_tiling(PixelPointTable* table, enum TILE_Type type, unsigned short height, unsigned short width){
	table->pointCount = 0;

	for(unsigned r = 0; r < width + 1; r++)
        for(unsigned c = 0; c < height + 1; c++)
            switch(type){
                case 0: // regular grid tiling
                    pixelPointToTable(table, (1.0 / height) * c, (1.0 / width) * r);
                    break;
                case 1: // triangular tiling attempt
                    if((r % 2 == 0 && c % 2 == 0) || (r % 2 == 1 && c % 2 == 1))
                        pixelPointToTable(table, (1.0 / height) * c, (1.0 / width) * r);
                    break;
                case 2: // hexagonal tiling attempt
                    if((r % 2 == 0 && c % 3 != 0) || (r % 2 == 1 && c % 3 == 0))
                        pixelPointToTable(table, (1.0 / height) * c, (1.0 / width) * r);
                    break;
                case 3: // diagonal tiling attempt
                    if(c == r || c == height + 1 - r || r == width + 1 - c) // TODO: Fix this
                        pixelPointToTable(table, (1.0 / height) * c, (1.0 / width) * r);
                    break;
                case 4: // perimiter tiling attempt
                    if(c == 0 || c == height + 1 || r == 0 || r == width + 1) // TODO: Fix this
                        pixelPointToTable(table, (1.0 / height) * c, (1.0 / width) * r);
                    break;
                default: pixelPointToTable(table, (double)rand() / (double)RAND_MAX, (double)rand() / (double)RAND_MAX); // random
            }

}

// --------------------------------   Cellular Functions   -------------------------------- //

static void setFlagBit(nebrFlags* target, enum NEBR_CellFlag flagBit){ *target = (*target | (1 << (flagBit))); }

static void clearFlagBit(nebrFlags* target, enum NEBR_CellFlag flagBit){ *target = (*target & (~(1 << (flagBit)))); }

nebrFlags neighbor_exists(uint32_t index, uint32_t width, uint32_t height){
    nebrFlags flags = 0xFF;

    if(index < width){ // clear top flags
        clearFlagBit(&flags, NEBR_Top_Left); clearFlagBit(&flags, NEBR_Top); clearFlagBit(&flags, NEBR_Top_Right);
    } else if((width * height) - index - 1 < width){ // clear bottom flags
        clearFlagBit(&flags, NEBR_Bot_Left); clearFlagBit(&flags, NEBR_Bot); clearFlagBit(&flags, NEBR_Bot_Right);
    }

	if(index % width == 0){ // clear left flags
        clearFlagBit(&flags, NEBR_Top_Left); clearFlagBit(&flags, NEBR_Left); clearFlagBit(&flags, NEBR_Bot_Left);
    } else if(index % width == width - 1){ // clear right flags
        clearFlagBit(&flags, NEBR_Top_Right); clearFlagBit(&flags, NEBR_Right); clearFlagBit(&flags, NEBR_Bot_Right);
    }

    return flags;
}

unsigned neighbor_getOffset(unsigned width, unsigned offset, enum NEBR_CellFlag whichNebr){
	switch (whichNebr) {
		case NEBR_Bot_Right: return offset + width + 1;
		case NEBR_Bot: return offset + width;
		case NEBR_Bot_Left: return offset + width - 1;
		case NEBR_Right: return offset + 1;
		case NEBR_Left: return offset - 1;
		case NEBR_Top_Right: offset - width + 1;
		case NEBR_Top: return offset - width;
		case NEBR_Top_Left: return offset - width - 1;
		default: return offset;
	}
}

uint32_t* neighbor_get(Rasteron_Image* refImage, uint32_t index, enum NEBR_CellFlag whichNebr){
	const uint32_t* target = refImage->data + neighbor_getOffset(refImage->width, index, whichNebr);
	return target;
}

unsigned short neighbor_count(unsigned color, unsigned neighbors[8]){
	unsigned short count = 0;
    for(unsigned n = 0; n < 8; n++) if(neighbors[n] == color) count++;
    return count;
}

void neighbors_load(const NebrTable* nebrTable, unsigned* br, unsigned* b, unsigned* bl, unsigned* r, unsigned* l, unsigned* tr, unsigned* t, unsigned* tl){
	unsigned short i = 0; // index to keep track of neighbor

	br = (nebrTable->flags & (1 << NEBR_Bot_Right))? *(nebrTable->nebrs + (++i - 1)) : NO_COLOR;
	b = (nebrTable->flags & (1 << NEBR_Bot))? *(nebrTable->nebrs + (++i - 1)) : NO_COLOR;
	bl = (nebrTable->flags & (1 << NEBR_Bot_Left))? *(nebrTable->nebrs + (++i - 1)) : NO_COLOR;
	r = (nebrTable->flags & (1 << NEBR_Right))? *(nebrTable->nebrs + (++i - 1)) : NO_COLOR;
	l = (nebrTable->flags & (1 << NEBR_Left))? *(nebrTable->nebrs + (++i - 1)) : NO_COLOR;
	tr = (nebrTable->flags & (1 << NEBR_Top_Right))? *(nebrTable->nebrs + (++i - 1)) : NO_COLOR;
	t = (nebrTable->flags & (1 << NEBR_Top))? *(nebrTable->nebrs + (++i - 1)) : NO_COLOR;
	tl = (nebrTable->flags & (1 << NEBR_Top_Left))? *(nebrTable->nebrs + (++i - 1)) : NO_COLOR;
}