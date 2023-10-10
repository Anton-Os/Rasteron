#ifndef RASTERON_H

#ifdef __cplusplus
extern "C"{
#endif

#include "Loader.h"

// #include "type.h"
#include "typeinit.h"

// TODO: EACH FUNCTION NEEDS A TEST TO COORESPOND TO IT!

// --------------------------------   Basics Operations    -------------------------------- //

Rasteron_Image* loadImgOp(const char* fileName); // creates an image from a file
Rasteron_Image* solidImgOp(ImageSize size, uint32_t color); // creates a solid color image
Rasteron_Image* copyedImgOp(ref_image_t refImage); // creates copy of image
Rasteron_Image* mirroredImgOp(ref_image_t refImage); // creates mirror image
Rasteron_Image* flippedImgOp(ref_image_t refImage, enum FLIP_Type flip); // creates flipped image

// --------------------------------   Filtering Operations    -------------------------------- //

Rasteron_Image* greyscaledImgOp(ref_image_t refImage); // creates greyscale image
Rasteron_Image* filteredImgOp(ref_image_t refImage, CHANNEL_Type channel); // creates isolated filter image
Rasteron_Image* channeledImgOp(ref_image_t refImage, CHANNEL_Type channel); // creates averaged channel image

// --------------------------------   Mixing Operations    -------------------------------- //

Rasteron_Image* blendsImgOp(ref_image_t image1, ref_image_t image2); // creates blended image
Rasteron_Image* fusionsImgOp(ref_image_t image1, ref_image_t image2);  // creates fused image
Rasteron_Image* embedsImgOp(ref_image_t image1, ref_image_t image2, float scale); // embeds image to create border

// --------------------------------   Seeding Operations    -------------------------------- //

Rasteron_Image* seededImgOp(ref_image_t refImage, uint32_t color, double prob); // seeds pixels on image from probability
Rasteron_Image* seedweightedImgOp(ref_image_t refImage, const ColorSeedTable* seedTable); // seeds pixels on image from seed weights

// --------------------------------  Functional Operations   -------------------------------- //

// Rasteron_Image* checkerImgOp(ImageSize size, ColorLatice lattice);
Rasteron_Image* recoloredImgOp(ref_image_t refImage, recolorCallback callback); // callbacks from input color
Rasteron_Image* mapImgOp(ImageSize size, mapCallback callback); // callbacks from x and y coordinates
Rasteron_Image* fieldImgOp(ImageSize size, const ColorPointTable* colorPointTable, fieldCallback callback); // callbacks from distance
Rasteron_Image* vornoiImgOp(ImageSize size, const ColorPointTable* colorPointTable);

// --------------------------------   Cellwise Opertaions    -------------------------------- //

Rasteron_Image* cellpatternImgOp(ref_image_t refImage, nebrCallback8 callback, unsigned iterations); // pattern image from neighbors with iterations
Rasteron_Image* cellpatternImgOp_horizontal(ref_image_t refImage, nebrCallback2 callback); // horizontal pattern image from left & right neighbors
Rasteron_Image* cellpatternImgOp_vertical(ref_image_t refImage, nebrCallback2 callback); // vertical pattern image from top & down neighbors

// --------------------------------   Tiling Opertaions    -------------------------------- //

/* Rasteron_Image* squareTileImgOp(ImageSize size, ColorLattice lattice);
Rasteron_Image* triangleTileImgOp(ImageSize size, ColorLattice lattice);
Rasteron_Image* hexagonTileImgOp(ImageSize size, ColorLattice lattice); */

// --------------------------------  Noise Operations  -------------------------------- //

Rasteron_Image* whiteNoiseImgOp(ImageSize size, uint32_t color1, uint32_t color2); // white noise generated between two values
Rasteron_Image* gradientNoiseImgOp(ImageSize size, ColorLattice lattice); // gradient noise over lattice
// Rasteron_Image* octaveNoiseImgOp(ImageSize size, ColorLatticeTable latticeTable); // gradient noise stacked over lattice table
// Rasteron_Image* fbmNoiseImgOp(ImageSize size, ColorLattice lattice); // fractal brownian motion noise


#ifdef RASTERON_ENABLE_ANIM
#include "Animation.h"
#endif

#ifdef RASTERON_ENABLE_FONT
#include "Font.h"
#endif

// TODO: Include Rasteron_Factory

#ifdef __cplusplus
}
#endif

#define RASTERON_H
#endif