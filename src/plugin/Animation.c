#include "Animation.h"

#include "math.h"

Rasteron_Animation* alloc_animation(const char* prefix, /* ImageSize size, */ unsigned frameCount){
    Rasteron_Animation* animation = (Rasteron_Animation*)malloc(sizeof(Rasteron_Animation));
    animation->prefix = prefix;
    animation->frameCount = frameCount;

    animation->frameData = (Rasteron_Image**)malloc(frameCount * sizeof(Rasteron_Image*));
    for(unsigned f = 0; f < frameCount; f++){
        *(animation->frameData + f) = createSolidImg((ImageSize){ ANIM_HEIGHT, ANIM_WIDTH }, ANIM_BACKGROUND);

        char frameStr[1024];
        sprintf(frameStr, "frame%d", f + 1);
        (*(animation->frameData + f))->name = frameStr;
    }
    return animation;
}

Rasteron_Image* getFrame(Rasteron_Animation* animation, unsigned short frameIndex){
    if (frameIndex >= animation->frameCount) {
		perror("Frame index out of range!");
		return NULL;
	}
    return *(animation->frameData + frameIndex);
}

Rasteron_Image* compositeImg(Rasteron_Animation* animation){
    if (animation == NULL) {
		perror("Cannot create copy image! Null pointer provided as animation");
		return NULL;
	}

    unsigned totalSize = 0;
    for(unsigned f = 0; f < animation->frameCount; f++)
        totalSize += getFrame(animation, f)->width * getFrame(animation, f)->height;

    unsigned side = totalSize / getFrame(animation, 0)->width;
    Rasteron_Image* compositeImg = alloc_image("composite", side, totalSize / side);

    unsigned frameIndex = 0;
    unsigned framePixOffset = 0;
    Rasteron_Image* stagingImg = getFrame(animation, frameIndex);
    for(unsigned p = 0; p < compositeImg->width * compositeImg->height; p++){
        *(compositeImg->data + p) = *(stagingImg->data + framePixOffset); // copying pixel operation
        framePixOffset++;
        if(framePixOffset > stagingImg->width * stagingImg->height){  // switching to proper staging image
            framePixOffset = 0; // reset to start of new image 
            frameIndex++;
            stagingImg = getFrame(animation, frameIndex); // move to next image
            if(stagingImg == NULL) break; // frame index out of range
        }
    }

    return compositeImg;
}

void addFrameAt(Rasteron_Animation* animation, const Rasteron_Image *const refImage, unsigned short frameIndex){
	if (frameIndex >= animation->frameCount) {
		perror("Frame index out of range!");
		return;
	}

    free_image(*(animation->frameData + frameIndex)); // deleting old image
    *(animation->frameData + frameIndex) = createCopyImg(refImage); // creating copy image
}

void free_animation(Rasteron_Animation* animation){
    if(animation != NULL){
        for(unsigned f = 0; f < animation->frameCount; f++)
            free_image(*(animation->frameData + f));
        free(animation->frameData);
        free(animation);
        animation = NULL;
    }
}