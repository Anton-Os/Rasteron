#include "Rasteron.h"

#include "Queue.h"

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
        frame->width = (!_invertImage)? size.width : size.height; 
        frame->height = (!_invertImage)? size.height : size.width;
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
	if(frameIndex > queue->frameCount) frameIndex = frameIndex % queue->frameCount; // prevent out-of-bounds
    dealloc_image(*(queue->frameData + frameIndex)); // deleting old image if exists

    queue->index = frameIndex;
    *(queue->frameData + frameIndex) = copyImgOp(refImage); // single line copy
}

Rasteron_Image* getFrameAt(Rasteron_Queue* queue, unsigned short frameIndex){
    if(frameIndex > queue->frameCount) frameIndex = frameIndex % queue->frameCount; // prevent out-of-bounds

    queue->index = frameIndex;
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

static unsigned _color_bk = UI_COLOR_BACKGROUND;
static unsigned _color_fg = UI_COLOR_FOREGROUND;
static unsigned _color_pos = UI_COLOR_ON;
static unsigned _color_def = UI_COLOR_DEFAULT;
static unsigned _color_neg = UI_COLOR_OFF;

static ImageSize getUI_ImageSize(enum MENU_Size size){
    switch(size){
        case MENU_Tiny: return (ImageSize){ 32, 32 };
        case MENU_Small: return (ImageSize){ 64, 64 }; 
        case MENU_Medium: return (ImageSize){ 104, 104 };
        case MENU_Large: return (ImageSize){ 152, 152 };
        case MENU_XL: return (ImageSize){ 202, 202 };
    }

    return (ImageSize){ 0, 0 };
}

void setUI_colorScheme(unsigned bgColor, unsigned fgColor, unsigned contentColors[3]){
    if(bgColor != NO_COLOR) _color_bk = bgColor;
    if(fgColor != NO_COLOR) _color_fg = fgColor;
    if(contentColors[0] != NO_COLOR) _color_pos = contentColors[0];
    if(contentColors[1] != NO_COLOR) _color_def = contentColors[1];
    if(contentColors[2] != NO_COLOR) _color_neg = contentColors[2];
}

Rasteron_Queue* loadUI_iconBtn(enum MENU_Size size, char* name){
    ImageSize menuSize = getUI_ImageSize(size);

    char iconPath[1024];
    genFullFilePath("material-icons-png\\", &iconPath);
    strcat(iconPath, name);
    strcat(iconPath, ".png");
    Rasteron_Image* iconImg = loadImgOp(iconPath);
    Rasteron_Image* sizedIconImg = resizeImgOp((ImageSize){ (unsigned)(menuSize.height * 0.8), (unsigned)(menuSize.width * 0.8) }, iconImg);

    Rasteron_Queue* menuQueue = (Rasteron_Queue*)alloc_queue("icon-btn", menuSize, 3);
    Rasteron_Image* noneImg = *(menuQueue->frameData + MENU_Off);
    Rasteron_Image* onImg = *(menuQueue->frameData + MENU_On);
    Rasteron_Image* offImg = *(menuQueue->frameData + MENU_None);

    unsigned iconPix = 0;
    for(unsigned p = 0; p < menuSize.width * menuSize.height; p++){
        double x = (1.0 / (double)menuSize.width) * (p % menuSize.width);
		double y = (1.0 / (double)menuSize.height) * (p / menuSize.width);
        PixelPoint pixelPoint = { x, y };

        if(x < 0.1 || x > 0.9 || y < 0.1 || y > 0.9){
            *(offImg->data + p) = _color_bk;
            *(onImg->data + p) = _color_bk;
            *(noneImg->data + p) = _color_bk;
        }
        else {
            unsigned iconColor = *(sizedIconImg->data + iconPix);
            
            *(offImg->data + p) = (iconColor != NO_COLOR)? _color_def : blendColors(_color_bk, _color_fg, 0.75);
            *(onImg->data + p) = (iconColor != NO_COLOR)? _color_pos : _color_fg;
            *(noneImg->data + p) = (iconColor != NO_COLOR)? _color_neg : _color_fg;

            iconPix++;
        }
    }

    dealloc_image(iconImg);
    dealloc_image(sizedIconImg);

    // fclose(iconsFile);
    return menuQueue;
}

Rasteron_Queue* loadUI_checkBtn(enum MENU_Size size){
    ImageSize menuSize = getUI_ImageSize(size);

    Rasteron_Queue* menuQueue = (Rasteron_Queue*)alloc_queue("check-btn", menuSize, 3);
    Rasteron_Image* checkNoImg = *(menuQueue->frameData + MENU_None);
    Rasteron_Image* checkOnImg = *(menuQueue->frameData + MENU_On);
    Rasteron_Image* checkOffImg = *(menuQueue->frameData + MENU_Off);

    for(unsigned p = 0; p < menuSize.width * menuSize.height; p++){
        double x = (1.0 / (double)menuSize.width) * (p % menuSize.width);
		double y = (1.0 / (double)menuSize.height) * (p / menuSize.width);
        double centerDist = sqrt((fabs(0.5 - x) * fabs(0.5 - x)) + (fabs(0.5 - y) * fabs(0.5 - y)));

        if(x < 0.1 || x > 0.9 || y < 0.1 || y > 0.9){
        // if(x < 0.15 || x > 0.85 || y < 0.15 || y > 0.85){
            *(checkNoImg->data + p) = _color_bk;
            *(checkOnImg->data + p) = _color_bk;
            *(checkOffImg->data + p) = _color_bk;
        }
        else {
            *(checkNoImg->data + p) = blendColors(_color_bk, _color_fg, 0.75);
            *(checkOnImg->data + p) = _color_fg;
            *(checkOffImg->data + p) = _color_fg;

            if(((x < y + 0.1F && x > y - 0.1F) || ((1.0 - x) < y + 0.1F && (1.0 - x) > y - 0.1F)) && centerDist < 0.4) 
                *(checkOffImg->data + p) = _color_neg;
            if((1.0 - x + 0.25) < y + 0.1F + 0.1 && (1.0 - x + 0.25) > y - 0.1F + 0.1 && x < 0.85 && y < x + 0.251) // right side checkmark
                *(checkOnImg->data + p) = _color_pos; // right side of checkmark
            if(x + 0.5 < y + 0.15F + 0.1 && x + 0.5 > y - 0.05F + 0.1 && x > 0.15 && y < (1.0 - x) + 0.251) 
                *(checkOnImg->data + p) = _color_pos; // left side of checkmark
        }
    }

    return menuQueue;
}

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
            double indicX = 0.5 + sin(t * ((3.141592653 * 2) / turns)) * 0.25; // x calculation
            double indicY = 0.5 + -cos(t * ((3.141592653 * 2) / turns)) * 0.25; // y calculation
            double indicatorDist = sqrt((fabs(indicX - x) * fabs(indicX - x)) + (fabs(indicY - y) * fabs(indicY - y)));

            if(indicatorDist < 0.05) *(dialImg->data + p) = (t == 0)? _color_pos : (turns % 2 == 0 && t == turns / 2)? _color_neg : _color_def;
            else if(centerDist < 0.35) *(dialImg->data + p) = _color_fg; // draws circular dial 
            else *(dialImg->data + p) = _color_bk;

            /* for(unsigned i = 0; i < turns; i++){
                double perimX = 0.5 + sin(i * ((3.141592653 * 2) / turns)) * 0.425; 
                double perimY = 0.5 + -cos(i * ((3.141592653 * 2) / turns)) * 0.425;
                double distance = sqrt((fabs(perimX - x) * fabs(perimX - x)) + (fabs(perimY - y) * fabs(perimY - y)));
                if(distance < 0.035) *(dialImg->data + p) = _color_fg;
            } */
        }
    }

    return menuQueue;
}

