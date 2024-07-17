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

void queue_addImg(Rasteron_Queue* queue, const Rasteron_Image *const refImage, unsigned short frameIndex){
	if(frameIndex > queue->frameCount) frameIndex = frameIndex % queue->frameCount; // prevent out-of-bounds
    RASTERON_DEALLOC(*(queue->frameData + frameIndex)); // deleting old image if exists

    queue->index = frameIndex;
    *(queue->frameData + frameIndex) = copyImgOp(refImage); // single line copy
}

Rasteron_Image* queue_getImg(Rasteron_Queue* queue, unsigned short frameIndex){
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
        case MENU_Tiny: return (ImageSize){ 64, 64 };
        case MENU_Small: return (ImageSize){ 82, 82 }; 
        case MENU_Medium: return (ImageSize){ 104, 104 };
        case MENU_Large: return (ImageSize){ 124, 124 };
        case MENU_XL: return (ImageSize){ 152, 152 };
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

    Rasteron_Image* refImages[5] = {
        noiseImgOp_white((ImageSize){ menuSize.height, menuSize.width }, internal_ui_bk, internal_ui_bk + 0xFF111111),
        solidImgOp((ImageSize){ menuSize.height, menuSize.width }, internal_ui_fg),
        solidImgOp((ImageSize){ menuSize.height, menuSize.width }, internal_ui_pos),
        solidImgOp((ImageSize){ menuSize.height, menuSize.width }, internal_ui_neg),
        solidImgOp((ImageSize){ menuSize.height, menuSize.width }, internal_ui_def),
    };

    Rasteron_Queue* menuQueue = (Rasteron_Queue*)RASTERON_QUEUE_ALLOC("button", menuSize, 4);
    Rasteron_Image* queueImages[4] = {
        *(menuQueue->frameData + MENU_None),
        *(menuQueue->frameData + MENU_On),
        *(menuQueue->frameData + MENU_Off),
        *(menuQueue->frameData + MENU_Pre)
    };

    unsigned iconPix = 0;
    for(unsigned p = 0; p < menuSize.width * menuSize.height; p++){
        double x = (1.0 / (double)menuSize.width) * (p % menuSize.width);
		double y = (1.0 / (double)menuSize.height) * (p / menuSize.width);
        PixelPoint pixelPoint = { x, y };

        if(x < 0.1 || x > 0.9 || y < 0.1 || y > 0.9){
            for(unsigned q = 0; q < 4; q++) *(queueImages[q]->data + p) = *(refImages[0]->data + p);

            if(x < 0.035 || x > 0.965 || y < 0.035 || y > 0.965){
                *(queueImages[1]->data + p) = *(refImages[2]->data + p);
                *(queueImages[2]->data + p) = *(refImages[3]->data + p);
            }
        }
        else {
            unsigned iconColor = *(finalIconImg->data + iconPix);
            
            *(queueImages[0]->data + p) = (iconColor != NO_COLOR)? *(refImages[4]->data + p) : *(refImages[1]->data + p);
            *(queueImages[1]->data + p) = (iconColor != NO_COLOR)? *(refImages[4]->data + p) : *(refImages[1]->data + p);
            *(queueImages[2]->data + p) = (iconColor != NO_COLOR)? *(refImages[2]->data + p) : *(refImages[4]->data + p);
            *(queueImages[3]->data + p) = (iconColor != NO_COLOR)? *(refImages[1]->data + p) : *(refImages[4]->data + p);

            iconPix++;
        }
    }

    for(unsigned r = 0; r < 5; r++) RASTERON_DEALLOC(refImages[r]);

    RASTERON_DEALLOC(iconImg);
    RASTERON_DEALLOC(sizedIconImg);
    RASTERON_DEALLOC(finalIconImg);

    // fclose(iconsFile);
    return menuQueue;
}

