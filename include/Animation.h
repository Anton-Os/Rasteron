#include "Rasteron.h"

// There is a default blank image as a placeholder for data inside animation struct
typedef struct {
    char* namePrefix; // each child name follows the prefix with a number
    uint32_t width; // each child inside data inherits the width
    uint32_t height; // each child inside data inherits the height

    unsigned frameCount;
    Rasteron_Image** data;
} Rasteron_Animation;

Rasteron_Animation* allocNewAnim(const char* prefix, uint32_t height, uint32_t width, unsigned frameCount);
void addFraneData(Rasteron_Animation* anim, const Rasteron_Image *const ref, unsigned frameIndex); // copies target image into index of animation struct
void deleteAnim(Rasteron_Animation* anim);