#include "typedef.h"

extern int _invertImage = INVERT_IMG_FALSE; // false for within Rasteron

// --------------------------------   Image    -------------------------------- //

ImageSize internal_create_size(unsigned height, unsigned width){ return (ImageSize){ height, width }; }

Rasteron_Image* internal_alloc_img(const char* name, uint32_t height, uint32_t width){
	static unsigned id = 0;

	Rasteron_Image* image = (Rasteron_Image*)malloc(sizeof(Rasteron_Image));
	
	image->name = name; // TODO: Append id hash next to name
	image->width = (!_invertImage)? width: height;
	image->height = (!_invertImage)? height : width;
	image->data = (uint32_t*)malloc(width * height * sizeof(uint32_t));

	id++;
	return image;
}

void internal_dealloc_img(Rasteron_Image* image){
	if(image->data != NULL) free(image->data);
    if(image != NULL) free(image);
	image = NULL; // set address to null
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

	swatch.colors[SWATCH_Red_Add] = color + (redAdd << 16) | (greenAdd << 8);
	swatch.colors[SWATCH_Green_Add] = color + (greenAdd << 8) | blueAdd;
	swatch.colors[SWATCH_Blue_Add] = color + (redAdd << 16) | (blueBit + blueAdd);
	swatch.colors[SWATCH_Light] = color + (redAdd << 16) | (greenAdd << 8) | blueAdd;
	swatch.colors[SWATCH_Dark] = color - ((redSub << 16) | (greenSub << 8) | (blueSub));
	swatch.colors[SWATCH_Red_Sub] = color - ((redSub << 16) | (greenSub << 8));
	swatch.colors[SWATCH_Green_Sub] = color - ((greenSub << 8) | blueSub);
	swatch.colors[SWATCH_Blue_Sub] = color - ((redSub << 16) | blueSub);

	return swatch;
}

// --------------------------------   PixelPoint & ColorPoint    -------------------------------- //

void pixelPointToTable(PixelPointTable* table, double xFrac, double yFrac){
    if(table->pointCount < MAX_PIXELPOINTS){
		table->points[table->pointCount].x = xFrac;
		table->points[table->pointCount].y = yFrac;
		table->pointCount++;
	}
}

void colorPointToTable(ColorPointTable* table, unsigned color, double xFrac, double yFrac){
	if(table->pointCount < MAX_PIXELPOINTS){
		table->points[table->pointCount].x = xFrac;
		table->points[table->pointCount].y = yFrac;
		table->points[table->pointCount].color = color;
		table->pointCount++;
	}
}

// --------------------------------   Cellwise    -------------------------------- //


extern nebrFlags neighbor_exists(uint32_t index, uint32_t width, uint32_t height);
extern uint32_t* neighbor_get(Rasteron_Image* refImage, uint32_t index, enum NEBR_CellFlag whichNebr);
extern unsigned neighbor_getOffset(unsigned width, unsigned offset, enum NEBR_CellFlag whichNebr);
extern unsigned short neighbor_count(unsigned color, unsigned neighbors[8]);

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
		currentTable->flags = neighbor_exists(pIndex, refImage->width, refImage->height);

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
				case NEBR_Bot_Right: *(currentTable->nebrs + i) = neighbor_get(refImage, pIndex, NEBR_Bot_Right); break;
				case NEBR_Bot: *(currentTable->nebrs + i) = neighbor_get(refImage, pIndex, NEBR_Bot); break;
				case NEBR_Bot_Left: *(currentTable->nebrs + i) = neighbor_get(refImage, pIndex, NEBR_Bot_Left); break;
				case NEBR_Right: *(currentTable->nebrs + i) = neighbor_get(refImage, pIndex, NEBR_Right); break;
				case NEBR_Left : *(currentTable->nebrs + i) = neighbor_get(refImage, pIndex, NEBR_Left); break;
				case NEBR_Top_Right: *(currentTable->nebrs + i) = neighbor_get(refImage, pIndex, NEBR_Top_Right); break;
				case NEBR_Top: *(currentTable->nebrs + i) = neighbor_get(refImage, pIndex, NEBR_Top); break;
				case NEBR_Top_Left: *(currentTable->nebrs + i) = neighbor_get(refImage, pIndex, NEBR_Top_Left); break;
				}
				i++;
			}
		}
		pIndex++;
	}
	return list;
}

void delNebrTables(NebrTable_List* nebrTables) {
	if(nebrTables != NULL){
		for (NebrTable* currentTable = nebrTables->tables;
			currentTable != nebrTables->tables + nebrTables->count; 
			currentTable++) {
			if(currentTable != NULL) free(currentTable->nebrs);
		}
		free(nebrTables->tables);
		nebrTables->tables = NULL;
		free(nebrTables);
		nebrTables = NULL;
	}
}
