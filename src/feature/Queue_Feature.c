#include "Rasteron.h"

#include "Queue_Feature.h"

static char frameName[1024];

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

        sprintf(frameName, "frame%d", f + 1);
        frame->name = frameName;
        frame->width = size.width; frame->height = size.height;
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
    // queue->state = *(queue->frameData + frameIndex);
    *(queue->frameData + frameIndex) = copyImgOp(refImage); // single line copy
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

static unsigned _background = UI_COLOR_BACKGROUND;
static unsigned _foreground = UI_COLOR_FOREGROUND;
static unsigned _content = UI_COLOR_CONTENT;

ImageSize getUI_ImageSize(enum MENU_Size size){
    switch(size){
        case MENU_Tiny: return (ImageSize){ 16, 16 };
        case MENU_Small: return (ImageSize){ 32, 32 }; 
        case MENU_Medium: return (ImageSize){ 64, 64 };
        case MENU_Large: return (ImageSize){ 128, 128 };
        case MENU_XL: return (ImageSize){ 256, 256 };
    }

    return (ImageSize){ 0, 0 };
}

void setUI_colorScheme(unsigned bgColor, unsigned fgColor, unsigned cnColor){
    if(bgColor != NO_COLOR) _background = bgColor;
    if(fgColor != NO_COLOR) _foreground = fgColor;
    if(cnColor != NO_COLOR) _content = cnColor;
}

Rasteron_Queue* loadUI_checkBtn(enum MENU_Size size){
    ImageSize menuSize = getUI_ImageSize(size);

    Rasteron_Queue* menuQueue = (Rasteron_Queue*)alloc_queue("kbutton", menuSize, 2);
    Rasteron_Image* releaseImg = *(menuQueue->frameData + 0);
    Rasteron_Image* pressImg = *(menuQueue->frameData + 1);

    for(unsigned p = 0; p < menuSize.width * menuSize.height; p++){
        double x = (1.0 / (double)menuSize.width) * (p % menuSize.width);
		double y = (1.0 / (double)menuSize.height) * (p / menuSize.width);

        if(x < 0.1 || x > 0.9 || y < 0.1 || y > 0.9){
            *(releaseImg->data + p) = _background;
            *(pressImg->data + p) = _background;
        }
        else {
            *(releaseImg->data + p) = blendColors(_background, _foreground, 0.5);
            *(pressImg->data + p) = _foreground;

            // TODO: Create red X for release image
            // TODO: Create green check for press image
        }
    }

    return menuQueue;
}

/* Rasteron_Queue* loadUI_toggleButton(enum MENU_Size size){
    return (Rasteron_Queue*)alloc_queue("toggle", getUI_ImageSize(size), 2);
} */

Rasteron_Queue* loadUI_dial(enum MENU_Size size, unsigned short turns){
    assert(turns > 1);

    ImageSize menuSize = getUI_ImageSize(size);
    unsigned shortSide = (menuSize.height > menuSize.width)? menuSize.width : menuSize.height;
    ImageSize dialSize = (ImageSize){ (unsigned)(shortSide * 0.9), (unsigned)(shortSide * 0.9) };

    Rasteron_Queue* menuQueue = (Rasteron_Queue*)alloc_queue("dial", menuSize, turns);

    for(unsigned t = 0; t < turns; t++){
        Rasteron_Image* dialImg = *(menuQueue->frameData + t);
        for(unsigned p = 0; p < menuSize.width * menuSize.height; p++){
            double x = (1.0 / (double)menuSize.width) * (unsigned)(p % menuSize.width);
		    double y = (1.0 / (double)menuSize.width) * (double)(p / menuSize.width);

            double centerDist = sqrt((fabs(0.5 - x) * fabs(0.5 - x)) + (fabs(0.5 - y) * fabs(0.5 - y)));
            // double centerDist = pixelDistance(p, ((menuSize.width * menuSize.height) / 2) + (menuSize.width / 2), menuSize.width) * (1.0 / (double)menuSize.width);

            if(centerDist < 0.35) *(dialImg->data + p) = _foreground; // draws circular dial 
            else *(dialImg->data + p) = _background;
        }

        // TODO: Draw dial indicator for turn amount
    }

    return menuQueue;
}

Rasteron_Queue* loadUI_slider(enum MENU_Size size, unsigned short levels){
    assert(levels > 1);

    ImageSize menuSize = getUI_ImageSize(size);
    menuSize.width += (menuSize.width / 2) * (levels - 2); // slider needs to scale to levels

    Rasteron_Queue* menuQueue = (Rasteron_Queue*)alloc_queue("slider", menuSize, levels);

    for(unsigned l = 0; l < levels; l++){
        Rasteron_Image* sliderImg = *(menuQueue->frameData + l);
        for(unsigned p = 0; p < menuSize.width * menuSize.height; p++){
            double x = (1.0 / (double)menuSize.width) * (p % menuSize.width);
            double y = (1.0 / (double)menuSize.height) * (p / menuSize.width);

            if(x < 0.1 || x > 0.9 || y < 0.4 || y > 0.6) *(sliderImg->data + p) = _background;
            else if(l == 0) *(sliderImg->data + p) = _foreground;
            else if(l == levels - 1) *(sliderImg->data + p) = _content;
            else {} // TODO: Compute in between levels
        }
    }

    return menuQueue;
}

// --------------------------------  Animation Operations  -------------------------------- //

#ifdef RASTERON_ENABLE_ANIM

Rasteron_Queue* loadAnim_frame30(ImageSize size){ return (Rasteron_Queue*)alloc_queue("frame30", size, 30); } 

Rasteron_Queue* loadAnim_frame60(ImageSize size){ return (Rasteron_Queue*)alloc_queue("frame60", size, 60); } 

Rasteron_Queue* loadAnim_frame120(ImageSize size){ return (Rasteron_Queue*)alloc_queue("frame120", size, 120); } 

#endif
