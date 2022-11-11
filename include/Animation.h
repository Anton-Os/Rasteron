#ifndef RASTERON_ANIM_H


#include "Image.h"

#define ANIM_HEIGHT 256
#define ANIM_WIDTH 256
#define ANIM_BACKGROUND 0xFFFFFF00 // yellow color

typedef struct {
    char* prefix; // each child name follows the prefix with a number

    unsigned short frameCount;
    Rasteron_Image** frameData;
} Rasteron_Animation;

Rasteron_Animation* allocNewAnim(const char* prefix, /* ImageSize size, */ unsigned frameCount);

Rasteron_Image* getFrame(Rasteron_Animation* animation, unsigned short frameIndex); // gets frame from an index within animation
Rasteron_Image* createCompositeImg(Rasteron_Animation* animation); // combines frames into composite image to be read
void addFrameData(Rasteron_Animation* anim, const Rasteron_Image *const ref, unsigned short frameIndex); // copies target image into index of animation struct

void deleteAnim(Rasteron_Animation* anim);

#define RASTERON_ANIM_H
#endif