#include "Animation.h"

Rasteron_Animation* allocNewAnim(const char* prefix, uint32_t height, uint32_t width, unsigned frameCount){
    Rasteron_Animation* animation = (Rasteron_Animation*)malloc(sizeof(Rasteron_Animation));
    animation->prefix = prefix;
    animation->height = height;
    animation->width = width;
    animation->frameCount = frameCount;
    animation->bkColor = 0xFF111111; // dark grey background by default

    animation->data = (Rasteron_Image**)malloc(frameCount * sizeof(Rasteron_Image*));
    for(unsigned f = 0; f < frameCount; f++)
        *(animation->data + f) = createImgBlank(height, width, animation->bkColor);
    return animation;
}

void addFrameData(Rasteron_Animation* animation, const Rasteron_Image *const refImage, unsigned frameIndex){
    if(frameIndex >= animation->frameCount){
        puts("Frame index out of range!");
        return;
    } else (*(animation->data + frameIndex))->name = animation->prefix; // rename image as part of animation prefix

    if(refImage->width == animation->width && refImage->height == animation->width) // sizes are valid
        for(unsigned p = 0; p < refImage->width * refImage->height; p)
            *((*(animation->data + frameIndex))->data + p) = *(refImage->data + p);
    else {
        puts("Animation frame sizes incompatible!");
        return;
    }
}

void deleteAnim(Rasteron_Animation* animation){
    for(unsigned f = 0; f < animation->frameCount; f++)
        deleteImg(*(animation->data + f));
    free(animation->data);
}