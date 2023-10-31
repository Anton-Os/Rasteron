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
    Rasteron_Image* state; // active state from index

    unsigned short frameCount; // total number of frames
    Rasteron_Image** frameData; // data contents of frames
} Rasteron_Queue;

Rasteron_Queue* alloc_queue(const char* prefix, ImageSize size, unsigned frameCount);
void free_queue(Rasteron_Queue* queue);

void addFrameAt(Rasteron_Queue* queue, ref_image_t refImage, unsigned short frameIndex); // copies target image into index of queue struct
Rasteron_Image* getFrameAt(Rasteron_Queue* queue, unsigned short frameIndex); // gets frame from an index within queue


// --------------------------------  MenuItem  -------------------------------- //

// TODO: Implement menu and GUI support features



// --------------------------------  Animation  -------------------------------- //

#ifdef RASTERON_ENABLE_ANIM

// TODO: Implement Aimation and Video encoding features

#endif


#define RASTERON_QUEUE_H
#endif