Rasteron_Queue* loadUI_checkBtn(enum MENU_Size size){
    ImageSize menuSize = getUI_ImageSize(size);

    Rasteron_Image* refImages[5] = {
        noiseImgOp_white((ImageSize){ menuSize.height, menuSize.width }, internal_ui_bk, internal_ui_bk + 0xFF111111),
        solidImgOp((ImageSize){ menuSize.height, menuSize.width }, internal_ui_fg),
        solidImgOp((ImageSize){ menuSize.height, menuSize.width }, internal_ui_pos),
        solidImgOp((ImageSize){ menuSize.height, menuSize.width }, internal_ui_neg),
        solidImgOp((ImageSize){ menuSize.height, menuSize.width }, internal_ui_def),
    };

    Rasteron_Queue* menuQueue = (Rasteron_Queue*)RASTERON_QUEUE_ALLOC("checkbox", menuSize, 4);
    Rasteron_Image* queueImages[4] = {
        *(menuQueue->frameData + MENU_None),
        *(menuQueue->frameData + MENU_On),
        *(menuQueue->frameData + MENU_Off),
        *(menuQueue->frameData + MENU_Pre)
    };

    for(unsigned p = 0; p < menuSize.width * menuSize.height; p++){
        double x = (1.0 / (double)menuSize.width) * (p % menuSize.width);
		double y = (1.0 / (double)menuSize.height) * (p / menuSize.width);
        double centerDist = sqrt((fabs(0.5 - x) * fabs(0.5 - x)) + (fabs(0.5 - y) * fabs(0.5 - y)));

        if(x < 0.1 || x > 0.9 || y < 0.1 || y > 0.9){
            for(unsigned q = 0; q < 4; q++) *(queueImages[q]->data + p) = *(refImages[0]->data + p);

            if(x < 0.035 || x > 0.965 || y < 0.035 || y > 0.965){
                *(queueImages[2]->data + p) = *(refImages[3]->data + p);
                *(queueImages[1]->data + p) = *(refImages[2]->data + p);
            }
        }
        else {
            *(queueImages[0]->data + p) = *(refImages[1]->data + p);
            *(queueImages[1]->data + p) = *(refImages[1]->data + p);
            *(queueImages[2]->data + p) = *(refImages[1]->data + p);
            *(queueImages[3]->data + p) = *(refImages[4]->data + p);

            if(((x < y + 0.1F && x > y - 0.1F) || ((1.0 - x) < y + 0.1F && (1.0 - x) > y - 0.1F)) && centerDist < 0.4) 
                *(queueImages[2]->data + p) = *(refImages[3]->data + p);
            if((1.0 - x + 0.25) < y + 0.1F + 0.1 && (1.0 - x + 0.25) > y - 0.1F + 0.1 && x < 0.85 && y < x + 0.251) // right side checkmark
                *(queueImages[1]->data + p) = *(refImages[2]->data + p);
            if(x + 0.5 < y + 0.15F + 0.1 && x + 0.5 > y - 0.05F + 0.1 && x > 0.15 && y < (1.0 - x) + 0.251) // left side of checkmark
                *(queueImages[1]->data + p) = *(refImages[2]->data + p);
        }
    }

    for(unsigned r = 0; r < 5; r++) RASTERON_DEALLOC(refImages[r]);

    return menuQueue;
}

Rasteron_Queue* loadUI_dial(enum MENU_Size size, unsigned short turns){
    assert(turns > 1);

    ImageSize menuSize = getUI_ImageSize(size);
    unsigned shortSide = (menuSize.height > menuSize.width)? menuSize.width : menuSize.height;

    Rasteron_Image* refImages[5] = {
        noiseImgOp_white((ImageSize){ menuSize.height, menuSize.width }, internal_ui_bk, internal_ui_bk + 0xFF111111),
        solidImgOp((ImageSize){ menuSize.height, menuSize.width }, internal_ui_fg),
        solidImgOp((ImageSize){ menuSize.height, menuSize.width }, internal_ui_pos),
        solidImgOp((ImageSize){ menuSize.height, menuSize.width }, internal_ui_neg),
        solidImgOp((ImageSize){ menuSize.height, menuSize.width }, internal_ui_def),
    };

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

            if(indicatorDist < 0.045) *(dialImg->data + p) = *(refImages[4]->data + p);
            else if(centerDist < 0.33) *(dialImg->data + p) = *(refImages[1]->data + p); // draws circular dial 
            else {
                *(dialImg->data + p) = *(refImages[0]->data + p);
                for(unsigned i = 0; i < turns; i++){
                    double perimX = 0.5 + sin(i * ((3.141592653 * 2) / turns)) * 0.4375; 
                    double perimY = 0.5 + -cos(i * ((3.141592653 * 2) / turns)) * 0.4375;
                    double distance = sqrt((fabs(perimX - x) * fabs(perimX - x)) + (fabs(perimY - y) * fabs(perimY - y)));
                    if(distance < 0.0275) *(dialImg->data + p) = (t == i)? *(refImages[2]->data + p) : *(refImages[4]->data + p);
                }
            }
        }
    }

    for(unsigned r = 0; r < 5; r++) RASTERON_DEALLOC(refImages[r]);

    return menuQueue;
}

