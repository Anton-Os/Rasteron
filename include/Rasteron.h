#include "ImageLoader.h"

#ifdef __cplusplus
extern "C"{
#endif

// Image functionality and types

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

// Sprite functionality and types

typedef struct {
    float topLeft_Pt[2];
    float topRight_Pt[2];
    float botLeft_Pt[2];
    float botRight_Pt[2];
} Rasteron_BoxBounds;

typedef struct {
    Rasteron_Image images;
    Rasteron_BoxBounds bounds;
} Rasteron_Sprite;

// Color table functionality and types

#define MAX_COLOR_TABLE_VALS 1024

typedef struct {
    unsigned imgPix_colors[MAX_COLOR_TABLE_VALS]; // Target colors
    unsigned imgPix_counts[MAX_COLOR_TABLE_VALS]; // Corresponding count of target colors
    unsigned colorIndex; // Increments as more colors are found
    unsigned bkIndex; // The index holding the background color

} Rasteron_Palette;

Rasteron_Palette* rstnCreate_Palette(const Rasteron_Image* ref);

void rstnDel_Palette(Rasteron_Palette* palette);

//void del


#ifdef __cplusplus
}
#endif