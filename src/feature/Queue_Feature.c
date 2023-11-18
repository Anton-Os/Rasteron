#include "Queue_Feature.h"

// --------------------------------  Queue Operations  -------------------------------- //

Rasteron_Queue* alloc_queue(const char* prefix, ImageSize size, unsigned frameCount){
    Rasteron_Queue* queue = (Rasteron_Queue*)malloc(sizeof(Rasteron_Queue));
    queue->prefix = prefix;
    queue->index = QUEUE_STATE_DEFAULT;
    queue->frameCount = frameCount;

    queue->frameData = (Rasteron_Image**)malloc(frameCount * sizeof(Rasteron_Image*));
    for(unsigned f = 0; f < frameCount; f++){
        *(queue->frameData + f) = (Rasteron_Image*)malloc(sizeof(Rasteron_Image));
        Rasteron_Image* frame = *(queue->frameData + f);

        char frameStr[1024];
        sprintf(frameStr, "frame%d", f + 1);

        frame->name = frameStr;
        frame->width = size.width;
        frame->height = size.height;
        frame->data = (uint32_t*)malloc(size.width * size.height * sizeof(uint32_t));
        
        for (unsigned i = 0; i < size.height * size.width; i++){
		    uint8_t frameColorBit = (unsigned)(f * (256.0 / frameCount));
            *(frame->data + i) = 0xFF000000 | (frameColorBit << 16) | (frameColorBit << 8) | frameColorBit;
        }
    }

    // queue->state = *queue->frameData;

    return queue;
}

void addFrameAt(Rasteron_Queue* queue, const Rasteron_Image *const refImage, unsigned short frameIndex){
	assert(frameIndex < queue->frameCount);
    dealloc_image(*(queue->frameData + frameIndex)); // deleting old image if exists

    queue->index = frameIndex;
    *(queue->frameData + frameIndex) = copyImgOp(refImage); // creating copy image
    // queue->state = *(queue->frameData + frameIndex);
}

Rasteron_Image* getFrameAt(Rasteron_Queue* queue, unsigned short frameIndex){
    assert(frameIndex < queue->frameCount);

    queue->index = frameIndex;
    // queue->state = *(queue->frameData + frameIndex);
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
    ImageSize menuSize = getUI_ImageSize(size);
    Rasteron_Queue* menuQueue = (Rasteron_Queue*)alloc_queue("toggle", getUI_ImageSize(size), 2);

    Rasteron_Image* bgImg = solidImgOp(menuSize, MENU_BG_COLOR);

    ImageSize buttonSize = (ImageSize){ (unsigned)(menuSize.height * 0.9), (unsigned)(menuSize.width * 0.9) };
    Rasteron_Image* fgImg1 = solidImgOp(buttonSize, MENU_FG_COLOR); // released state
    Rasteron_Image* releasedImg = insertImgOp(fgImg1, bgImg); // TODO: Add character
    Rasteron_Image* fgImg2 = solidImgOp(buttonSize, MENU_BG_COLOR); // pressed state
    Rasteron_Image* pressedImg = insertImgOp(fgImg2, bgImg); // TODO: Add character 

    addFrameAt(menuQueue, releasedImg, 0);
    addFrameAt(menuQueue, pressedImg, 0);

    dealloc_image(bgImg);
    dealloc_image(fgImg1); dealloc_image(releasedImg);
    dealloc_image(fgImg2); dealloc_image(pressedImg);

    return menuQueue;
}

Rasteron_Queue* loadUI_toggleButton(enum MENU_Size size){
    return (Rasteron_Queue*)alloc_queue("toggle", getUI_ImageSize(size), 2);
}

Rasteron_Queue* loadUI_dial(enum MENU_Size size, unsigned short steps){
    assert(steps > 1);
    return (Rasteron_Queue*)alloc_queue("dial", getUI_ImageSize(size), steps);
}

Rasteron_Queue* loadUI_slider(enum MENU_Size size, unsigned short levels){
    assert(levels > 1);
    return (Rasteron_Queue*)alloc_queue("slider",  getUI_ImageSize(size), levels);
}

// --------------------------------  Animation Operations  -------------------------------- //

#ifdef RASTERON_ENABLE_ANIM

Rasteron_Queue* loadAnim_frame30(ImageSize size){ return (Rasteron_Queue*)alloc_queue("frame30", size, 30); } 

Rasteron_Queue* loadAnim_frame60(ImageSize size){ return (Rasteron_Queue*)alloc_queue("frame60", size, 60); } 

Rasteron_Queue* loadAnim_frame120(ImageSize size){ return (Rasteron_Queue*)alloc_queue("frame120", size, 120); } 

#endif
