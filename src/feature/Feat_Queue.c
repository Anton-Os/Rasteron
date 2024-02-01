#include "Rasteron.h"

#include "Feat_Queue.h"

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
static unsigned _color_pos = UI_COLOR_PLUS;
static unsigned _color_def = UI_COLOR_DEFAULT;
static unsigned _color_neg = UI_COLOR_MINUS;

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
    /* FILE* iconsFile;
#ifdef _WIN32
    if(fopen_s(&iconsFile, MATERIAL_ICONS_FILE, "r")) return NO_QUEUE; // error
#else
    iconsFile = fopen(MATERIAL_ICONS_FILE, "r");
    if(iconsFile == NULL) return NO_QUEUE; // error
#endif
    int c;
    while((c = getc(iconsFile)) != EOF) putchar(c); */ // TODO: Implement lookup
    
    ImageSize menuSize = getUI_ImageSize(size);

    char iconPath[1024];
    genFullFilePath("material-icons-png\\", &iconPath);
    strcat(iconPath, name);
    strcat(iconPath, ".png");
    Rasteron_Image* iconImg = loadImgOp(iconPath);
    Rasteron_Image* sizedIconImg = resizeImgOp((ImageSize){ (unsigned)(menuSize.height * 0.8), (unsigned)(menuSize.width * 0.8) }, iconImg);

    Rasteron_Queue* menuQueue = (Rasteron_Queue*)alloc_queue("icon-btn", menuSize, 3);
    Rasteron_Image* defaultImg = *(menuQueue->frameData + UI_STATE_DEFAULT);
    Rasteron_Image* plusImg = *(menuQueue->frameData + UI_STATE_PLUS);
    Rasteron_Image* minusImg = *(menuQueue->frameData + UI_STATE_MINUS);

    unsigned iconPix = 0;
    for(unsigned p = 0; p < menuSize.width * menuSize.height; p++){
        double x = (1.0 / (double)menuSize.width) * (p % menuSize.width);
		double y = (1.0 / (double)menuSize.height) * (p / menuSize.width);
        PixelPoint pixelPoint = { x, y };

        if(x < 0.1 || x > 0.9 || y < 0.1 || y > 0.9){
        // if(x < 0.15 || x > 0.85 || y < 0.15 || y > 0.85){
            *(defaultImg->data + p) = _color_bk;
            *(plusImg->data + p) = _color_bk;
            *(minusImg->data + p) = _color_bk;
        }
        else {
            unsigned iconColor = *(sizedIconImg->data + iconPix);
            
            *(defaultImg->data + p) = (iconColor != NO_COLOR)? _color_def : blendColors(_color_bk, _color_fg, 0.75);
            *(plusImg->data + p) = (iconColor != NO_COLOR)? _color_pos : _color_fg;
            *(minusImg->data + p) = (iconColor != NO_COLOR)? _color_neg : _color_fg;

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
    Rasteron_Image* uncheckImg = *(menuQueue->frameData + UI_STATE_DEFAULT);
    Rasteron_Image* checkImg = *(menuQueue->frameData + UI_STATE_PLUS);
    Rasteron_Image* excheckImg = *(menuQueue->frameData + UI_STATE_MINUS);

    for(unsigned p = 0; p < menuSize.width * menuSize.height; p++){
        double x = (1.0 / (double)menuSize.width) * (p % menuSize.width);
		double y = (1.0 / (double)menuSize.height) * (p / menuSize.width);
        double centerDist = sqrt((fabs(0.5 - x) * fabs(0.5 - x)) + (fabs(0.5 - y) * fabs(0.5 - y)));

        if(x < 0.1 || x > 0.9 || y < 0.1 || y > 0.9){
        // if(x < 0.15 || x > 0.85 || y < 0.15 || y > 0.85){
            *(uncheckImg->data + p) = _color_bk;
            *(checkImg->data + p) = _color_bk;
            *(excheckImg->data + p) = _color_bk;
        }
        else {
            *(uncheckImg->data + p) = blendColors(_color_bk, _color_fg, 0.75);
            *(checkImg->data + p) = _color_fg;
            *(excheckImg->data + p) = _color_fg;

            if(((x < y + 0.1F && x > y - 0.1F) || ((1.0 - x) < y + 0.1F && (1.0 - x) > y - 0.1F)) && centerDist < 0.4) 
                *(excheckImg->data + p) = _color_neg;

            if((1.0 - x + 0.25) < y + 0.1F + 0.1 && (1.0 - x + 0.25) > y - 0.1F + 0.1 && x < 0.85 && y < x + 0.251) // right side checkmark
                *(checkImg->data + p) = _color_pos; // right side of checkmark
            if(x + 0.5 < y + 0.15F + 0.1 && x + 0.5 > y - 0.05F + 0.1 && x > 0.15 && y < (1.0 - x) + 0.251) 
                *(checkImg->data + p) = _color_pos; // left side of checkmark
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
            double indicatorPos[2] = { 0.0, 0.0 };
            indicatorPos[0] += 0.5 + sin(t * ((3.141592653 * 2) / turns)) * 0.25; // x calculation
            indicatorPos[1] += 0.5 + -cos(t * ((3.141592653 * 2) / turns)) * 0.25; // y calculation
            double indicatorDist = sqrt((fabs(indicatorPos[0] - x) * fabs(indicatorPos[0] - x)) + (fabs(indicatorPos[1] - y) * fabs(indicatorPos[1] - y)));

            if(indicatorDist < 0.05) *(dialImg->data + p) = (t == 0)? _color_pos : (turns % 2 == 0 && t == turns / 2)? _color_neg : _color_def;
            else if(centerDist < 0.35) *(dialImg->data + p) = _color_fg; // draws circular dial 
            else *(dialImg->data + p) = _color_bk;
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

            double sliderX = (0.1 * (0.5 * levels)) + (l * ((0.8 / (levels - 1)) * (0.5 * levels)));
            double adjustX = x * (0.5 * levels);
            double sliderDist = sqrt(fabs(sliderX - adjustX) * fabs(sliderX - adjustX) + (fabs(0.5 - y) * fabs(0.5 - y)));

            if(x < 0.1 || x > 0.9 || y < 0.4 || y > 0.6) *(sliderImg->data + p) = _color_bk;
            else *(sliderImg->data + p) = _color_fg;

            if(sliderDist < 0.09) 
                *(sliderImg->data + p) = (l == levels - 1)? _color_pos : (l == 0)? _color_neg : _color_def;
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