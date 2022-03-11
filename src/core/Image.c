#include "Image.h"

// Helper Types Operations

/* static unsigned calcPixelPointCoord(double input, double xMin, double xMax){
	if(pixPos->xFrac <= xMin) xOffset = 0;
	else if(pixPos->xFrac >= 1.0) xOffset = refImage->width - 1;
	else xOffset = (unsigned)((double)refImage->width * pixPos->xFrac);
} */

void addSeed(Rasteron_SeedTable* table, unsigned color){ addWeightedSeed(table, color, 0.0); }

void addWeightedSeed(Rasteron_SeedTable* table, unsigned color, double weight){
	table->seeds[table->seedCount].color = color;
	table->seeds[table->seedCount].weight = weight;
	table->seedCount++;
}

void addColorPoint(Rasteron_ColorPointTable* table, unsigned color, double xFrac, double yFrac){
	table->positions[table->pixelPointCount].pos.xFrac = xFrac;
	table->positions[table->pixelPointCount].pos.yFrac = yFrac;
	table->positions[table->pixelPointCount].color = color;
	table->pixelPointCount++;
}

unsigned getPixOffset(const Rasteron_PixelPoint* pixPos, Rasteron_Image* refImage){
	unsigned xOffset;
	if(pixPos->xFrac <= 0.0) xOffset = 0;
	else if(pixPos->xFrac >= 1.0) xOffset = refImage->width - 1;
	else xOffset = (unsigned)((double)refImage->width * pixPos->xFrac);

	unsigned yOffset;
	if(pixPos->yFrac <= 0.0) yOffset = 0;
	else if(pixPos->yFrac >= 1.0) yOffset = refImage->height;
	else yOffset = (unsigned)((double)refImage->height * pixPos->yFrac);

	unsigned pixIndex = (yOffset * refImage->width) + xOffset;
	return pixIndex;
}

unsigned getPixCursorOffset(const Rasteron_PixelPoint* pixPos, Rasteron_Image* refImage){
	unsigned xOffset;
	if(pixPos->xFrac <= -1.0) xOffset = 0;
	else if(pixPos->xFrac >= 1.0) xOffset = refImage->width - 1;

	unsigned yOffset;
	if(pixPos->yFrac <= -1.0) yOffset = 0;
	else if(pixPos->yFrac >= 1.0) yOffset = refImage->height;

	// Implement Body Here!
	return 0;
}

// Image Operations

Rasteron_Image* allocNewImg(const char* name, uint32_t height, uint32_t width){
	Rasteron_Image* image = (Rasteron_Image*)malloc(sizeof(Rasteron_Image));
	image->name = name;
	image->width = width;
	image->height = height;
	image->data = (uint32_t*)malloc(width * height * sizeof(uint32_t));

	return image;
}

Rasteron_Image* createImgRef(const char* fileName){
	FileImage fileImage;
	loadFileImage(fileName, &fileImage);

    Rasteron_Image* refImage;
    switch(fileImage.fileFormat){
#ifdef USE_IMG_TIFF
	case(IMG_Tiff):
		refImage = allocNewImg("ref-tiff", fileImage.data.tiff.length, fileImage.data.tiff.width);
		switchRB(fileImage.data.tiff.raster, fileImage.data.tiff.width * fileImage.data.tiff.length);
		for(unsigned i = 0; i < refImage->width * refImage->height; i++)
		   *(refImage->data + i) = *(fileImage.data.tiff.raster + i); // copying operation
		break;
#endif
#ifdef USE_IMG_BMP
	case(IMG_Bmp):
		refImage = allocNewImg("ref-bmp", abs(fileImage.data.bmp.height), abs(fileImage.data.bmp.width));
		for (unsigned i = 0; i < refImage->width * refImage->height; i++)
			*(refImage->data + i) = *(fileImage.data.bmp.data + i); // copying operation
		break;
#endif
#ifdef USE_IMG_PNG
	case(IMG_Png):
		refImage = allocNewImg("ref-png", fileImage.data.png.width, fileImage.data.png.height);
		for (unsigned i = 0; i < refImage->width * refImage->height; i++)
			*(refImage->data + i) = *(fileImage.data.png.rgbaData + i); // copying operation
		break;
#endif
	default:
		perror("Image Format not yet supported!!!");
		return NULL;
	}

	delFileImage(&fileImage);
	return refImage;
}

