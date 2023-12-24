#ifndef RASTERON_EXPERIMENTAL_H

#define RASTERON_ENABLE_ANIM
#define RASTERON_ENABLE_FONT

#include "Rasteron.h"

#define PRIMITIVES_MAX 1024
#define TABLES_MAX 512
#define IMAGES_MAX 4096

typedef struct {
    ColorSwatch swatches[PRIMITIVES_MAX];
    PixelPoint pixelPoints[PRIMITIVES_MAX];
    PixelPointTable pixelPointTables[TABLES_MAX];
    ColorPoint colorPoints[PRIMITIVES_MAX];
    ColorPointTable colorPointTables[TABLES_MAX];
    ColorGrid grids[PRIMITIVES_MAX];
    ColorGridTable gridTables[TABLES_MAX];

    Rasteron_Image* images[IMAGES_MAX]; // unsigned short imageIndex; // for tracking active images
    
    Rasteron_Sprite* sprites[512]; // unsigned short spriteIndex; // for tracking active sprites
    Rasteron_Heightmap* heightmaps[512]; // unsigned short heightmapIndex; // for tracking active heightmaps

#ifdef RASTERON_ENABLE_ANIM
    Rasteron_Queue* queues[256]; // unsigned short queueIndex; // for tracking active queues
#endif
#ifdef RASTERON_ENABLE_FONT
    // Rasteron_Text textObjs[1024];
    // Rasteron_Message messageObjs[256];
#endif
} Custom_Experimental;

// Management Functions

// Custom_Experimental* loadExperimental();
// void delExperimental(Custom_Experimental* catalouge);

// Image Creation Functions

Rasteron_Image* oragamiImgOp(enum FLIP_Type flip, double xCrop, double yCrop); // Flipping, mirroring, and cropping
Rasteron_Image* nestboxesImgOp(double x, double y); // Looped nesting of ever-smaller squares
Rasteron_Image* distillingImgOp(enum CHANNEL_Type channel); // Distilling image to color ranges and performing channel operations
Rasteron_Image* overlayerImgOp(unsigned pArg, unsigned color1, unsigned color2); // Visual lense effects and enhancements to base image
Rasteron_Image* wordsmithImgOp(const char* text); // Drawing text example
Rasteron_Image* multiNoiseImgOp(int noiseOp); // Mixing and matching various noise functions while adding experimental arguments
Rasteron_Image* cellAutomataImgOp(int seedOp); // Seeding and performing cellular logic operations between neighbors iteratively
Rasteron_Image* proxPatternImgOp(unsigned short points); // Vornoi and other distance related functions

#define RASTERON_EXPERIMENTAL_H
#endif