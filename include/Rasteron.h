#include "ImageLoader.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct {
	uint32_t width;
	uint32_t height;

    // Base of the image
    char* name;
    uint32_t* data;
} Rasteron_Image;

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

Rasteron_Image* rstnCreate_Base(const Image* image); // Creates an image based on my readers
Rasteron_Image* rstnCreate_Grey(const Rasteron_Image* ref);

void delImage_Rstn(Rasteron_Image* rstn_image);

#ifdef __cplusplus
}
#endif