Rasteron_Image* createImgBlank(uint32_t height, uint32_t width, uint32_t solidColor){
	Rasteron_Image* blankImage = allocNewImg("blank", height, width);
	makeColor(blankImage->data, blankImage->width * blankImage->height, solidColor);
	return blankImage;
}

Rasteron_Image* createImgFlip(const Rasteron_Image* refImage, enum FLIP_Type flip){
	Rasteron_Image* flipImage = NULL;
	if(flip == FLIP_Upside){
		flipImage = allocNewImg("flip", refImage->height, refImage->width); // matches source
		for(unsigned p = 0; p < refImage->height * refImage->width; p++)
			*(flipImage->data + (refImage->height * refImage->width) - p - 1) = *(refImage->data + p); // copies pixels in reverse
	} else {
		flipImage = allocNewImg("flip", refImage->width, refImage->height); // opposite of source
		for(unsigned p = 0; p < refImage->height * refImage->width; p++){
			unsigned matchIndex = (flip == FLIP_Clock)? p : p; // change these to coorespond to flipped image index!
			*(flipImage->data + matchIndex) = *(refImage->data + p);
		}
	}

	return flipImage;
}

Rasteron_Image* createImgGrey(const Rasteron_Image* refImage) {
	if (refImage == NULL) {
		perror("Cannot create grey image! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Image* greyImage = allocNewImg("grey", refImage->height, refImage->width);
	
	// Generation Logic
	uint32_t grey;
	for (unsigned p = 0; p < greyImage->width * greyImage->height; p++) {
		grey = grayify32(*(refImage->data + p));
		*(greyImage->data + p) = grey;
	}

	return greyImage;
}

Rasteron_Image* createImgFltCh(const Rasteron_Image* refImage, CHANNEL_Type channel) {
	if (refImage == NULL) {
		perror("Cannot create filter image! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Image* filterImage;
	uint32_t colorMask; // used for isolating a specific color value
	switch(channel){
		case CHANNEL_Red:
			filterImage = allocNewImg("filter-red", refImage->height, refImage->width);
			colorMask = RED_CHANNEL;
			break;
		case CHANNEL_Green:
			filterImage = allocNewImg("filter-green", refImage->height, refImage->width);
			colorMask = GREEN_CHANNEL;
			break;
		case CHANNEL_Blue:
			filterImage = allocNewImg("filter-blue", refImage->height, refImage->width);
			colorMask = BLUE_CHANNEL;
			break;
	}
	
	// Generation Logic
	uint32_t color;
	for (unsigned p = 0; p < filterImage->width * filterImage->height; p++) {
		color = (*(refImage->data + p) & ALPHA_CHANNEL) + (*(refImage->data + p) & colorMask) ;
		*(filterImage->data + p) = color;
	}

	return filterImage;
}

Rasteron_Image* createImgAvgCh(const Rasteron_Image* refImage, CHANNEL_Type channel) {
	if (refImage == NULL) {
		perror("Cannot create averaged image! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Image* greyImage = createImgGrey(refImage); // greyscale image used as reference
	Rasteron_Image* averageImage = createImgFltCh(greyImage, channel);

	switch(channel){ // renaming
		case CHANNEL_Red: averageImage->name = "average-red"; break;
		case CHANNEL_Green: averageImage->name = "average-green"; break;
		case CHANNEL_Blue: averageImage->name = "average-blue"; break;
	}

	
	deleteImg(greyImage);
	return averageImage;
}

Rasteron_Image* createImgSeedRaw(const Rasteron_Image* refImage, uint32_t color, double prob){
	if (refImage == NULL) {
		perror("Cannot create seeded image! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Image* seededImage = allocNewImg("seeded", refImage->height, refImage->width);
	// *seededImage->data = *refImage->data; // copies original image
	if(prob >= 1.0){
		makeColor(seededImage->data, seededImage->width * seededImage->height, color);
		return seededImage;
	} else if(prob <= 0.0) return seededImage; // no modifications

	double chance = 0.0;
	for (unsigned p = 0; p < seededImage->width * seededImage->height; p++) {
		chance = (double)rand() / (double)RAND_MAX;
		if (chance <= prob) *(seededImage->data + p) = color; // if chance is less than probability change to target color
		else *(seededImage->data + p) = *(refImage->data + p); // otherwise copy ref contents
	}

	return seededImage;
}

Rasteron_Image* createImgSeedWeighted(const Rasteron_Image* refImage, const Rasteron_SeedTable* seedTable){
	if (refImage == NULL) {
		perror("Cannot create palette image! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Image* paletteImage = allocNewImg("palette", refImage->height, refImage->width);
	// *paletteImage->data = *refImage->data; // copies original image
	const float defaultSeedWeight = 1.0f / (seedTable->seedCount + 1); // leaves less covered as seed count increases

	double chance = 0.0f;
	double seedRanges[MAX_COLOR_SEEDS][2];
	double seedWeightTotal = 0.0f;

	// calculating weights
	for(unsigned s = 0; s < seedTable->seedCount; s++){
		double currentWeight = (seedTable->seeds[s].weight == DEFAULT_SEED_WEIGHT) ? defaultSeedWeight : seedTable->seeds[s].weight;
		seedRanges[s][0] = seedWeightTotal; // lower bound is the running weight total
		seedRanges[s][1] = seedWeightTotal + currentWeight; // upper bound is the added weight
		seedWeightTotal += currentWeight;
	}

	// setting proper colors
	for (unsigned p = 0; p < paletteImage->width * paletteImage->height; p++) {
		chance = ((double)rand() / (double)RAND_MAX) * seedWeightTotal;
		unsigned color = *(refImage->data + p);
		for(unsigned s = 0; s < seedTable->seedCount; s++)
			if(chance > seedRanges[s][0] && chance < seedRanges[s][1]){ // checks if rand number is within bounds
				color = seedTable->seeds[s].color; // sets new color if range is satisfied
				break;
			}
		*(paletteImage->data + p) = color;
	}

	return paletteImage;
}

Rasteron_Image* createImgProxim(const Rasteron_Image* refImage, const Rasteron_ColorPointTable* colorPointTable){
	if (refImage == NULL) {
		perror("Cannot create palette image! Null pointer provided as reference image!");
		return NULL;
	}

	unsigned* targetPixels = malloc(colorPointTable->pixelPointCount * sizeof(unsigned));

	for(unsigned t = 0; t < colorPointTable->pixelPointCount; t++)
		*(targetPixels + t) = getPixOffset(&colorPointTable->positions[t].pos, refImage);

	Rasteron_Image* proxCellImage = allocNewImg("prox-cell", refImage->height, refImage->width);

	for (unsigned p = 0; p < proxCellImage->width * proxCellImage->height; p++) {
		unsigned color = 0xFF448844; // some default color!
		double minDist = (double)(proxCellImage->width * proxCellImage->height);

		for(unsigned t = 0; t < colorPointTable->pixelPointCount; t++){
			double dist = getPixDist(p, *(targetPixels + t), proxCellImage->width);
			if(dist < minDist){
				minDist = dist;
				color = colorPointTable->positions[t].color;
			}
			*(proxCellImage->data + p) = color;
		}
	}

	free(targetPixels);
	return proxCellImage;
}


void deleteImg(Rasteron_Image* image) {
    if(image->data != NULL) free(image->data);
    if(image != NULL) free(image);
	image = NULL; // set address to null
}