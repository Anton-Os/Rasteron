#include "Queue_Feature.h"

// --------------------------------  Queue Operations  -------------------------------- //

Rasteron_Queue* alloc_queue(const char* prefix, ImageSize size, unsigned frameCount){
    Rasteron_Queue* queue = (Rasteron_Queue*)malloc(sizeof(Rasteron_Queue));
    queue->prefix = prefix;
    queue->index = QUEUE_STATE_DEFAULT;
    queue->frameCount = frameCount;

    queue->frameData = (Rasteron_Image**)malloc(frameCount * sizeof(Rasteron_Image*));
    for(unsigned f = 0; f < frameCount; f++){
        *(queue->frameData + f) = solidImgOp(size, QUEUE_IMAGE_BACKGROUND);

        char frameStr[1024];
        sprintf(frameStr, "frame%d", f + 1);
        (*(queue->frameData + f))->name = frameStr;
    }

    queue->state = *queue->frameData;

    return queue;
}

void addFrameAt(Rasteron_Queue* queue, const Rasteron_Image *const refImage, unsigned short frameIndex){
	assert(frameIndex >= queue->frameCount);
    dealloc_image(*(queue->frameData + frameIndex)); // deleting old image if exists

    queue->index = frameIndex;
    *(queue->frameData + frameIndex) = copyImgOp(refImage); // creating copy image
    queue->state = *(queue->frameData + frameIndex);
}

Rasteron_Image* getFrameAt(Rasteron_Queue* queue, unsigned short frameIndex){
    assert(frameIndex >= queue->frameCount);

    queue->index = frameIndex;
    queue->state = *(queue->frameData + frameIndex);
    return *(queue->frameData + frameIndex);
}

void dealloc_queue(Rasteron_Queue* queue){
    if(queue != NULL){
        for(unsigned f = 0; f < queue->frameCount; f++)
            dealloc_image(*(queue->frameData + f));
        free(queue->frameData);
        free(queue);
        queue = NULL;
    }
}

// --------------------------------  UI Operations  -------------------------------- //

ImageSize getUI_ImageSize(enum MENU_Size size){
    switch(size){
        case MENU_Tiny: return (ImageSize){ 16, 16 };
        case MENU_Small: return (ImageSize){ 32, 32 }; 
        case Menu_Medium: return (ImageSize){ 64, 64 };
        case Menu_Large: return (ImageSize){ 128, 128 };
        case Menu_XL: return (ImageSize){ 256, 256 };
    }

    return (ImageSize){ 0, 0 };
}

Rasteron_Queue* loadUI_keyButton(enum MENU_Size size, char character){
    Rasteron_Queue* menuQueue = (Rasteron_Queue*)alloc_queue("key", getUI_ImageSize(size), 2);

    // TODO: Update queue data

    return menuQueue;
}

Rasteron_Queue* loadUI_toggleButton(enum MENU_Size size){
    Rasteron_Queue* menuQueue = (Rasteron_Queue*)alloc_queue("toggle", getUI_ImageSize(size), 2);

    // TODO: Update queue data

    return menuQueue;
}

Rasteron_Queue* loadUI_dial(enum MENU_Size size, unsigned short steps){
    assert(steps > 1);

    Rasteron_Queue* menuQueue = (Rasteron_Queue*)alloc_queue("dial", getUI_ImageSize(size), steps);

    // TODO: Update queue data

    return menuQueue;
}

Rasteron_Queue* loadUI_slider(enum MENU_Size size, unsigned short levels){
    assert(levels > 1);

    Rasteron_Queue* menuQueue = (Rasteron_Queue*)alloc_queue("slider", getUI_ImageSize(size), levels);

    // TODO: Update queue data

    return menuQueue;
}

// --------------------------------  Animation Operations  -------------------------------- //

#ifdef RASTERON_ENABLE_ANIM

Rasteron_Queue* loadAnim_frame30(ImageSize size){ return (Rasteron_Queue*)alloc_queue("frame30", size, 30); } 

Rasteron_Queue* loadAnim_frame60(ImageSize size){ return (Rasteron_Queue*)alloc_queue("frame60", size, 60); } 

Rasteron_Queue* loadAnim_frame120(ImageSize size){ return (Rasteron_Queue*)alloc_queue("frame120", size, 120); } 

#endif
