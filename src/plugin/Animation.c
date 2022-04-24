#include "Animation.h"

static char* getFrameName(char* prefix, unsigned frameIndex) {
	// Naming the Image by Frame
	char* name = "";
	strcpy(name, prefix);
	char* suffix = (char)frameIndex + '0';
	strcat(name, &suffix);
	return name;
}

Rasteron_Animation* allocNewAnim(const char* prefix, ImageSize size, unsigned frameCount){
    Rasteron_Animation* animation = (Rasteron_Animation*)malloc(sizeof(Rasteron_Animation));
    animation->prefix = prefix;
    animation->height = size.height;
    animation->width = size.width;
    animation->frameCount = frameCount;
    animation->bkColor = ANIM_BACKGROUND; // cyan background by default

    animation->data = (Rasteron_Image**)malloc(frameCount * sizeof(Rasteron_Image*));
    for(unsigned f = 0; f < frameCount; f++)
        *(animation->data + f) = createImgBlank(size.height, size.width, animation->bkColor);
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

	char* name = getFrameName(animation->prefix, frameIndex);

    // Copying Image Contents
    Rasteron_Image* targetImage = *(animation->data + frameIndex);
	if (refImage->width == (targetImage)->width && refImage->height == targetImage->height){ // checks for size compatability
		targetImage->name = name;
        for (unsigned p = 0; p < refImage->width * refImage->height; p++)
			*(targetImage->data + p) = *(refImage->data + p); // copies data pixel by pixel
    }
    else {
        deleteImg(targetImage); // deletes old image contents
        targetImage = allocNewImg(name, refImage->height, refImage->width);
        for (unsigned p = 0; p < refImage->width * refImage->height; p++)
			*(targetImage->data + p) = *(refImage->data + p); // copies data pixel by pixel
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