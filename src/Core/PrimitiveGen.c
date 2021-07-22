#include "Rasteron.h"

// Sprite related stuff

Rasteron_Sprite* createSprite(const Rasteron_Image* refImage){
	if (refImage == NULL) {
		puts("Cannot create palette! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Sprite* sprite = (Rasteron_Sprite*)malloc(sizeof(Rasteron_Sprite));

	sprite->image = refImage; // Simply copy a pointer to the image

	sprite->bounds.topRight_Pt[0] = refImage->width / 2.0f / SPRITE_DIM_RATIO ;
	sprite->bounds.topRight_Pt[1] = refImage->height / 2.0f / SPRITE_DIM_RATIO ;

	sprite->bounds.topLeft_Pt[0] = -1.0 * (refImage->width / 2.0f / SPRITE_DIM_RATIO );
	sprite->bounds.topLeft_Pt[1] = refImage->height / 2.0f / SPRITE_DIM_RATIO ;

	sprite->bounds.botRight_Pt[0] = refImage->width / 2.0f / SPRITE_DIM_RATIO ;
	sprite->bounds.botRight_Pt[1] = -1.0 * (refImage->height / 2.0f / SPRITE_DIM_RATIO );

	sprite->bounds.botLeft_Pt[0] = -1.0 * (refImage->width / 2.0f / SPRITE_DIM_RATIO );
	sprite->bounds.botLeft_Pt[1] = -1.0 * (refImage->height / 2.0f / SPRITE_DIM_RATIO );

	return sprite;
}

void deleteSprite(Rasteron_Sprite* sprite){
	free(sprite);
}

// Palette related stuff

Rasteron_Palette* createPalette(const Rasteron_Image* refImage){
	if (refImage == NULL) {
		puts("Cannot create palette! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Palette* palette = (Rasteron_Palette*)malloc(sizeof(Rasteron_Palette));
	palette->maxColors = 0;

	for (unsigned p = 0; p < refImage->width * refImage->height; p++) {
		unsigned targetIndex = 0;
		while (targetIndex <= palette->maxColors) { // step one is find target color in our palette
			if (*(refImage->data + p) == palette->colors[targetIndex][COLOR_CODE_OFFSET]) break;
			else targetIndex++;
		}

		if (targetIndex == palette->maxColors + 1) { // match does not exist
			palette->colors[palette->maxColors][COLOR_CODE_OFFSET] = *(refImage->data + p);
			palette->colors[palette->maxColors][COLOR_COUNT_OFFSET] = 1; // only one pixel found
			palette->maxColors++;
		}
		else palette->colors[targetIndex][COLOR_COUNT_OFFSET]++;
	}

	return palette;
}

Rasteron_Palette* filterPalette(unsigned minPixelCount, const Rasteron_Palette* refImage){
	if (refImage == NULL) {
		puts("Cannot create palette! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Palette* palette = (Rasteron_Palette*)malloc(sizeof(Rasteron_Palette));
	palette->maxColors = 0;

	for(unsigned p = 0; p < refImage->maxColors; p++)
		if(refImage->colors[p][COLOR_COUNT_OFFSET] >= minPixelCount){
			palette->colors[palette->maxColors][COLOR_CODE_OFFSET] = refImage->colors[palette->maxColors][COLOR_CODE_OFFSET];
			palette->colors[palette->maxColors][COLOR_COUNT_OFFSET] = refImage->colors[palette->maxColors][COLOR_COUNT_OFFSET];
			palette->maxColors++;
		}
	
	return palette;
}

void deletePalette(Rasteron_Palette* palette){ free(palette); }