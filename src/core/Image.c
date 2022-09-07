#include "Image.h"

// Table and Swatch operations

void addSeed(Rasteron_SeedTable* table, unsigned color){ addWeightedSeed(table, color, 0.0); }

void addWeightedSeed(Rasteron_SeedTable* table, unsigned color, double weight){
	table->seeds[table->seedCount].color = color;
	table->seeds[table->seedCount].weight = weight;
	table->seedCount++;
}

Rasteron_Swatch createSwatch(unsigned color, uint8_t deviation){
	Rasteron_Swatch swatch;
	
	swatch.base = color;
	swatch.deviation = deviation;

	uint8_t redBit = (color & RED_CHANNEL) >> 16;
	uint8_t greenBit = (color & GREEN_CHANNEL) >> 8;
	uint8_t blueBit = color & BLUE_CHANNEL;

	// Clamps Additive Values Below 0xFF
	uint8_t redAdd = (deviation < 0xFF - redBit)? deviation : 0xFF - redBit;
	uint8_t greenAdd = (deviation < 0xFF - greenBit)? deviation : 0xFF - greenBit;
	uint8_t blueAdd = (deviation < 0xFF - blueBit)? deviation : 0xFF - blueBit;

	// Clamps Substractive Values Above 0x00
	uint8_t redSub = (deviation < redBit)? deviation : redBit;
	uint8_t greenSub = (deviation < greenBit)? deviation : greenBit;
	uint8_t blueSub = (deviation < greenBit)? deviation : blueBit;

	// TODO: Add calculation here!
	swatch.colors[SWATCH_Yellow_Add] = color + (redAdd << 16) | (greenAdd << 8);
	swatch.colors[SWATCH_Cyan_Add] = color + (greenAdd << 8) | blueAdd;
	swatch.colors[SWATCH_Magenta_Add] = color + (redAdd << 16) | (blueBit + blueAdd);
	swatch.colors[SWATCH_Light] = color + (redAdd << 16) | (greenAdd << 8) | blueAdd;
	swatch.colors[SWATCH_Dark] = color - ((redSub << 16) | (greenSub << 8) | (blueSub));
	swatch.colors[SWATCH_Yellow_Sub] = color - ((redSub << 16) | (greenSub << 8));
	swatch.colors[SWATCH_Cyan_Sub] = color - ((greenSub << 8) | blueSub);
	swatch.colors[SWATCH_Magenta_Sub] = color - ((redSub << 16) | blueSub);

	return swatch;
}

Rasteron_SeedTable createSeedTable(const Rasteron_Swatch* swatch) {
	Rasteron_SeedTable seedTable = { 0 };
	addSeed(&seedTable, swatch->colors[SWATCH_Yellow_Add]);
	addSeed(&seedTable, swatch->colors[SWATCH_Cyan_Add]);
	addSeed(&seedTable, swatch->colors[SWATCH_Magenta_Add]);
	addSeed(&seedTable, swatch->colors[SWATCH_Light]);
	addSeed(&seedTable, swatch->colors[SWATCH_Dark]);
	addSeed(&seedTable, swatch->colors[SWATCH_Yellow_Sub]);
	addSeed(&seedTable, swatch->colors[SWATCH_Cyan_Sub]);
	addSeed(&seedTable, swatch->colors[SWATCH_Magenta_Sub]);

	return seedTable;
}

// Basic Image operations

Rasteron_Image* allocNewImg(const char* name, uint32_t height, uint32_t width){
	Rasteron_Image* image = (Rasteron_Image*)malloc(sizeof(Rasteron_Image));
	
	image->name = name;
	image->width = width;
	image->height = height;
	image->data = (uint32_t*)malloc(width * height * sizeof(uint32_t));

	return image;
}

Rasteron_Image* createRefImg(const char* fileName){
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
	}

	delFileImage(&fileImage);
	return refImage;
}

Rasteron_Image* createSolidImg(ImageSize size, uint32_t color){
	Rasteron_Image* solidImage = allocNewImg("solid", size.height, size.width);
	makeColor(solidImage->data, solidImage->width * solidImage->height, color);
	return solidImage;
}