Rasteron_Queue* loadUI_slider(enum MENU_Size size, unsigned short levels){
    assert(levels > 1);

    ImageSize menuSize = getUI_ImageSize(size);
    menuSize.width += (menuSize.width / 2) * (levels - 2); // scaling slider
    Rasteron_Queue* menuQueue = (Rasteron_Queue*)alloc_queue("slider", menuSize, levels);

    for(unsigned l = 0; l < levels; l++){
        Rasteron_Image* sliderImg = *(menuQueue->frameData + l);
        for(unsigned p = 0; p < menuSize.width * menuSize.height; p++){
            double x = (!_invertImage)? ((1.0 / (double)menuSize.width) * (p % menuSize.width)) : ((1.0 / (double)menuSize.height) * (p % menuSize.height));
            double y = (!_invertImage)? ((1.0 / (double)menuSize.height) * (p / menuSize.width)) : ((1.0 / (double)menuSize.width) * (p / menuSize.height));

            double sliderX = 0.15 + (((double)0.7 / ((double)levels - 1)) * l); // position along slider
 
            if(x < 0.1 || x > 0.9 || y < 0.4 || y > 0.6) *(sliderImg->data + p) = _color_bk;
            else if(sliderX > x - 0.05 && sliderX < x + 0.05) 
                *(sliderImg->data + p) = (l == levels - 1)? _color_pos : (l == 0)? _color_neg : _color_def;
            else *(sliderImg->data + p) = _color_fg; /* {
                for(unsigned s = 0; s < levels; s++){
                    double sliderX = 0.15 + (((double)0.7 / ((double)levels - 1)) * s);
                    if(sliderX > x - 0.0035 && sliderX < x + 0.0035)
                        *(sliderImg->data + p) = _color_bk;
                    else if(*(sliderImg->data + p) != _color_bk) 
                        *(sliderImg->data + p) = _color_fg;
                }
            } */
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