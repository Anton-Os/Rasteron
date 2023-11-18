#ifndef RASTERON_H

#ifdef __cplusplus
// namespace Rasteron {
extern "C"{
#endif

#include "Loader.h"

#include "type.h"
#include "typeinit.h"


// --------------------------------   Basic Operations    -------------------------------- //
//
//      Involves the most basic operations such as loading from files, fillin in a solid color,
//      copying, cropping, mirroring, and flipping

Rasteron_Image* loadImgOp(const char* fileName); // creates an image from a file
Rasteron_Image* solidImgOp(ImageSize size, uint32_t color); // creates a solid color image
Rasteron_Image* copyImgOp(ref_image_t refImage); // creates copy of image
Rasteron_Image* cropImgOp(ref_image_t refImage, enum CROP_Type type, double factor); // creates cropped image
Rasteron_Image* mirrorImgOp(ref_image_t refImage); // creates horizontal mirror image
Rasteron_Image* flipImgOp(ref_image_t refImage, enum FLIP_Type type); // creates flipped image


// --------------------------------   Filtering Operations    -------------------------------- //
//
//      Distills an image by modifying data by channels

Rasteron_Image* greyImgOp(ref_image_t refImage); // creates greyscale image
Rasteron_Image* filterImgOp(ref_image_t refImage, CHANNEL_Type channel); // creates isolated filter image
Rasteron_Image* channelImgOp(ref_image_t refImage, CHANNEL_Type channel); // creates averaged channel image

// --------------------------------   Mixing Operations    -------------------------------- //
//
//     Operations requiring two or more images to calculate or combine into a final color,
//     in most cases require matching width and height for both target images

Rasteron_Image* insertImgOp(ref_image_t image1, ref_image_t image2, double coordX, double coordY); // insert image at a given offset
Rasteron_Image* mixingImgOp(ref_image_t image1, ref_image_t image2, mixCallback callback); // creates effect per pixel from 2 images 
Rasteron_Image* blendImgOp(ref_image_t image1, ref_image_t image2); // creates blended image
Rasteron_Image* fusionImgOp(ref_image_t image1, ref_image_t image2);  // creates fused image


// --------------------------------  Procedural Operations   -------------------------------- //
//
//      Generates images based on various callbacks with inputs as data and outputs as colors

// Rasteron_Image* checkerImgOp(ImageSize size, ColorLatice lattice);
Rasteron_Image* recolorImgOp(ref_image_t refImage, recolorCallback callback); // callbacks from input color
Rasteron_Image* mapImgOp(ImageSize size, coordCallback callback); // callbacks from x and y coordinates
Rasteron_Image* fieldImgOp(ImageSize size, const ColorPointTable* colorPointTable, fieldCallback callback); // callbacks based on distance to nearest point
Rasteron_Image* vornoiImgOp(ImageSize size, const ColorPointTable* colorPointTable); // implementation of vornoi algorithm


// --------------------------------  Noise Operations  -------------------------------- //
//
//      Operations for creating various types of noises

Rasteron_Image* noiseImgOp_white(ImageSize size, uint32_t color1, uint32_t color2); // white noise generated between two values
Rasteron_Image* noiseImgOp_gradient(ImageSize size, ColorLattice lattice); // gradient noise over lattice
Rasteron_Image* noiseImgOp_fbm(ImageSize size, ColorLatticeTable latticeTable); // fbm noise over layered gradients
// TODO: Include domain warping function


// --------------------------------   Seeding Operations    -------------------------------- //
//
//      Seeds pixels at randomized intervals over an image based on color and probability

Rasteron_Image* seededImgOp(ref_image_t refImage, ColorSeed seed); // seeds pixels on image from probability
Rasteron_Image* seededImgOp_tabled(ref_image_t refImage, const ColorSeedTable* seedTable); // seeds pixels on image from seed weights


// --------------------------------   Cellwise Opertaions    -------------------------------- //
//
//      For each pixel adjascent neighbors are determined and used to compute the final color

Rasteron_Image* cellwiseImgOp(ref_image_t refImage, nebrCallback8 callback, unsigned iterations); // pattern image from neighbors with iterations
Rasteron_Image* cellwiseImgOp_horizontal(ref_image_t refImage, nebrCallback2 callback); // horizontal pattern image from left & right neighbors
Rasteron_Image* cellwiseImgOp_vertical(ref_image_t refImage, nebrCallback2 callback); // vertical pattern image from top & down neighbors


#include "Queue_Feature.h" // enables sequenced image types with potential animation support
#include "Spatial_Feature.h" // enables spatial types, including sprite and heightmap

#ifdef RASTERON_ENABLE_FONT
#include "Text_Feature.h" // enables single line text and expanded message objects
#endif

// TODO: Include Rasteron_Factory

#ifdef __cplusplus
} // }
#endif

#define RASTERON_H
#endif