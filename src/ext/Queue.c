#include "Rasteron.h"

#include "Queue.h"

static char frameName[1024];

// --------------------------------  Queue Operations  -------------------------------- //

Rasteron_Queue* internal_alloc_queue(const char* prefix, ImageSize size, unsigned frameCount){
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
    RASTERON_DEALLOC(*(queue->frameData + frameIndex)); // deleting old image if exists

    queue->index = frameIndex;
    *(queue->frameData + frameIndex) = copyImgOp(refImage); // single line copy
}

Rasteron_Image* getFrameAt(Rasteron_Queue* queue, unsigned short frameIndex){
    if(frameIndex > queue->frameCount) frameIndex = frameIndex % queue->frameCount; // prevent out-of-bounds

    queue->index = frameIndex;
    return *(queue->frameData + frameIndex);
}

void internal_dealloc_queue(Rasteron_Queue* queue){
    if(queue != NULL){
        for(unsigned f = 0; f < queue->frameCount; f++)
            RASTERON_DEALLOC(*(queue->frameData + f));
        free(queue->frameData);
        free(queue);
        queue = NULL;
    }
}

// --------------------------------  UI Operations  -------------------------------- //

static unsigned internal_ui_bk = UI_COLOR_BACKGROUND;
static unsigned internal_ui_fg = UI_COLOR_FOREGROUND;
static unsigned internal_ui_pos = UI_COLOR_ON;
static unsigned internal_ui_def = UI_COLOR_DEFAULT;
static unsigned internal_ui_neg = UI_COLOR_OFF;

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
    if(bgColor != NO_COLOR) internal_ui_bk = bgColor;
    if(fgColor != NO_COLOR) internal_ui_fg = fgColor;
    if(contentColors[0] != NO_COLOR) internal_ui_pos = contentColors[0];
    if(contentColors[1] != NO_COLOR) internal_ui_def = contentColors[1];
    if(contentColors[2] != NO_COLOR) internal_ui_neg = contentColors[2];
}

Rasteron_Queue* loadUI_iconBtn(enum MENU_Size size, char* name){
    ImageSize menuSize = getUI_ImageSize(size);

    char iconPath[1024];
    genFullFilePath("material-icons-png\\", &iconPath);
    strcat(iconPath, name);
    strcat(iconPath, ".png");
    Rasteron_Image* iconImg = loadImgOp(iconPath);
    Rasteron_Image* sizedIconImg = resizeImgOp((ImageSize){ (unsigned)(menuSize.height * 0.8), (unsigned)(menuSize.width * 0.8) }, iconImg);
    Rasteron_Image* finalIconImg = copyImgOp(sizedIconImg); // antialiasImgOp(sizedIconImg, 1);

    Rasteron_Image* bkImg = noiseImgOp_white((ImageSize){ menuSize.height, menuSize.width }, internal_ui_bk, internal_ui_bk + 0xFF111111);
    Rasteron_Image* fgImg = gradientImgOp((ImageSize){ menuSize.height, menuSize.width }, SIDE_Bottom, internal_ui_fg, internal_ui_fg + 0x333333);

    Rasteron_Queue* menuQueue = (Rasteron_Queue*)RASTERON_QUEUE_ALLOC("icon-btn", menuSize, 3);
    Rasteron_Image* noneImg = *(menuQueue->frameData + MENU_Off);
    Rasteron_Image* onImg = *(menuQueue->frameData + MENU_On);
    Rasteron_Image* offImg = *(menuQueue->frameData + MENU_None);

    unsigned iconPix = 0;
    for(unsigned p = 0; p < menuSize.width * menuSize.height; p++){
        double x = (1.0 / (double)menuSize.width) * (p % menuSize.width);
		double y = (1.0 / (double)menuSize.height) * (p / menuSize.width);
        PixelPoint pixelPoint = { x, y };

        if(x < 0.1 || x > 0.9 || y < 0.1 || y > 0.9){
            *(offImg->data + p) = *(bkImg->data + p);
            *(onImg->data + p) = *(bkImg->data + p);
            *(noneImg->data + p) = *(bkImg->data + p);
        }
        else {
            unsigned iconColor = *(finalIconImg->data + iconPix);
            
            *(offImg->data + p) = (iconColor != NO_COLOR)? internal_ui_def : *(fgImg->data + p);
            *(onImg->data + p) = (iconColor != NO_COLOR)? internal_ui_pos : *(fgImg->data + p);
            *(noneImg->data + p) = (iconColor != NO_COLOR)?  internal_ui_neg : *(fgImg->data + p);

            iconPix++;
        }
    }

    RASTERON_DEALLOC(bkImg); 
    RASTERON_DEALLOC(fgImg);

    RASTERON_DEALLOC(iconImg);
    RASTERON_DEALLOC(sizedIconImg);
    RASTERON_DEALLOC(finalIconImg);

    // fclose(iconsFile);
    return menuQueue;
}

