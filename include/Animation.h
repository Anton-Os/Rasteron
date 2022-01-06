#include "Rasteron.h"

// there is a default blank image as a placeholder for data inside animation struct
typedef struct {
    char* prefix; // each child name follows the prefix with a number
    uint32_t width; // each child inherits the width
    uint32_t height; // each child inherits the height
    unsigned bkColor; // each child has a starting background color

    unsigned frameCount;
    Rasteron_Image** data;
} Rasteron_Animation;

Rasteron_Animation* allocNewAnim(const char* prefix, uint32_t height, uint32_t width, unsigned frameCount);
void addFrameData(Rasteron_Animation* anim, const Rasteron_Image *const ref, unsigned frameIndex); // copies target image into index of animation struct
void deleteAnim(Rasteron_Animation* anim);