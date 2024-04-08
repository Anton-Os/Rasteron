
#define RASTERON_WIN_HEIGHT 200
#define RASTERON_WIN_WIDTH 600
#define RASTERON_ENABLE_ANIM

#define GUI_COUNT 5

#include "Rasteron.h"

Rasteron_Queue* guiElements;

Rasteron_Image* canvasImg;
void keyEvent(char key){}
void mouseEvent(double x, double y){}
void timerEvent(unsigned secs){
    guiElements->index = secs % GUI_COUNT;
    if(canvasImg != NULL) RASTERON_DEALLOC(canvasImg);
    canvasImg = getFrameAt(guiElements, guiElements->index);
}

#include "Util_Demo.h"

int main(int argc, char** argv) {
    // if(canvasImg != NULL) RASTERON_DEALLOC(canvasImg);
    canvasImg = noiseImgOp_white((ImageSize){ RASTERON_WIN_HEIGHT, RASTERON_WIN_WIDTH }, UI_COLOR_BACKGROUND, UI_COLOR_BACKGROUND + 0xFF111111);

    Rasteron_Queue* checkButtons[5] = { 
        loadUI_checkBtn(MENU_Tiny), loadUI_checkBtn(MENU_Small), 
        loadUI_checkBtn(MENU_Medium), 
        loadUI_checkBtn(MENU_Large), loadUI_checkBtn(MENU_XL),
    };

    Rasteron_Queue* dials[5] = { 
        loadUI_dial(MENU_Tiny, 2), loadUI_dial(MENU_Small, 4), 
        loadUI_dial(MENU_Medium, 8), 
        loadUI_dial(MENU_Medium, 16), loadUI_dial(MENU_Medium, 32), 
    };

    Rasteron_Queue* sliders[5] = { 
        loadUI_slider(MENU_Tiny, 2), loadUI_slider(MENU_Small, 4), 
        loadUI_slider(MENU_Medium, 8), 
        loadUI_slider(MENU_Medium, 16), loadUI_slider(MENU_Medium, 32), 
    };

    guiElements = RASTERON_QUEUE_ALLOC("gui", createImgSize(256, 256), GUI_COUNT);

    Rasteron_Image* controlPanelImg = NULL;
    for(unsigned g = 0; g < 5; g++){
        Rasteron_Image* tempImg = (controlPanelImg == NULL)? copyImgOp(canvasImg) : copyImgOp(controlPanelImg);
        if(controlPanelImg != NULL) RASTERON_DEALLOC(controlPanelImg);

        controlPanelImg = copyImgOp(tempImg);
        // TODO: Add control panel data here

        addFrameAt(guiElements, controlPanelImg, g);
        RASTERON_DEALLOC(tempImg);
    }

    inputLoop(NULL);

    RASTERON_QUEUE_DEALLOC(guiElements);
    RASTERON_DEALLOC(canvasImg);
    for(unsigned b = 0; b < 5; b++) RASTERON_QUEUE_DEALLOC(checkButtons[b]);

    return 0;
}