Rasteron_Queue* loadUI_slider(enum MENU_Size size, unsigned short levels){
    assert(levels > 1);

    ImageSize menuSize = getUI_ImageSize(size);
    menuSize.width += (menuSize.width / 2) * (levels - 2); // scaling slider
    Rasteron_Queue* menuQueue = (Rasteron_Queue*)internal_alloc_queue("slider", menuSize, levels);

    Rasteron_Image* refImages[5] = {
        noiseImgOp_white((ImageSize){ menuSize.height, menuSize.width }, internal_ui_bk, internal_ui_bk + 0xFF111111),
        solidImgOp((ImageSize){ menuSize.height, menuSize.width }, internal_ui_fg),
        solidImgOp((ImageSize){ menuSize.height, menuSize.width }, internal_ui_pos),
        solidImgOp((ImageSize){ menuSize.height, menuSize.width }, internal_ui_neg),
        solidImgOp((ImageSize){ menuSize.height, menuSize.width }, internal_ui_def),
    };

    for(unsigned l = 0; l < levels; l++){
        Rasteron_Image* sliderImg = *(menuQueue->frameData + l);
        for(unsigned p = 0; p < menuSize.width * menuSize.height; p++){
            double x = (!_invertImage)? ((1.0 / (double)menuSize.width) * (p % menuSize.width)) : ((1.0 / (double)menuSize.height) * (p % menuSize.height));
            double y = (!_invertImage)? ((1.0 / (double)menuSize.height) * (p / menuSize.width)) : ((1.0 / (double)menuSize.width) * (p / menuSize.height));

            double sliderX = 0.15 + (((double)0.7 / ((double)levels - 1)) * l); // position along slider
 
            if(x < 0.1 || x > 0.9 || y < 0.4 || y > 0.6) *(sliderImg->data + p) = *(refImages[0]->data + p); // populate background
            else {
                *(sliderImg->data + p) = *(refImages[1]->data + p); // populate slider bar
                for(unsigned s = 0; s < levels; s++){
                    double sliderX = 0.15 + (((double)0.7 / ((double)levels - 1)) * s);
                    if(sliderX > x - 0.01 && sliderX < x + 0.01) *(sliderImg->data + p) = *(refImages[(s == 0)? 3 : (s == levels - 1)? 2 : 4]->data + p); // indicator status based on offset
                }
            }
            if(sliderX > x - 0.015 && sliderX < x + 0.015 && y < 0.8 && y > 0.2) *(sliderImg->data + p) = *(refImages[4]->data + p); // populate slider thumb
        }
    }

    for(unsigned r = 0; r < 5; r++) RASTERON_DEALLOC(refImages[r]);

    return menuQueue;
}

// --------------------------------  Animation Operations  -------------------------------- //

#ifdef RASTERON_ENABLE_ANIM

Rasteron_Queue* loadAnim_frame30(ImageSize size){ return (Rasteron_Queue*)internal_alloc_queue("frame30", size, 30); } 

Rasteron_Queue* loadAnim_frame60(ImageSize size){ return (Rasteron_Queue*)internal_alloc_queue("frame60", size, 60); } 

Rasteron_Queue* loadAnim_frame120(ImageSize size){ return (Rasteron_Queue*)internal_alloc_queue("frame120", size, 120); } 

#endif