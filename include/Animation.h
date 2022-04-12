#ifndef RASTERON_ANIM_H

#include "Image.h"

#define ANIM_BACKGROUND 0xFFFFFF00 // yellow color

// there is a default blank image as a placeholder for data inside animation struct
typedef struct {
    char* prefix; // each child name follows the prefix with a number
    uint32_t width; // each child inherits the width
    uint32_t height; // each child inherits the height
    unsigned bkColor; // each child has a starting background color

    unsigned short frameCount;
    Rasteron_Image** data;
} Rasteron_Animation;

Rasteron_Animation* allocNewAnim(const char* prefix, uint32_t height, uint32_t width, unsigned frameCount);

Rasteron_Image* getFrame(Rasteron_Animation* animation, unsigned short frameIndex);
void addFrameData(Rasteron_Animation* anim, const Rasteron_Image *const ref, unsigned short frameIndex); // copies target image into index of animation struct

void deleteAnim(Rasteron_Animation* anim);

#define RASTERON_ANIM_H
#endif