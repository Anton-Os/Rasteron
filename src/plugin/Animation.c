#include "Animation.h"

Rasteron_Animation* allocNewAnim(const char* prefix, uint32_t height, uint32_t width, unsigned frameCount){
    Rasteron_Animation* animation;
    animation->prefix = prefix;
    animation->height = height;
    animation->width = width;
    animation->frameCount = frameCount;
    animation->bkColor = 0xFF00FFFF;

    animation->data = (Rasteron_Image**)malloc(frameCount * sizeof(Rasteron_Image*));
    for(unsigned f = 0; f < frameCount; f++)
        *(animation->data + f) = createImgBlank(height, width, animation->bkColor);
    return animation;
}

void addFrameData(Rasteron_Animation* animation, const Rasteron_Image *const ref, unsigned frameIndex){
    if(frameIndex >= animation->frameCount) return;
    // else statement copy image contents into existing image
}

void deleteAnim(Rasteron_Animation* animation){
    for(unsigned f = 0; f < animation->frameCount; f++)
        deleteImg(*(animation->data + f));
    free(animation->data);
}