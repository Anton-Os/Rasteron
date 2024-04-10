
#define RASTERON_WIN_HEIGHT 180
#define RASTERON_WIN_WIDTH 650

#define RASTERON_ENABLE_ANIM
#define RASTERON_ENABLE_FONT

#define GUI_COUNT 5

#include "Rasteron.h"

#include "Util_Demo.h"

void keyEvent(char key){}
void mouseEvent(double x, double y){}
void timerEvent(unsigned secs){ // WHY THIS DOESNT WORK?
    int index = secs % 5;

    if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    _outputImg = getFrameAt(_mainQueue, index);
}

// Rasteron_Image* controlPanelImgOp(enum MENU_Size size){}

int main(int argc, char** argv) {
    // Image Creation // TODO: Create images that create control panel

    if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    _outputImg = noiseImgOp_white((ImageSize){ RASTERON_WIN_HEIGHT, RASTERON_WIN_WIDTH }, UI_COLOR_BACKGROUND, UI_COLOR_BACKGROUND + 0xFF111111);

    Rasteron_Queue* checks[GUI_COUNT] = { 
        loadUI_checkBtn(MENU_Tiny), loadUI_checkBtn(MENU_Small), loadUI_checkBtn(MENU_Medium), loadUI_checkBtn(MENU_Large), loadUI_checkBtn(MENU_XL),
    };

    Rasteron_Queue* dials[GUI_COUNT] = { 
        loadUI_dial(MENU_Tiny, 5), loadUI_dial(MENU_Small, 5), loadUI_dial(MENU_Medium, 5), loadUI_dial(MENU_Large, 5), loadUI_dial(MENU_XL, 5), 
    };

    Rasteron_Queue* sliders[GUI_COUNT] = { 
        loadUI_slider(MENU_Tiny, 5), loadUI_slider(MENU_Small, 5), loadUI_slider(MENU_Medium, 5), loadUI_slider(MENU_Large, 5), loadUI_slider(MENU_XL, 5), 
    };

    /* Rasteron_Queue* icons[GUI_COUNT] = {
        loadUI_iconBtn(MENU_Tiny, "info"), loadUI_iconBtn(MENU_Small, "info"), loadUI_iconBtn(MENU_Medium, "info"), loadUI_iconBtn(MENU_Large, "info"), loadUI_iconBtn(MENU_XL, "info"),
    }; */

    Rasteron_Text textObjs[GUI_COUNT] = {
        { "", "", UI_COLOR_BACKGROUND, UI_COLOR_FOREGROUND },
        { "", "", UI_COLOR_BACKGROUND, UI_COLOR_FOREGROUND },
        { "", "", UI_COLOR_BACKGROUND, UI_COLOR_FOREGROUND },
        { "", "", UI_COLOR_BACKGROUND, UI_COLOR_FOREGROUND },
        { "", "", UI_COLOR_BACKGROUND, UI_COLOR_FOREGROUND },
    };

    // Queue Creation // TODO: Add necessary components to queue

    _mainQueue = RASTERON_QUEUE_ALLOC("gui", createImgSize(RASTERON_WIN_HEIGHT, RASTERON_WIN_WIDTH), GUI_COUNT);

    for(unsigned g = 0; g < GUI_COUNT; g++){
        Rasteron_Image* backgroundImg = solidImgOp((ImageSize){ RASTERON_WIN_HEIGHT, RASTERON_WIN_WIDTH }, UI_COLOR_BACKGROUND);

        Rasteron_Image* insertImg1 = insertImgOp(getFrameAt(sliders[2], g), backgroundImg, 0.0, -0.5); // (-1.0 / GUI_COUNT) * g);
        Rasteron_Image* insertImg2 = insertImgOp(getFrameAt(dials[2], g), insertImg1, 0.9, -0.5);
        Rasteron_Image* insertImg3 = insertImgOp(getFrameAt(checks[2], (g % 2) + 1), insertImg2, -0.9, -0.5);

        addFrameAt(_mainQueue, insertImg3, g); // adding background

        RASTERON_DEALLOC(backgroundImg);
        RASTERON_DEALLOC(insertImg1); RASTERON_DEALLOC(insertImg2); RASTERON_DEALLOC(insertImg3);
    }

    // Event Loop

    inputLoop(NULL);

    // Deallocation

    RASTERON_QUEUE_DEALLOC(_mainQueue);
    RASTERON_DEALLOC(_outputImg);
    // for(unsigned b = 0; b < GUI_COUNT; b++) RASTERON_QUEUE_DEALLOC(checks[b]);
    // for(unsigned b = 0; b < GUI_COUNT; b++) RASTERON_QUEUE_DEALLOC(dials[b]);
    // for(unsigned b = 0; b < GUI_COUNT; b++) RASTERON_QUEUE_DEALLOC(sliders[b]);
    // for(unsigned b = 0; b < GUI_COUNT; b++) RASTERON_QUEUE_DEALLOC(icons[b]);

    return 0;
}