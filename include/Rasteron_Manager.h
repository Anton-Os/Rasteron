#include "type.h"

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
} Rasteron_Manager;

// TODO: Create initialization function
// TODO: Create destroy function