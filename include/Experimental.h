#ifndef RASTERON_EXPERIMENTAL_H

#define RASTERON_ENABLE_ANIM
#define RASTERON_ENABLE_FONT

#include "Rasteron.h"

/* 
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
    Rasteron_Text textObjs[1024];
    Rasteron_Message messageObjs[256];
#endif
} Custom_Experimental; 
*/

// Image Creation Functions

Rasteron_Image* oragamiImgOp(enum FLIP_Type flip, double xCrop, double yCrop); // Flipping, mirroring, and cropping
Rasteron_Image* nestboxesImgOp(double x, double y); // Looped nesting of ever-smaller squares
Rasteron_Image* lensesImgOp(enum CHANNEL_Type channel); // lenses image to color ranges and performing channel operations
Rasteron_Image* hypnosisImgOp(unsigned pArg, unsigned color1, unsigned color2); // Visual lense effects and enhancements to base image
Rasteron_Image* typographyImgOp(unsigned color1, unsigned color2); // Drawing text example
Rasteron_Image* grassNoiseImgOp(int noiseOp, unsigned xCells, unsigned yCells); // Mixing and matching various noise functions while adding experimental arguments
Rasteron_Image* gameOfLifeImgOp(int seedOp); // Seeding and performing cellular logic operations between neighbors iteratively
Rasteron_Image* patchingImgOp(unsigned short points); // Vornoi and other distance related functions
Rasteron_Image* mosaicImgOp(double z1, double z2); // Creates a regular repeating triangular pattern
Rasteron_Image* interferenceImgOp(unsigned short inc, unsigned short dec); // Combines grid tiles, feild operations and seeding to create the result
Rasteron_Image* perturbImgOp(double xCenter, double yCenter); // Purturbs a cross image using red as x, green as y, and blue for lightness
Rasteron_Image* ballingImgOp(double size); // Black and white blobs transformed into gradients effects
Rasteron_Image* stratifyImgOp(unsigned short levels); // Stratifies colors into discrete intervals based on brightness
Rasteron_Image* chemicalsImgOp(unsigned color1, unsigned color2); // TODO: Implement this

Rasteron_Image* expImgOp(); // placeholder for new images
Rasteron_Image* expImgOp1(); // placeholder for new images
Rasteron_Image* expImgOp2(); // placeholder for new images
Rasteron_Image* expImgOp3(); // placeholder for new images
Rasteron_Image* expImgOp4(); // placeholder for new images
Rasteron_Image* expImgOp5(); // placeholder for new images
Rasteron_Image* expImgOp6(); // placeholder for new images
Rasteron_Image* expImgOp7(); // placeholder for new images
Rasteron_Image* expImgOp8(); // placeholder for new images
Rasteron_Image* expImgOp9(); // placeholder for new images
Rasteron_Image* expImgOp10(); // placeholder for new images
Rasteron_Image* expImgOp11(); // placeholder for new images
Rasteron_Image* expImgOp12(); // placeholder for new images

#define RASTERON_EXPERIMENTAL_H
#endif