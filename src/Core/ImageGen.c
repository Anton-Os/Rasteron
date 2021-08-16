#include "Image.h"

// Helper Types Operations

void addSeed(Rasteron_SeedTable* table, unsigned color){
	addWeightedSeed(table, color, 0.0);
}

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

// Image Operations

Rasteron_Image* allocNewImg(const char* name, uint32_t width, uint32_t height){
	Rasteron_Image* image = (Rasteron_Image*)malloc(sizeof(Rasteron_Image));
	image->name = name;
	image->width = width;
	image->height = height;
	image->data = (uint32_t*)malloc(width * height * sizeof(uint32_t));

	return image;
}

Rasteron_Image* createImgBlank(uint32_t width, uint32_t height, uint32_t solidColor){
	Rasteron_Image* blankImage = allocNewImg("blank", width, height);

	makeRasterColor(blankImage->data, blankImage->width * blankImage->height, solidColor);

	return blankImage;
}

Rasteron_Image* createImgBase(const FileImage* refImage){
	if (refImage == NULL) {
		puts("Cannot create base image! Null pointer provided as reference image!");
		return NULL;
	}

    Rasteron_Image* baseImage;
    switch(refImage->fileFormat){
#ifdef USE_IMG_TIFF
	case(IMG_Tiff):
		baseImage = allocNewImg("base", refImage->imageData.tiff.width, refImage->imageData.tiff.length);
		switchRasterRB(refImage->imageData.tiff.raster, refImage->imageData.tiff.width * refImage->imageData.tiff.length);
		for(unsigned i = 0; i < baseImage->width * baseImage->height; i++)
		   *(baseImage->data + i) = *(refImage->imageData.tiff.raster + i);
		break;
#endif
#ifdef USE_IMG_BMP
	case(IMG_Bmp):
		baseImage = allocNewImg("base", abs(refImage->imageData.bmp.width), abs(refImage->imageData.bmp.height));
		for (unsigned i = 0; i < baseImage->width * baseImage->height; i++)
			*(baseImage->data + i) = *(refImage->imageData.bmp.data + i);
		break;
#endif
#ifdef USE_IMG_PNG
	case(IMG_Png):
		baseImage = allocNewImg("base", refImage->imageData.png.width, refImage->imageData.png.height);
		for (unsigned i = 0; i < baseImage->width * baseImage->height; i++)
			*(baseImage->data + i) = *(refImage->imageData.png.rgbaData + i);
		break;
#endif
	default:
		puts("Image Format not yet supported!!!");
		break;
	}

	return baseImage;
}

Rasteron_Image* createImgGrey(const Rasteron_Image* refImage) {
	if (refImage == NULL) {
		puts("Cannot create grey image! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Image* greyImage = allocNewImg("grey", refImage->width, refImage->height);
	
	// Generation Logic
	uint32_t grey;
	for (unsigned p = 0; p < greyImage->width * greyImage->height; p++) {
		grey = grayify32(*(refImage->data + p));
		*(greyImage->data + p) = grey;
	}

	return greyImage;
}

Rasteron_Image* createImgFilter(const Rasteron_Image* refImage, CHANNEL_Type channel) {
	if (refImage == NULL) {
		puts("Cannot create filter image! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Image* filterImage;
	uint32_t colorMask; // used for isolating a specific color value
	switch(channel){
		case CHANNEL_Red:
			filterImage = allocNewImg("red", refImage->width, refImage->height);
			colorMask = RED_BITS_MASK;
			break;
		case CHANNEL_Green:
			filterImage = allocNewImg("green", refImage->width, refImage->height);
			colorMask = GREEN_BITS_MASK;
			break;
		case CHANNEL_Blue:
			filterImage = allocNewImg("blue", refImage->width, refImage->height);
			colorMask = BLUE_BITS_MASK;
			break;
	}
	
	// Generation Logic
	uint32_t color;
	for (unsigned p = 0; p < filterImage->width * filterImage->height; p++) {
		color = (*(refImage->data + p) & ALPHA_BITS_MASK) + (*(refImage->data + p) & colorMask) ;
		*(filterImage->data + p) = color;
	}

	return filterImage;
}

Rasteron_Image* createImgScatter(const Rasteron_Image* refImage, uint32_t color, double prob){
	if (refImage == NULL) {
		puts("Cannot create scatter image! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Image* scatterImage = allocNewImg("scatter", refImage->width, refImage->height);
	// *scatterImage->data = *refImage->data; // copies original image
	if(prob >= 1.0){
		makeRasterColor(scatterImage->data, scatterImage->width * scatterImage->height, color);
		return scatterImage;
	} else if(prob <= 0.0) return scatterImage; // no modifications

	double chance = 0.0;
	for (unsigned p = 0; p < scatterImage->width * scatterImage->height; p++) {
		chance = (double)rand() / (double)RAND_MAX;
		if (chance <= prob) *(scatterImage->data + p) = color; // if chance is less than probability change to target color
		else *(scatterImage->data + p) = *(refImage->data + p); // otherwise copy ref contents
	}

	return scatterImage;
}

Rasteron_Image* createImgSplash(const Rasteron_Image* refImage, const Rasteron_SeedTable* seedTable){
	if (refImage == NULL) {
		puts("Cannot create splash image! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Image* splashImage = allocNewImg("splash", refImage->width, refImage->height);
	// *splashImage->data = *refImage->data; // copies original image
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
	for (unsigned p = 0; p < splashImage->width * splashImage->height; p++) {
		chance = (rand() /  RAND_MAX) * seedWeightTotal;
		unsigned color = *(refImage->data + p);
		for(unsigned s = 0; s < seedTable->seedCount; s++)
			if(chance > seedRanges[s][0] && chance < seedRanges[s][1]){ // checks if rand number is within bounds
				color = seedTable->seeds[s].color; // sets new color if range is satisfied
				break;
			}
		(*(splashImage->data + p) = color);
	}

	return splashImage;
}

Rasteron_Image* createImgProxCell(const Rasteron_Image* refImage, const Rasteron_ColorPointTable* colorPointTable){
	if (refImage == NULL) {
		puts("Cannot create splash image! Null pointer provided as reference image!");
		return NULL;
	}

	unsigned* targetPixels = malloc(colorPointTable->pixelPointCount * sizeof(unsigned));

	for(unsigned t = 0; t < colorPointTable->pixelPointCount; t++)
		*(targetPixels + t) = getPixIndexFromPos(&colorPointTable->positions[t].pos, refImage);

	Rasteron_Image* proxCellImage = allocNewImg("prox-cell", refImage->width, refImage->height);

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
}

// Additional Types operations

// Color Seed operations

void addColorToSeeds(Rasteron_SeedTable* seeds, uint32_t color, double weight){
	unsigned offset = seeds->seedCount;
	seeds->seeds[offset].color = color;
	seeds->seeds[offset].weight = weight;
	seeds->seedCount++;
}

// Pixel Point operations

unsigned getPixIndexFromPos(Rasteron_PixelPoint* pixPos, Rasteron_Image* refImage){
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