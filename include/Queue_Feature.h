#ifndef RASTERON_QUEUE_H

#include "type.h"

// --------------------------------  Queue  -------------------------------- //

// #define QUEUE_IMAGE_HEIGHT 1024
// #define QUEUE_IMAGE_WIDTH 1024
#define QUEUE_IMAGE_BACKGROUND 0xFFFFFF00 // yellow color
#define QUEUE_STATE_DEFAULT 0 // default state

// typedef struct { unsigned short count; Rasteron_Image** data; } Rasteron_FrameData;

typedef struct {
    char* prefix; // each child name follows the prefix with a number

    unsigned short index; // index to retrieve the state
    // Rasteron_Image** state; // active state from index

    unsigned short frameCount; // total number of frames
    Rasteron_Image** frameData; // data contents of frames
} Rasteron_Queue;

Rasteron_Queue* alloc_queue(const char* prefix, ImageSize size, unsigned frameCount);
void dealloc_queue(Rasteron_Queue* queue);

void addFrameAt(Rasteron_Queue* queue, ref_image_t refImage, unsigned short frameIndex); // copies target image into index of queue struct
Rasteron_Image* getFrameAt(Rasteron_Queue* queue, unsigned short frameIndex); // gets frame from an index within queue


// --------------------------------  UI  -------------------------------- //

#define UI_COLOR_BACKGROUND 0xFF111111
#define UI_COLOR_FOREGROUND 0xFF333333
#define UI_COLOR_CONTENT 0xFF00FF00

enum MENU_Size { MENU_Tiny, MENU_Small, MENU_Medium, MENU_Large, MENU_XL };

ImageSize getUI_ImageSize(enum MENU_Size size);

enum MENU_Style { MENU_Regular }; // TODO: Add support for at least 2 alternate styles

// void setUI_style();
void setUI_colorScheme(unsigned bgColor, unsigned fgColor, unsigned cnColor); // sets background, foreground, and content

// enum TOGGLE_Type { TOGGLE_Switch }; // TODO: Add radio, checkmark, and other toggles

Rasteron_Queue* loadUI_checkBtn(enum MENU_Size size);

// Rasteron_Queue* loadUI_keyBtn(enum MENU_Size size, char key);
// Rasteron_Queue* loadUI_iconBtn(enum MENU_Size size, char* name);

Rasteron_Queue* loadUI_dial(enum MENU_Size size, unsigned short turns);

Rasteron_Queue* loadUI_slider(enum MENU_Size size, unsigned short levels);

// --------------------------------  Animation  -------------------------------- //

#ifdef RASTERON_ENABLE_ANIM

Rasteron_Queue* loadAnim_frame30(ImageSize size);

Rasteron_Queue* loadAnim_frame60(ImageSize size);

Rasteron_Queue* loadAnim_frame120(ImageSize size);

#endif


#define RASTERON_QUEUE_H
#endif