Rasteron_Queue* loadUI_checkBtn(enum MENU_Size size){
    ImageSize menuSize = getUI_ImageSize(size);

    Rasteron_Image* bkImg = noiseImgOp_white((ImageSize){ menuSize.height, menuSize.width }, internal_ui_bk, internal_ui_bk + 0xFF111111);
    Rasteron_Image* fgImg = gradientImgOp((ImageSize){ menuSize.height, menuSize.width }, SIDE_Bottom, internal_ui_fg, internal_ui_fg + 0x333333);

    Rasteron_Queue* menuQueue = (Rasteron_Queue*)RASTERON_QUEUE_ALLOC("check-btn", menuSize, 3);
    Rasteron_Image* checkNoImg = *(menuQueue->frameData + MENU_None);
    Rasteron_Image* checkOnImg = *(menuQueue->frameData + MENU_On);
    Rasteron_Image* checkOffImg = *(menuQueue->frameData + MENU_Off);

    for(unsigned p = 0; p < menuSize.width * menuSize.height; p++){
        double x = (1.0 / (double)menuSize.width) * (p % menuSize.width);
		double y = (1.0 / (double)menuSize.height) * (p / menuSize.width);
        double centerDist = sqrt((fabs(0.5 - x) * fabs(0.5 - x)) + (fabs(0.5 - y) * fabs(0.5 - y)));

        if(x < 0.1 || x > 0.9 || y < 0.1 || y > 0.9){
        // if(x < 0.15 || x > 0.85 || y < 0.15 || y > 0.85){
            *(checkNoImg->data + p) = *(bkImg->data + p);
            *(checkOnImg->data + p) = *(bkImg->data + p);
            *(checkOffImg->data + p) = *(bkImg->data + p);
        }
        else {
            *(checkNoImg->data + p) = *(fgImg->data + p);
            *(checkOnImg->data + p) = *(fgImg->data + p);
            *(checkOffImg->data + p) = *(fgImg->data + p);

            if(((x < y + 0.1F && x > y - 0.1F) || ((1.0 - x) < y + 0.1F && (1.0 - x) > y - 0.1F)) && centerDist < 0.4) 
                *(checkOffImg->data + p) = internal_ui_neg;
            if((1.0 - x + 0.25) < y + 0.1F + 0.1 && (1.0 - x + 0.25) > y - 0.1F + 0.1 && x < 0.85 && y < x + 0.251) // right side checkmark
                *(checkOnImg->data + p) = internal_ui_pos;
            if(x + 0.5 < y + 0.15F + 0.1 && x + 0.5 > y - 0.05F + 0.1 && x > 0.15 && y < (1.0 - x) + 0.251) 
                *(checkOnImg->data + p) = internal_ui_pos;
        }
    }

    RASTERON_DEALLOC(bkImg); 
    RASTERON_DEALLOC(fgImg);

    return menuQueue;
}

Rasteron_Queue* loadUI_dial(enum MENU_Size size, unsigned short turns){
    assert(turns > 1);

    ImageSize menuSize = getUI_ImageSize(size);
    unsigned shortSide = (menuSize.height > menuSize.width)? menuSize.width : menuSize.height;
    ImageSize dialSize = (ImageSize){ (unsigned)(shortSide * 0.9), (unsigned)(shortSide * 0.9) };

    Rasteron_Image* bkImg = noiseImgOp_white((ImageSize){ menuSize.height, menuSize.width }, internal_ui_bk, internal_ui_bk + 0xFF111111);
    Rasteron_Image* fgImg = gradientImgOp((ImageSize){ menuSize.height, menuSize.width }, SIDE_Radial, internal_ui_fg + 0x333333,internal_ui_fg);

    Rasteron_Queue* menuQueue = (Rasteron_Queue*)RASTERON_QUEUE_ALLOC("dial", menuSize, turns);

    for(unsigned t = 0; t < turns; t++){
        Rasteron_Image* dialImg = *(menuQueue->frameData + t);
        for(unsigned p = 0; p < menuSize.width * menuSize.height; p++){
            double x = (1.0 / (double)menuSize.width) * (unsigned)(p % menuSize.width);
		    double y = (1.0 / (double)menuSize.width) * (double)(p / menuSize.width);

            double centerDist = sqrt((fabs(0.5 - x) * fabs(0.5 - x)) + (fabs(0.5 - y) * fabs(0.5 - y)));
            double indicX = 0.5 + sin(t * ((3.141592653 * 2) / turns)) * 0.25; // x calculation
            double indicY = 0.5 + -cos(t * ((3.141592653 * 2) / turns)) * 0.25; // y calculation
            double indicatorDist = sqrt((fabs(indicX - x) * fabs(indicX - x)) + (fabs(indicY - y) * fabs(indicY - y)));

            if(indicatorDist < 0.05) *(dialImg->data + p) = (t == 0)? internal_ui_pos : (turns % 2 == 0 && t == turns / 2)? internal_ui_neg : internal_ui_def;
            else if(centerDist < 0.35) *(dialImg->data + p) = *(fgImg->data + p); // draws circular dial 
            else *(dialImg->data + p) = *(bkImg->data + p);

            /* for(unsigned i = 0; i < turns; i++){
                double perimX = 0.5 + sin(i * ((3.141592653 * 2) / turns)) * 0.425; 
                double perimY = 0.5 + -cos(i * ((3.141592653 * 2) / turns)) * 0.425;
                double distance = sqrt((fabs(perimX - x) * fabs(perimX - x)) + (fabs(perimY - y) * fabs(perimY - y)));
                if(distance < 0.035) *(dialImg->data + p) = internal_ui_fg;
            } */
        }
    }

    RASTERON_DEALLOC(bkImg); 
    RASTERON_DEALLOC(fgImg);

    return menuQueue;
}

