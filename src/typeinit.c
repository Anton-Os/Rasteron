#include "typeinit.h"

// --------------------------------   Seed    -------------------------------- //

void seedToTable(ColorSeedTable* table, unsigned color){ 
    table->seeds[table->seedCount].color = color;
	table->seeds[table->seedCount].weight = 0.0; // default seed weight
	table->seedCount++;
}

void seedweightToTable(ColorSeedTable* table, unsigned color, double weight){
    table->seeds[table->seedCount].color = color;
	table->seeds[table->seedCount].weight = weight; // custom seed weight
	table->seedCount++;
}


// --------------------------------   Swatch    -------------------------------- //

ColorSwatch createSwatch(unsigned color, uint8_t deviation){
    ColorSwatch swatch;
	
	swatch.base = color;
	swatch.deviation = deviation;

	uint8_t redBit = (color & RED_CHANNEL) >> 16;
	uint8_t greenBit = (color & GREEN_CHANNEL) >> 8;
	uint8_t blueBit = color & BLUE_CHANNEL;

	uint8_t redAdd = (deviation < 0xFF - redBit)? deviation : 0xFF - redBit;
	uint8_t greenAdd = (deviation < 0xFF - greenBit)? deviation : 0xFF - greenBit;
	uint8_t blueAdd = (deviation < 0xFF - blueBit)? deviation : 0xFF - blueBit;

	uint8_t redSub = (deviation < redBit)? deviation : redBit;
	uint8_t greenSub = (deviation < greenBit)? deviation : greenBit;
	uint8_t blueSub = (deviation < greenBit)? deviation : blueBit;

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

/* ColorSeedTable createSeedTable(const ColorSwatch* swatch){
    ColorSeedTable seedTable = { 0 };
	addSeed(&seedTable, swatch->colors[SWATCH_Yellow_Add]);
	addSeed(&seedTable, swatch->colors[SWATCH_Cyan_Add]);
	addSeed(&seedTable, swatch->colors[SWATCH_Magenta_Add]);
	addSeed(&seedTable, swatch->colors[SWATCH_Light]);
	addSeed(&seedTable, swatch->colors[SWATCH_Dark]);
	addSeed(&seedTable, swatch->colors[SWATCH_Yellow_Sub]);
	addSeed(&seedTable, swatch->colors[SWATCH_Cyan_Sub]);
	addSeed(&seedTable, swatch->colors[SWATCH_Magenta_Sub]);

	return seedTable;
} */

// --------------------------------   PixelPoint & ColorPoint    -------------------------------- //

void pixelPointToTable(PixelPointTable* table, double xFrac, double yFrac){
    table->points[table->pointCount].x = xFrac;
	table->points[table->pointCount].y = yFrac;
	table->pointCount++;
}

void colorPointToTable(ColorPointTable* table, unsigned color, double xFrac, double yFrac){
    table->points[table->pointCount].x = xFrac;
	table->points[table->pointCount].y = yFrac;
	table->points[table->pointCount].color = color;
	table->pointCount++;
}

// --------------------------------   Image    -------------------------------- //

Rasteron_Image* alloc_image(const char* name, uint32_t height, uint32_t width){
	Rasteron_Image* image = (Rasteron_Image*)malloc(sizeof(Rasteron_Image));
	
	image->name = name;
	image->width = width;
	image->height = height;
	image->data = (uint32_t*)malloc(width * height * sizeof(uint32_t));

	return image;
}

void free_image(Rasteron_Image* image){
	if(image->data != NULL) free(image->data);
    if(image != NULL) free(image);
	image = NULL; // set address to null
}

// --------------------------------   Sprite    -------------------------------- //

Rasteron_Sprite* loadSprite(const Rasteron_Image* refImage){
	if (refImage == NULL) {
		perror("Cannot create sprite! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Sprite* sprite = (Rasteron_Sprite*)malloc(sizeof(Rasteron_Sprite));

	sprite->image = refImage; // Simply copy a pointer to the image

	sprite->bounds.topRight[0] = refImage->width / 2.0f / SPRITE_SIZE;
	sprite->bounds.topRight[1] = refImage->height / 2.0f / SPRITE_SIZE;
	sprite->bounds.topLeft[0] = -1.0 * (refImage->width / 2.0f / SPRITE_SIZE);
	sprite->bounds.topLeft[1] = refImage->height / 2.0f / SPRITE_SIZE;
	sprite->bounds.botRight[0] = refImage->width / 2.0f / SPRITE_SIZE;
	sprite->bounds.botRight[1] = -1.0 * (refImage->height / 2.0f / SPRITE_SIZE);
	sprite->bounds.botLeft[0] = -1.0 * (refImage->width / 2.0f / SPRITE_SIZE);
	sprite->bounds.botLeft[1] = -1.0 * (refImage->height / 2.0f / SPRITE_SIZE);

	return sprite;
}

void free_sprite(Rasteron_Sprite* sprite){
	if(sprite->image != NULL) free_image(sprite->image);
	if(sprite != NULL) free(sprite);
	sprite = NULL;
}

// --------------------------------   Heightmap    -------------------------------- //

static double computeHeight(unsigned inputColor, double minBound, double maxBound){;
	uint8_t greyColorRef = grayify8(inputColor);
	double heightVal = (double)greyColorRef; // conversion to double
	heightVal /= (255.0 / maxBound);

	return heightVal + minBound;
}

Rasteron_Heightmap* alloc_heightmap(uint32_t height, uint32_t width, double minBound, double maxBound){
	Rasteron_Heightmap* heightmap = (Rasteron_Heightmap*)malloc(sizeof(Rasteron_Heightmap));
    
	heightmap->height = height;
    heightmap->width = width;
	heightmap->minBound = minBound; // default min is zero
	heightmap->maxBound = maxBound; // default max is one
    heightmap->data = (double*)malloc(heightmap->height * heightmap->width * sizeof(double));

	return heightmap;
}

Rasteron_Heightmap* loadHeightmap(ref_image_t refImage){
	assert(refImage != NULL);

    Rasteron_Heightmap* heightmap = alloc_heightmap(refImage->width, refImage->height, 0.0, 1.0);

	for (unsigned p = 0; p < heightmap->width * heightmap->height; p++)
		*(heightmap->data + p) = computeHeight(*(refImage->data + p), heightmap->minBound, heightmap->maxBound);

    return heightmap;
}

void free_heightmap(Rasteron_Heightmap* heightmap){
	if(heightmap->data != NULL) free(heightmap->data);
    if(heightmap != NULL) free(heightmap);
	heightmap = NULL;
}

// --------------------------------   Cellwise    -------------------------------- //

static void setFlagBit(nebrFlags* target, enum NEBR_CellFlag flagBit){ *target = (*target | (1 << (flagBit))); }

static void clearFlagBit(nebrFlags* target, enum NEBR_CellFlag flagBit){ *target = (*target & (~(1 << (flagBit)))); }

nebrFlags checkExistNebrs(uint32_t index, uint32_t width, uint32_t height){
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

unsigned findNeighborOffset(unsigned width, unsigned offset, enum NEBR_CellFlag whichNebr){
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

uint32_t* findNeighbor(Rasteron_Image* refImage, uint32_t index, enum NEBR_CellFlag whichNebr){
	const uint32_t* target = refImage->data + findNeighborOffset(refImage->width, index, whichNebr);
	return target;
}

/* static unsigned getAvgNebrColor(const NebrTable_List* nebrTables, unsigned offset){
	unsigned nebrCount = 0;
	for(unsigned n = 0; n < NEBR_COUNT; n++) 
		if((nebrTables->tables + offset)->flags | (1 << n)) nebrCount++;

	unsigned long avgColor = 0;
	for(unsigned n = 0; n < nebrCount; n++)
		avgColor += *(*((nebrTables->tables + offset)->nebrs + n));

	printf("Neighbor count is %d", nebrCount); // testing

	return (unsigned)(avgColor / nebrCount);
} */

NebrTable_List* loadNebrTables(ref_image_t refImage){
	NebrTable_List* list = (NebrTable_List*)malloc(sizeof(NebrTable_List));
	list->count = refImage->width * refImage->height;
	list->tables = (NebrTable*)malloc(refImage->width * refImage->height * sizeof(NebrTable));
	
	// const uint32_t* target = refImage->data; // target is a pointer to the first pixel
	unsigned pIndex = 0; // index to the pixel being processed
	for (NebrTable* currentTable = list->tables; // start with the first table
		currentTable != list->tables + (refImage->width * refImage->height); // pointer to end on in the mem space
		currentTable++) { // move to next table pointer

		currentTable->target = refImage->data + pIndex;
		currentTable->flags = checkExistNebrs(pIndex, refImage->width, refImage->height);

		unsigned short nebrCount = 0;
		for (unsigned short n = 0; n < 8; n++) // Determine number of neighbors based on flags
			if (currentTable->flags & (1 << (n))) // Traverse through all flags and increment if present
				nebrCount++;

		currentTable->nebrs = (uint32_t**)malloc(nebrCount * sizeof(uint32_t*));
		unsigned short i = 0; // index to keep track of neighbor
		for (unsigned short n = 0; n < 8; n++) {
			if (currentTable->flags & (1 << n)) {
				// unsigned* nebr = *(currentTable->nebrs + i);
				switch (n) {
				case NEBR_Bot_Right: *(currentTable->nebrs + i) = findNeighbor(refImage, pIndex, NEBR_Bot_Right); break;
				case NEBR_Bot: *(currentTable->nebrs + i) = findNeighbor(refImage, pIndex, NEBR_Bot); break;
				case NEBR_Bot_Left: *(currentTable->nebrs + i) = findNeighbor(refImage, pIndex, NEBR_Bot_Left); break;
				case NEBR_Right: *(currentTable->nebrs + i) = findNeighbor(refImage, pIndex, NEBR_Right); break;
				case NEBR_Left : *(currentTable->nebrs + i) = findNeighbor(refImage, pIndex, NEBR_Left); break;
				case NEBR_Top_Right: *(currentTable->nebrs + i) = findNeighbor(refImage, pIndex, NEBR_Top_Right); break;
				case NEBR_Top: *(currentTable->nebrs + i) = findNeighbor(refImage, pIndex, NEBR_Top); break;
				case NEBR_Top_Left: *(currentTable->nebrs + i) = findNeighbor(refImage, pIndex, NEBR_Top_Left); break;
				}
				i++;
			}
		}
		pIndex++;
	}
	return list;
}

void delNebrTables(NebrTable_List* nebrTables) {
	for (NebrTable* currentTable = nebrTables->tables;
		currentTable != nebrTables->tables + nebrTables->count; 
		currentTable++) {
		free(currentTable->nebrs);
	}
	free(nebrTables->tables);
	nebrTables->tables = NULL;
	free(nebrTables);
	nebrTables = NULL;
}