Rasteron_Image* createCopyImg(Rasteron_Image* refImage){
	if (refImage == NULL) {
		perror("Cannot create copy image! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Image* copyImage = allocNewImg("copy", refImage->height, refImage->width);

	for(unsigned p = 0; p < refImage->width * refImage->height; p++)
		*(copyImage->data + p) = *(refImage->data + p); // copy each pixel

	return copyImage;
}

Rasteron_Image* createFlipImg(const Rasteron_Image* refImage, enum FLIP_Type flip){
	if (refImage == NULL) {
		perror("Cannot create flip image! Null pointer provided as reference image!");
		return NULL;
	}
	
	Rasteron_Image* flipImage = NULL;
	if(flip == FLIP_Upside){
		flipImage = allocNewImg("flip", refImage->height, refImage->width); // parameters match source
		for(unsigned p = 0; p < refImage->height * refImage->width; p++)
			*(flipImage->data + (refImage->height * refImage->width) - p - 1) = *(refImage->data + p); // copies pixels in reverse
	} else {
		flipImage = allocNewImg("flip", refImage->width, refImage->height); // parameters inverse of source
		unsigned offset = 0;

		for(unsigned w = 0; w < refImage->width; w++){
			unsigned match = (flip == FLIP_Clock) 
				? (refImage->width * refImage->height) + w
				: refImage->width - w;
	
			for(unsigned h = 0; h < refImage->height; h++){
				match = (flip == FLIP_Clock) ? match - refImage->width : match + refImage->width;
				*(flipImage->data + offset) = *(refImage->data + match); // copies to image from match index

				offset++; // move to next pixel
			}
		}
	}

	return flipImage;
}

void deleteImg(Rasteron_Image* image) {
    if(image->data != NULL) free(image->data);
    if(image != NULL) free(image);
	image = NULL; // set address to null
}

// Filtering operations

Rasteron_Image* createGreyImg(const Rasteron_Image* refImage) {
	if (refImage == NULL) {
		perror("Cannot create grey image! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Image* greyImage = allocNewImg("grey", refImage->height, refImage->width);
	
	for (unsigned p = 0; p < greyImage->width * greyImage->height; p++)
		*(greyImage->data + p) = grayify32(*(refImage->data + p));

	return greyImage;
}

Rasteron_Image* createFltChanImg(const Rasteron_Image* refImage, CHANNEL_Type channel) {
	if (refImage == NULL) {
		perror("Cannot create filter image! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Image* channelImage;
	uint32_t colorMask; // used for isolating a specific color value
	switch(channel){
		case CHANNEL_Red:
			channelImage = allocNewImg("filter-red", refImage->height, refImage->width);
			colorMask = RED_CHANNEL;
			break;
		case CHANNEL_Green:
			channelImage = allocNewImg("filter-green", refImage->height, refImage->width);
			colorMask = GREEN_CHANNEL;
			break;
		case CHANNEL_Blue:
			channelImage = allocNewImg("filter-blue", refImage->height, refImage->width);
			colorMask = BLUE_CHANNEL;
			break;
	}
	
	uint32_t color;
	for (unsigned p = 0; p < channelImage->width * channelImage->height; p++) {
		color = (*(refImage->data + p) & ALPHA_CHANNEL) + (*(refImage->data + p) & colorMask) ;
		*(channelImage->data + p) = color;
	}

	return channelImage;
}

Rasteron_Image* createAvgChanImg(const Rasteron_Image* refImage, CHANNEL_Type channel) {
	if (refImage == NULL) {
		perror("Cannot create averaged image! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Image* greyImage = createGreyImg(refImage); // greyscale image used as reference
	Rasteron_Image* channelImage = createFltChanImg(greyImage, channel);

	switch(channel){ // renaming
		case CHANNEL_Red: channelImage->name = "average-red"; break;
		case CHANNEL_Green: channelImage->name = "average-green"; break;
		case CHANNEL_Blue: channelImage->name = "average-blue"; break;
	}

	deleteImg(greyImage);
	return channelImage;
}

// Combination and Seeding operations

Rasteron_Image* createBlendImg(const Rasteron_Image* image1, const Rasteron_Image* image2){
	if (image1 == NULL || image2 == NULL || image1->height != image2->height || image1->width != image2->width) {
		perror("Cannot create seeded image! Invalid parameters provided!");
		return NULL;
	}

	Rasteron_Image* blendImage = allocNewImg("blend", image1->height, image1->width);
	for(unsigned p = 0; p < image1->width * image1->height; p++)
		*(blendImage->data + p) = blend(*(image1->data + p), *(image2->data + p), 0.5); // blends halfway

	return blendImage;
}

Rasteron_Image* createFuseImg(const Rasteron_Image* image1, const Rasteron_Image* image2){
	if (image1 == NULL || image2 == NULL || image1->height != image2->height || image1->width != image2->width) {
		perror("Cannot create seeded image! Invalid parameters provided!");
		return NULL;
	}

	Rasteron_Image* fuseImage = allocNewImg("fuse", image1->height, image1->width);
	for(unsigned p = 0; p < image1->width * image1->height; p++)
		*(fuseImage->data + p) = fuse(*(image1->data + p), *(image2->data + p), 0.5); // fuses halfway

	return fuseImage;
}

Rasteron_Image* createSeedRawImg(const Rasteron_Image* refImage, uint32_t color, double prob){
	if (refImage == NULL) {
		perror("Cannot create seeded image! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Image* seedImage = allocNewImg("seeded", refImage->height, refImage->width);
	if(prob >= 1.0){
		makeColor(seedImage->data, seedImage->width * seedImage->height, color);
		return seedImage;
	} else if(prob <= 0.0) return seedImage; // no modifications

	double chance = 0.0;
	for (unsigned p = 0; p < seedImage->width * seedImage->height; p++) {
		chance = (double)rand() / (double)RAND_MAX;
		if (chance <= prob) *(seedImage->data + p) = color; // if chance is less than probability change to target color
		else *(seedImage->data + p) = *(refImage->data + p); // otherwise copy ref contents
	}

	return seedImage;
}

Rasteron_Image* createSeedWeightImg(const Rasteron_Image* refImage, const Rasteron_SeedTable* seedTable){
	if (refImage == NULL) {
		perror("Cannot create palette image! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Image* seedImage = allocNewImg("palette", refImage->height, refImage->width);
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
	for (unsigned p = 0; p < seedImage->width * seedImage->height; p++) {
		chance = ((double)rand() / (double)RAND_MAX) * seedWeightTotal;
		unsigned color = *(refImage->data + p);
		for(unsigned s = 0; s < seedTable->seedCount; s++)
			if(chance > seedRanges[s][0] && chance < seedRanges[s][1]){ // checks if rand number is within bounds
				color = seedTable->seeds[s].color; // sets new color if range is satisfied
				break;
			}
		*(seedImage->data + p) = color;
	}

	return seedImage;
}

// Mapped operations

Rasteron_Image* createMappedImg(ImageSize size, mapCallback callback){
	Rasteron_Image* mappedImage = allocNewImg("map", size.height, size.width);

	for(unsigned p = 0; p < mappedImage->width * mappedImage->height; p++){
		double x = (1.0 / (double)size.width) * (p % size.width);
		double y = (1.0 / (double)size.height) * (p / size.width);
		*(mappedImage->data + p) = callback(x, y);
	}

	return mappedImage;
}