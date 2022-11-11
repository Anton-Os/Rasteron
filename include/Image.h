#ifndef RASTERON_IMAGE_H

#include "Loader.h"
#include "palette_type.h"

enum FLIP_Type { FLIP_Clock, FLIP_Counter, FLIP_Upside };

typedef struct {
	uint32_t height;
	uint32_t width;
} ImageSize;

typedef struct {
    char* name;
    uint32_t width;
	uint32_t height;
    uint32_t* data;
} Rasteron_Image;

typedef const Rasteron_Image *const ref_image_t; // Replace function parameters with this type

// Basic Image operations

Rasteron_Image* allocNewImg(const char* name, uint32_t height, uint32_t width); // Image allocation helper

Rasteron_Image* createRefImg(const char* fileName); // creates an image from a file
Rasteron_Image* createSolidImg(ImageSize size, uint32_t solidColor); // creates a solid color image
Rasteron_Image* createCopyImg(ref_image_t refImage); // creates copy of image
Rasteron_Image* createMirrorImg(ref_image_t refImage); // creates mirror version of image
Rasteron_Image* createFlipImg(ref_image_t refImage, enum FLIP_Type flip); // creates flipped version of image

void deleteImg(Rasteron_Image* image);

// Filtering Image operations

Rasteron_Image* createGreyscaleImg(ref_image_t refImage); // creates greyscale version of image
Rasteron_Image* createFilterImg(ref_image_t refImage, CHANNEL_Type channel); // creates isolated filter version of image
Rasteron_Image* createChannelImg(ref_image_t refImage, CHANNEL_Type channel); // creates averaged channel version of image

// Combination and Seeding Image operations

Rasteron_Image* createBlendImg(ref_image_t image1, ref_image_t image2); // creates blended version of image
Rasteron_Image* createFuseImg(ref_image_t image1, ref_image_t image2); // creates fused version of image
Rasteron_Image* createSeedRawImg(ref_image_t refImage, uint32_t color, double prob); // seeds pixel over image based on probability
Rasteron_Image* createSeedWeightImg(ref_image_t refImage, const Rasteron_SeedTable* seedTable); // seeds pixels over image based on weights

// Mapped Image operations based on x and y coordinates

typedef unsigned (*mapCallback)(double x, double y);

Rasteron_Image* createMappedImg(ImageSize size, mapCallback callback);
// Rasteron_Image* createBorderImg(ref_image_t refImage, unsigned color, double size);

#define RASTERON_IMAGE_H
#endif