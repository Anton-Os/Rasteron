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
static unsigned _contentPlus = UI_COLOR_PLUS;
static unsigned _contentNeutral = UI_COLOR_NEUTRAL;
static unsigned _contentMinus = UI_COLOR_MINUS;

ImageSize getUI_ImageSize(enum MENU_Size size){
    switch(size){
        case MENU_Tiny: return (ImageSize){ 32, 32 };
        case MENU_Small: return (ImageSize){ 64, 64 }; 
        case MENU_Medium: return (ImageSize){ 96, 96 };
        case MENU_Large: return (ImageSize){ 128, 128 };
        case MENU_XL: return (ImageSize){ 256, 256 };
    }

    return (ImageSize){ 0, 0 };
}

void setUI_colorScheme(unsigned bgColor, unsigned fgColor, unsigned contentColors[3]){
    if(bgColor != NO_COLOR) _background = bgColor;
    if(fgColor != NO_COLOR) _foreground = fgColor;
    if(contentColors[0] != NO_COLOR) _contentPlus = contentColors[0];
    if(contentColors[1] != NO_COLOR) _contentNeutral = contentColors[1];
    if(contentColors[2] != NO_COLOR) _contentMinus = contentColors[2];
}

Rasteron_Queue* loadUI_checkBtn(enum MENU_Size size){
    ImageSize menuSize = getUI_ImageSize(size);

    Rasteron_Queue* menuQueue = (Rasteron_Queue*)alloc_queue("kbutton", menuSize, 2);
    Rasteron_Image* releaseImg = *(menuQueue->frameData + 0);
    Rasteron_Image* pressImg = *(menuQueue->frameData + 1);

    for(unsigned p = 0; p < menuSize.width * menuSize.height; p++){
        double x = (1.0 / (double)menuSize.width) * (p % menuSize.width);
		double y = (1.0 / (double)menuSize.height) * (p / menuSize.width);
        double centerDist = sqrt((fabs(0.5 - x) * fabs(0.5 - x)) + (fabs(0.5 - y) * fabs(0.5 - y)));

        if(x < 0.1 || x > 0.9 || y < 0.1 || y > 0.9){
        // if(x < 0.15 || x > 0.85 || y < 0.15 || y > 0.85){
            *(releaseImg->data + p) = _background;
            *(pressImg->data + p) = _background;
        }
        else {
            *(releaseImg->data + p) = blendColors(_background, _foreground, 0.75);
            *(pressImg->data + p) = _foreground;

            if(((x < y + 0.1F && x > y - 0.1F) || ((1.0 - x) < y + 0.1F && (1.0 - x) > y - 0.1F)) && centerDist < 0.4) 
                *(releaseImg->data + p) = _contentMinus;

            if((1.0 - x + 0.25) < y + 0.1F + 0.1 && (1.0 - x + 0.25) > y - 0.1F + 0.1 && x < 0.85 && y < x + 0.251) // right side checkmark
                *(pressImg->data + p) = _contentPlus; // right side of checkmark
            if(x + 0.5 < y + 0.15F + 0.1 && x + 0.5 > y - 0.05F + 0.1 && x > 0.15 && y < (1.0 - x) + 0.251) 
                *(pressImg->data + p) = _contentPlus; // left side of checkmark
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
            double indicatorPos[2] = { 0.0, 0.0 };
            indicatorPos[0] += 0.5 + sin(t * ((3.141592653 * 2) / turns)) * 0.25; // x calculation
            indicatorPos[1] += 0.5 + -cos(t * ((3.141592653 * 2) / turns)) * 0.25; // y calculation
            double indicatorDist = sqrt((fabs(indicatorPos[0] - x) * fabs(indicatorPos[0] - x)) + (fabs(indicatorPos[1] - y) * fabs(indicatorPos[1] - y)));

            if(indicatorDist < 0.05) *(dialImg->data + p) = _contentNeutral; // draws indicator
            else if(centerDist < 0.35) *(dialImg->data + p) = _foreground; // draws circular dial 
            else *(dialImg->data + p) = _background;
        }
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

            // double sliderX = (0.1 * (0.5 * levels)) + (l * ((1.0 / levels) * (0.5 * levels)));
            double sliderX = (0.1 * (0.5 * levels)) + (l * ((0.8 / (levels - 1)) * (0.5 * levels)));
            double adjustX = x * (0.5 * levels);
            double sliderDist = sqrt(fabs(sliderX - adjustX) * fabs(sliderX - adjustX) + (fabs(0.5 - y) * fabs(0.5 - y)));

            if(x < 0.1 || x > 0.9 || y < 0.4 || y > 0.6) *(sliderImg->data + p) = _background;
            else if(l == 0) *(sliderImg->data + p) = _contentMinus;
            else if(l == levels - 1) *(sliderImg->data + p) = _contentPlus;
            else *(sliderImg->data + p) = _foreground;

            if(sliderDist < 0.1) *(sliderImg->data + p) = _contentNeutral;
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