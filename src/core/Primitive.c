#include "Rasteron.h"

// Sprite related operations

Rasteron_Sprite* createSprite(const Rasteron_Image* refImage){
	if (refImage == NULL) {
		perror("Cannot create palette! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Sprite* sprite = (Rasteron_Sprite*)malloc(sizeof(Rasteron_Sprite));

	sprite->image = refImage; // Simply copy a pointer to the image

	sprite->bounds.topRight_point[0] = refImage->width / 2.0f / SPRITE_DIM_RATIO ;
	sprite->bounds.topRight_point[1] = refImage->height / 2.0f / SPRITE_DIM_RATIO ;

	sprite->bounds.topLeft_point[0] = -1.0 * (refImage->width / 2.0f / SPRITE_DIM_RATIO );
	sprite->bounds.topLeft_point[1] = refImage->height / 2.0f / SPRITE_DIM_RATIO ;

	sprite->bounds.botRight_point[0] = refImage->width / 2.0f / SPRITE_DIM_RATIO ;
	sprite->bounds.botRight_point[1] = -1.0 * (refImage->height / 2.0f / SPRITE_DIM_RATIO );

	sprite->bounds.botLeft_point[0] = -1.0 * (refImage->width / 2.0f / SPRITE_DIM_RATIO );
	sprite->bounds.botLeft_point[1] = -1.0 * (refImage->height / 2.0f / SPRITE_DIM_RATIO );

	return sprite;
}

void deleteSprite(Rasteron_Sprite* sprite){
	if(sprite != NULL) free(sprite);
	sprite = NULL;
}

// Palette related operations

Rasteron_Palette* createPalette(const Rasteron_Image* refImage){
	if (refImage == NULL) {
		perror("Cannot create palette! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Palette* palette = (Rasteron_Palette*)malloc(sizeof(Rasteron_Palette));
	palette->colorsFound = 0;

	for (unsigned p = 0; p < refImage->width * refImage->height; p++) {
		unsigned targetIndex = 0;
		while (targetIndex <= palette->colorsFound) { // step one is find target color in our palette
			if (*(refImage->data + p) == palette->colors[targetIndex][COLOR_OFFSET]) break;
			else targetIndex++;
		}

		if (targetIndex == palette->colorsFound + 1) { // if a match does not exist
			palette->colors[palette->colorsFound][COLOR_OFFSET] = *(refImage->data + p);
			palette->colors[palette->colorsFound][COLOR_COUNT_OFFSET] = 1; // only one pixel found
			palette->colorsFound++;
		}
		else palette->colors[targetIndex][COLOR_COUNT_OFFSET]++;
	}

	return palette;
}

Rasteron_Palette* filterPalette(unsigned minColorCount, const Rasteron_Palette* refImage){
	if (refImage == NULL) {
		perror("Cannot create palette! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Palette* palette = (Rasteron_Palette*)malloc(sizeof(Rasteron_Palette));
	palette->colorsFound = 0;

	for(unsigned p = 0; p < refImage->colorsFound; p++)
		if(refImage->colors[p][COLOR_COUNT_OFFSET] >= minColorCount){
			palette->colors[palette->colorsFound][COLOR_OFFSET] = refImage->colors[palette->colorsFound][COLOR_OFFSET];
			palette->colors[palette->colorsFound][COLOR_COUNT_OFFSET] = refImage->colors[palette->colorsFound][COLOR_COUNT_OFFSET];
			palette->colorsFound++;
		}
	
	return palette;
}

void deletePalette(Rasteron_Palette* palette){ 
	if(palette != NULL) free(palette); 
	palette = NULL;
}