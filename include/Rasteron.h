#ifndef RASTERON_H

#include "ImageLoader.h"

#ifdef __cplusplus
extern "C"{
#endif

// Loader.c declarations

typedef Image FileImage; // Just makes an easier distinction between Rasteron image and file Image

void rstnLoadFromFile(const char* fileName, FileImage* image); // Image is destination
void rstnDelFromFile(FileImage* image);

// Primitives.c declarations
// Rasteron specific image type, based on file image with a name (label) attached

typedef struct {
	uint32_t width;
	uint32_t height;

    // Base of the image
    char* name;
    uint32_t* data;
} Rasteron_Image;

Rasteron_Image* rstnCreate_ImgBase(const Image* image); // Creates an image based on my readers
Rasteron_Image* rstnCreate_ImgGrey(const Rasteron_Image* ref);

void rstnDel_Img(Rasteron_Image* rstn_image);

// Sprite is a wrapper around a base image and coordinates

#define DIMENSION_RATIO_UNSIGNED_TO_FLOAT 1000

typedef struct {
    float topLeft_Pt[2];
    float topRight_Pt[2];
    float botLeft_Pt[2];
    float botRight_Pt[2];
} Rasteron_BoxBounds;

typedef struct {
    const Rasteron_Image* image;
    Rasteron_BoxBounds bounds;
} Rasteron_Sprite;

Rasteron_Sprite* rstnCreate_Sprite(const Rasteron_Image* ref);

void rstnDel_Sprite(Rasteron_Sprite* sprite);

// Palette holds the frequencies of colors that appear in an image

#define MAX_COLOR_TABLE_VALS 65536 // 2 ^ 16

typedef struct {
    unsigned imgPix_colors[MAX_COLOR_TABLE_VALS]; // Target colors
    unsigned imgPix_counts[MAX_COLOR_TABLE_VALS]; // Corresponding count of target colors
    unsigned colorIndex; // Increments as more colors are found
    unsigned bkIndex; // The index holding the background color
} Rasteron_Palette;

Rasteron_Palette* rstnCreate_Palette(const Rasteron_Image* ref);

void rstnDel_Palette(Rasteron_Palette* palette);

// Outline should approximately trace the target image

#define MAX_VERTEX_POS_VALS 32768 // 2 ^ 15

typedef struct {
    float xVals[MAX_VERTEX_POS_VALS];
    float yVals[MAX_VERTEX_POS_VALS];
    unsigned vertexIndex;
} Rasteron_Outline;

Rasteron_Outline* rstnCreate_Outline(const Rasteron_Image* ref);
// Rasteron_Outline* rstnCreate_OutlineColor(const Rasteron_Image* ref, unsigned color); // Colored outline

void rstnDel_Outline(Rasteron_Outline* outline);


#ifdef __cplusplus
}
#endif

#define RASTERON_H
#endif