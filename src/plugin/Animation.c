#include "Animation.h"

Rasteron_Animation* allocNewAnim(const char* prefix, uint32_t height, uint32_t width, unsigned frameCount){
    Rasteron_Animation* animation = (Rasteron_Animation*)malloc(sizeof(Rasteron_Animation));
    animation->prefix = prefix;
    animation->height = height;
    animation->width = width;
    animation->frameCount = frameCount;
    animation->bkColor = ANIM_BACKGROUND; // cyan background by default

    animation->data = (Rasteron_Image**)malloc(frameCount * sizeof(Rasteron_Image*));
    for(unsigned f = 0; f < frameCount; f++)
        *(animation->data + f) = createImgBlank(height, width, animation->bkColor);
    return animation;
}

Rasteron_Image* getFrame(Rasteron_Animation* animation, unsigned short frameIndex){
    if (frameIndex >= animation->frameCount) {
		perror("Frame index out of range!");
		return NULL;
	}
    return *(animation->data + frameIndex);
}

void addFrameData(Rasteron_Animation* animation, const Rasteron_Image *const refImage, unsigned short frameIndex){
	if (frameIndex >= animation->frameCount) {
		perror("Frame index out of range!");
		return;
	}
	else {
		char* name = animation->prefix; strcat(name, "-frame");
		(*(animation->data + frameIndex))->name = name;
	}
	// else (*(animation->data + frameIndex))->name = strcat(animation->prefix, "-frame"); // rename image as part of animation prefix

	if (refImage->width == (*(animation->data + frameIndex))->width && refImage->height == (*(animation->data + frameIndex))->height) // checks for size compatability
		for (unsigned p = 0; p < refImage->width * refImage->height; p++)
			*((*(animation->data + frameIndex))->data + p) = *(refImage->data + p); // copies data pixel by pixel
    else {
        // TODO: Delete the old image and create anew
    }
}

void deleteAnim(Rasteron_Animation* animation){
    if(animation != NULL){
        for(unsigned f = 0; f < animation->frameCount; f++)
            deleteImg(*(animation->data + f));
        free(animation->data);
        free(animation);
        animation = NULL;
    }
}