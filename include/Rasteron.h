#ifndef RASTERON_H

#ifdef __cplusplus
extern "C"{
#endif

#include "Loader.h"

#include "typedef.h"
#include "typeinit.h"


// --------------------------------   Basic Operations    -------------------------------- //
//
//      Involves the most basic operations such as loading from files, fillin in a solid color,
//      copying, cropping, mirroring, and flipping

Rasteron_Image* loadImgOp(const char* fileName); // creates an image from a file
Rasteron_Image* solidImgOp(ImageSize size, uint32_t color); // creates a solid color image
Rasteron_Image* copyImgOp(ref_image_t refImage); // creates copy of image
Rasteron_Image* resizeImgOp(ImageSize size, ref_image_t refImage); // resize image to size
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
Rasteron_Image* fusionImgOp(ref_image_t image1, ref_image_t image2); // creates fused image


// --------------------------------  Procedural Operations   -------------------------------- //
//
//      Generates images based on various callbacks with inputs as data and outputs as colors

Rasteron_Image* recolorImgOp(ref_image_t refImage, recolorCallback callback); // callbacks from input color
Rasteron_Image* mapImgOp(ImageSize size, coordCallback callback); // callbacks from x and y coordinates
Rasteron_Image* fieldImgOp(ImageSize size, const ColorPointTable* colorPointTable, fieldCallback callback); // callbacks to nearest point w distance
Rasteron_Image* fieldExtImgOp(ImageSize size, const ColorPointTable* colorPointTable, fieldCallback3 callback); // callbacks to 3 nearest points w distance
Rasteron_Image* vornoiImgOp(ImageSize size, const ColorPointTable* colorPointTable); // implementation of vornoi algorithm

Rasteron_Image* seededImgOp(ref_image_t refImage, const ColorPointTable* colorPointTable);
// Rasteron_Image* turingPatternImgOp();


// --------------------------------  Noise Operations  -------------------------------- //
//
//      Operations for creating various types of noises

Rasteron_Image* noiseImgOp_white(ImageSize size, uint32_t color1, uint32_t color2); // white noise generated between two values
Rasteron_Image* noiseImgOp_grid(ImageSize size, ColorGrid grid); // gradient noise over grid
Rasteron_Image* noiseImgOp_warp(ImageSize size, ColorGrid grid, ColorGrid domain); // warping grid over domain


// --------------------------------   Cellwise Opertaions    -------------------------------- //
//
//      For each pixel adjascent neighbors are determined and used to compute the final color

Rasteron_Image* cellwiseRowImgOp(ref_image_t refImage, nebrCallback2 callback); // horizontal generated image from left & right neighbors
Rasteron_Image* cellwiseColImgOp(ref_image_t refImage, nebrCallback2 callback); // vertically generated image from top & down neighbors
Rasteron_Image* cellwiseExtImgOp(ref_image_t refImage, nebrCallback8 callback, unsigned short iterations); // 2D generated image from 8 neighbors

Rasteron_Image* antialiasImgOp(ref_image_t refImage); // performs antialiasing operation



// --------------------------------   Rasteron exts   -------------------------------- //

#include "Spacial.h" // enables spatial types, including sprite and heightmap

#ifdef RASTERON_ENABLE_ANIM
#include "Queue.h" // enables sequenced image types with potential animation support
#endif
#ifdef RASTERON_ENABLE_FONT
#include "Font.h" // enables single line text and expanded message objects
#endif

// TODO: Include Rasteron_Factory

#ifdef __cplusplus
} // }
#endif

#define RASTERON_H
#endif