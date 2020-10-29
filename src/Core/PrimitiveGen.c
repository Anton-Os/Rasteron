#include "Rasteron.h"

// Sprite related stuff

Rasteron_Sprite* rstnCreate_Sprite(const Rasteron_Image* ref){
	if (ref == NULL) {
		puts("Cannot create palette! Null pointer provided!");
		return NULL;
	}

	Rasteron_Sprite* rstn_sprite = (Rasteron_Sprite*)malloc(sizeof(Rasteron_Sprite));

	rstn_sprite->image = ref; // Simply copy a pointer to the image

	rstn_sprite->bounds.topRight_Pt[0] = ref->width / 2.0f / DIMENSION_RATIO_UNSIGNED_TO_FLOAT;
	rstn_sprite->bounds.topRight_Pt[1] = ref->height / 2.0f / DIMENSION_RATIO_UNSIGNED_TO_FLOAT;

	rstn_sprite->bounds.topLeft_Pt[0] = -1.0 * (ref->width / 2.0f / DIMENSION_RATIO_UNSIGNED_TO_FLOAT);
	rstn_sprite->bounds.topLeft_Pt[1] = ref->height / 2.0f / DIMENSION_RATIO_UNSIGNED_TO_FLOAT;

	rstn_sprite->bounds.botRight_Pt[0] = ref->width / 2.0f / DIMENSION_RATIO_UNSIGNED_TO_FLOAT;
	rstn_sprite->bounds.botRight_Pt[1] = -1.0 * (ref->height / 2.0f / DIMENSION_RATIO_UNSIGNED_TO_FLOAT);

	rstn_sprite->bounds.botLeft_Pt[0] = -1.0 * (ref->width / 2.0f / DIMENSION_RATIO_UNSIGNED_TO_FLOAT);
	rstn_sprite->bounds.botLeft_Pt[1] = -1.0 * (ref->height / 2.0f / DIMENSION_RATIO_UNSIGNED_TO_FLOAT);

	return rstn_sprite;
}

void rstnDel_Sprite(Rasteron_Sprite* sprite){
	free(sprite);
}

// Palette related stuff

Rasteron_Palette* rstnCreate_Palette(const Rasteron_Image* ref){
	if (ref == NULL) {
		puts("Cannot create palette! Null pointer provided!");
		return NULL;
	}

	Rasteron_Palette* rstn_palette = (Rasteron_Palette*)malloc(sizeof(Rasteron_Palette));
	rstn_palette->colorIndex = 0;

	for (unsigned p = 0; p < ref->width * ref->height; p++) {
		unsigned pixColorDbg = *(ref->data + p); // For testing

		if (rstn_palette->colorIndex == MAX_COLOR_TABLE_VALS) {
			puts("Color pallette exceeded max capacity");
			break; // Image's pallette is too big
		}

		unsigned colorTargetIdx = 0;
		while (colorTargetIdx <= rstn_palette->colorIndex) { // Step one is find target color in our palette
			if (*(ref->data + p) == rstn_palette->imgPix_colors[colorTargetIdx])
				break;
			else colorTargetIdx++;
		}

		if (colorTargetIdx == rstn_palette->colorIndex + 1) { // Match does not exist
			rstn_palette->imgPix_colors[rstn_palette->colorIndex] = *(ref->data + p);
			rstn_palette->imgPix_counts[rstn_palette->colorIndex] = 1; // Because there is one pixel
			rstn_palette->colorIndex++;
		}
		else
			rstn_palette->imgPix_counts[colorTargetIdx]++;
	}

	return rstn_palette;
}

void rstnDel_Palette(Rasteron_Palette* palette){
	free(palette);
}

// Outline related stuff

Rasteron_Outline* rstnCreate_Outline(const Rasteron_Image* ref){
	if (ref == NULL) {
		puts("Cannot create palette! Null pointer provided!");
		return NULL;
	}

	Rasteron_Outline* rstn_outline = (Rasteron_Outline*)malloc(sizeof(Rasteron_Outline));

	unsigned bkColor = getBkColor(ref->data, ref->width, ref->height);
	
	return rstn_outline;
}

void rstnDel_Outline(Rasteron_Outline* outline){
	free(outline);
}