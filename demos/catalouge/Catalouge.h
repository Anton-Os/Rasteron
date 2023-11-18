#include "Rasteron.h"

#define PRIMITIVES_MAX 1024
#define TABLES_MAX 512
#define IMAGES_MAX 4096

typedef struct {
    ColorSeed seeds[PRIMITIVES_MAX];
    ColorSeedTable seedTables[TABLES_MAX];
    ColorSwatch swatches[PRIMITIVES_MAX];
    PixelPoint pixelPoints[PRIMITIVES_MAX];
    PixelPointTable pixelPointTables[TABLES_MAX];
    ColorPoint colorPoints[PRIMITIVES_MAX];
    ColorPointTable colorPointTables[TABLES_MAX];
    ColorLattice lattices[PRIMITIVES_MAX];
    ColorLatticeTable latticeTables[TABLES_MAX];

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
#if defined(RASTERON_ENABLE_ANIM) && defined(RASTERON_ENABLE_FONT)
    Rasteron_MenuItem* menuItems[2048]; // unsigned short menuItemIndex // for tracking active menu items
#endif
} Custom_Catalouge;

// Management Functions

// Custom_Catalouge* loadCatalouge();
// void delCatalouge(Custom_Catalouge* catalouge);

// Image Creation Functions

Rasteron_Image* slicediceImgOp(enum FLIP_Type flip, double xCrop, double yCrop); // Flipping, mirroring, and cropping
Rasteron_Image* concentricImgOp(double x, double y); // Looped nesting of ever-smaller squares
Rasteron_Image* distillingImgOp(enum CHANNEL_Type channel); // Distilling image to color ranges and performing channel operations
Rasteron_Image* overlayerImgOp(unsigned pArg, unsigned color1, unsigned color2); // Visual lense effects and enhancements to base image
Rasteron_Image* multiNoiseImgOp(); // Mixing and matching various noise functions while adding experimental arguments
Rasteron_Image* cellAutomataImgOp(); // Seeding and performing cellular logic operations between neighbors iteratively
Rasteron_Image* proxPatternImgOp(); // Vornoi and other distance related functions
Rasteron_Image* dynamicTextImgOp(); // Playing and expirementing with fractal patterns