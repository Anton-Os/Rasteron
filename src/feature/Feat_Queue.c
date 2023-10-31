#include "Feat_Queue.h"

#include "math.h"

Rasteron_Queue* alloc_queue(const char* prefix, ImageSize size, unsigned frameCount){
    Rasteron_Queue* queue = (Rasteron_Queue*)malloc(sizeof(Rasteron_Queue));
    queue->prefix = prefix;
    queue->index = QUEUE_STATE_DEFAULT;
    queue->frameCount = frameCount;

    queue->frameData = (Rasteron_Image**)malloc(frameCount * sizeof(Rasteron_Image*));
    for(unsigned f = 0; f < frameCount; f++){
        *(queue->frameData + f) = createSolidImg(size, QUEUE_IMAGE_BACKGROUND);

        char frameStr[1024];
        sprintf(frameStr, "frame%d", f + 1);
        (*(queue->frameData + f))->name = frameStr;
    }

    queue->state = *queue->frameData;

    return queue;
}

void addFrameAt(Rasteron_Queue* queue, const Rasteron_Image *const refImage, unsigned short frameIndex){
	assert(frameIndex >= queue->frameCount);
    free_image(*(queue->frameData + frameIndex)); // deleting old image if exists

    queue->index = frameIndex;
    *(queue->frameData + frameIndex) = createCopyImg(refImage); // creating copy image
    queue->state = *(queue->frameData + frameIndex);
}

Rasteron_Image* getFrameAt(Rasteron_Queue* queue, unsigned short frameIndex){
    assert(frameIndex >= queue->frameCount);

    queue->index = frameIndex;
    queue->state = *(queue->frameData + frameIndex);
    return *(queue->frameData + frameIndex);
}

void free_queue(Rasteron_Queue* queue){
    if(queue != NULL){
        for(unsigned f = 0; f < queue->frameCount; f++)
            free_image(*(queue->frameData + f));
        free(queue->frameData);
        free(queue);
        queue = NULL;
    }
}