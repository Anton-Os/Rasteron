#define RASTERON_ENABLE_ANIM
#define RASTERON_ENABLE_FONT

#define GUI_COUNT 5

#define RASTERON_WIN_HEIGHT 180
#define RASTERON_WIN_WIDTH 650

#include "Rasteron.h"

#include "../_Demo.h"

void _onKeyEvent(char key){}
void _onPressEvent(double x, double y){}
void _onTickEvent(unsigned secs){ // WHY THIS DOESNT WORK?
    unsigned short index = secs % 5;

    if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    _outputImg = queue_getImg(_mainQueue, index);
}

// Rasteron_Image* controlPanelImgOp(enum MENU_Size size){}

int main(int argc, char** argv) {
    char fontFilePath[1024];
    // Image Creation // TODO: Create images that create control panel

    if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    _outputImg = noiseImgOp_white((ImageSize){ RASTERON_WIN_HEIGHT, RASTERON_WIN_WIDTH }, UI_COLOR_BACKGROUND, UI_COLOR_BACKGROUND + 0xFF111111);

    enum MENU_Size size = MENU_Medium;

    Rasteron_Queue* dials[GUI_COUNT] = { 
        loadUI_dial(size, 2), loadUI_dial(size, 4), loadUI_dial(size, 6), loadUI_dial(size, 8), loadUI_dial(size, 16), 
    };

    Rasteron_Queue* sliders[GUI_COUNT] = { 
        loadUI_slider(size, 2), loadUI_slider(size, 4), loadUI_slider(size, 6), loadUI_slider(size, 8), loadUI_slider(size, 16), 
    };

    Rasteron_Queue* icons[GUI_COUNT] = {
        loadUI_iconBtn(size, "android"), loadUI_iconBtn(size, "bench"), loadUI_iconBtn(size, "candy"), loadUI_iconBtn(size, "walkman"), loadUI_iconBtn(size, "uv-sun")
    };

    Rasteron_Queue* checks[GUI_COUNT] = { 
        loadUI_checkBtn(MENU_Tiny), loadUI_checkBtn(MENU_Small), loadUI_checkBtn(MENU_Medium), loadUI_checkBtn(MENU_Large), loadUI_checkBtn(MENU_XL),
    };

    /* Rasteron_Queue* guiItems[4][GUI_COUNT] = {
        { loadUI_dial(size, 2), loadUI_dial(size, 4), loadUI_dial(size, 6), loadUI_dial(size, 8), loadUI_dial(size, 16) },
        { loadUI_slider(size, 2), loadUI_slider(size, 4), loadUI_slider(size, 6), loadUI_slider(size, 8), loadUI_slider(size, 16) },
        { loadUI_iconBtn(size, "android"), loadUI_iconBtn(size, "bench"), loadUI_iconBtn(size, "candy"), loadUI_iconBtn(size, "walkman"), loadUI_iconBtn(size, "uv-sun") },
        { loadUI_checkBtn(MENU_Tiny), loadUI_checkBtn(MENU_Small), loadUI_checkBtn(MENU_Medium), loadUI_checkBtn(MENU_Large), loadUI_checkBtn(MENU_XL) }
    }; */

    genFullFilePath("New-Tegomin.ttf", fontFilePath);
    Rasteron_Text textObjs[GUI_COUNT] = { // TODO: Add legitimate text
        { "123456789", fontFilePath, UI_COLOR_BACKGROUND, UI_COLOR_FOREGROUND },
        { "QWERTYUIOPASDFGHJKLZXCVBNM", fontFilePath, UI_COLOR_BACKGROUND, UI_COLOR_FOREGROUND },
        { "Hello World", fontFilePath, UI_COLOR_BACKGROUND, UI_COLOR_FOREGROUND },
        { "Option #1", fontFilePath, UI_COLOR_BACKGROUND, UI_COLOR_FOREGROUND },
        { "Welcome to GUI Land!", fontFilePath, UI_COLOR_BACKGROUND, UI_COLOR_FOREGROUND },
    };

    _mainQueue = RASTERON_QUEUE_ALLOC("gui", internal_create_size(1024, 1024), GUI_COUNT);

    Rasteron_Image* backgroundImg = noiseImgOp_white((ImageSize){ RASTERON_WIN_HEIGHT, RASTERON_WIN_WIDTH }, UI_COLOR_BACKGROUND, UI_COLOR_BACKGROUND + 0xFF111111);
    for(unsigned g = 0; g < GUI_COUNT; g++){
        Rasteron_Image* guiImg = copyImgOp(queue_getImg(icons[g], 0));
        Rasteron_Image* insertImg = insertImgOp(guiImg, backgroundImg, 0.0, 0.0);

        queue_addImg(_mainQueue, insertImg, g);

        RASTERON_DEALLOC(guiImg);
        RASTERON_DEALLOC(insertImg);
    }
    RASTERON_DEALLOC(backgroundImg);

    _run(argc, argv); // system specific initialization and continuous loop

    // Deallocation

    RASTERON_QUEUE_DEALLOC(_mainQueue);
    RASTERON_DEALLOC(_outputImg);
    for(unsigned b = 0; b < GUI_COUNT; b++) RASTERON_QUEUE_DEALLOC(checks[b]);
    for(unsigned b = 0; b < GUI_COUNT; b++) RASTERON_QUEUE_DEALLOC(dials[b]);
    for(unsigned b = 0; b < GUI_COUNT; b++) RASTERON_QUEUE_DEALLOC(sliders[b]);
    for(unsigned b = 0; b < GUI_COUNT; b++) RASTERON_QUEUE_DEALLOC(icons[b]);
    // for(unsigned g = 0; g < 4; g++)
    //    for(unsigned b = 0; b < GUI_COUNT; b++) RASTERON_QUEUE_DEALLOC(guiItems[g][b]);

    return 0;
}
