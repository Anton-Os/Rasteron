#include "ImageLoader.h"

typedef struct {
	uint32_t width;
	uint32_t height;

	uint8_t count;
    char** labels;
	uint32_t** data; // For animated/layered image support
} Rasteron_ImageSq;

typedef struct {
    float topLeft_Pt[2];
    float topRight_Pt[2];
    float botLeft_Pt[2];
    float botRight_Pt[2];
} Rasteron_BoxBounds;

typedef struct {
    Rasteron_ImageSq images;
    Rasteron_BoxBounds bounds;
} Rasteron_Sprite;

Rasteron_ImageSq* createImageSq_Rstn(const Image* image); // Creates an image based on my readers
Rasteron_Sprite* createSprite_Rstn(const Rasteron_ImageSq* rstn_image); // Creates a sprite based on rstn image above

void delImageSq_Rstn(Rasteron_ImageSq* rstn_image);
void delSprite_Rstn(Rasteron_Sprite* rstn_sprite);