Rasteron_Queue* loadUI_slider(enum MENU_Size size, unsigned short levels){
    assert(levels > 1);

    ImageSize menuSize = getUI_ImageSize(size);
    menuSize.width += (menuSize.width / 2) * (levels - 2); // scaling slider
    Rasteron_Queue* menuQueue = (Rasteron_Queue*)internal_alloc_queue("slider", menuSize, levels);

    Rasteron_Image* bkImg = noiseImgOp_white((ImageSize){ menuSize.height, menuSize.width }, internal_ui_bk, internal_ui_bk + 0xFF111111);
    Rasteron_Image* fgImg = solidImgOp((ImageSize){ menuSize.height, menuSize.width }, internal_ui_fg);
    // Rasteron_Image* fgImg = gradientImgOp((ImageSize){ menuSize.height, menuSize.width }, SIDE_Radial, internal_ui_fg, internal_ui_fg + 0x333333);

    for(unsigned l = 0; l < levels; l++){
        Rasteron_Image* sliderImg = *(menuQueue->frameData + l);
        for(unsigned p = 0; p < menuSize.width * menuSize.height; p++){
            double x = (!_invertImage)? ((1.0 / (double)menuSize.width) * (p % menuSize.width)) : ((1.0 / (double)menuSize.height) * (p % menuSize.height));
            double y = (!_invertImage)? ((1.0 / (double)menuSize.height) * (p / menuSize.width)) : ((1.0 / (double)menuSize.width) * (p / menuSize.height));

            double sliderX = 0.15 + (((double)0.7 / ((double)levels - 1)) * l); // position along slider
 
            if(x < 0.1 || x > 0.9 || y < 0.4 || y > 0.6) *(sliderImg->data + p) = *(bkImg->data + p);
            else if(sliderX > x - 0.05 && sliderX < x + 0.05) 
                *(sliderImg->data + p) = (l == levels - 1)? internal_ui_pos : (l == 0)? internal_ui_neg : internal_ui_def;
            else *(sliderImg->data + p) = *(fgImg->data + p); /* {
                for(unsigned s = 0; s < levels; s++){
                    double sliderX = 0.15 + (((double)0.7 / ((double)levels - 1)) * s);
                    if(sliderX > x - 0.0035 && sliderX < x + 0.0035)
                        *(sliderImg->data + p) = internal_ui_bk;
                    else if(*(sliderImg->data + p) != internal_ui_bk) 
                        *(sliderImg->data + p) = internal_ui_fg;
                }
            } */
        }
    }

    RASTERON_DEALLOC(bkImg); 
    RASTERON_DEALLOC(fgImg);

    return menuQueue;
}

// --------------------------------  Animation Operations  -------------------------------- //

#ifdef RASTERON_ENABLE_ANIM

Rasteron_Queue* loadAnim_frame30(ImageSize size){ return (Rasteron_Queue*)internal_alloc_queue("frame30", size, 30); } 

Rasteron_Queue* loadAnim_frame60(ImageSize size){ return (Rasteron_Queue*)internal_alloc_queue("frame60", size, 60); } 

Rasteron_Queue* loadAnim_frame120(ImageSize size){ return (Rasteron_Queue*)internal_alloc_queue("frame120", size, 120); } 

#endif