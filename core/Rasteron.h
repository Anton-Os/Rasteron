#ifndef RASTERON_H

#ifdef __cplusplus
extern "C"{
#endif

#include "Loader.h"

#include "type_def.h"
#include "func_def.h"

// --------------------------------   Basic Operations    -------------------------------- //
//
//      Involves the most basic operations such as loading from files, fillin in a solid color,
//      copying, cropping, mirroring, and flipping

Rasteron_Image* loadImgOp(const char* fileName); // creates an image from a file
Rasteron_Image* solidImgOp(ImageSize size, uint32_t color); // creates a solid color image
Rasteron_Image* copyImgOp(ref_image_t refImage); // creates copy of imagea
Rasteron_Image* resizeImgOp(ImageSize size, ref_image_t refImage); // resize image to size
Rasteron_Image* cropImgOp(ref_image_t refImage, enum SIDE_Type type, double factor); // creates cropped image
Rasteron_Image* mirrorImgOp(ref_image_t refImage); // creates horizontal mirror image
Rasteron_Image* flipImgOp(ref_image_t refImage, enum FLIP_Type type); // creates flipped image
Rasteron_Image* cornerImgOp(ref_image_t refImage, double tl, double tr, double bl, double br); // cut corners in image

Rasteron_Image* errorImgOp(const char* error); // shows up on error

// --------------------------------   Filtering Operations    -------------------------------- //
//
//      Distills an image by modifying data by channels

Rasteron_Image* recolorImgOp(ref_image_t refImage, recolorCallback callback); // callbacks from input color
Rasteron_Image* remaskImgOp(ref_image_t refImage, remaskCallback callback); // callbacks from channel input
Rasteron_Image* greyImgOp(ref_image_t refImage); // creates greyscale image
Rasteron_Image* filterImgOp(ref_image_t refImage, CHANNEL_Type channel); // creates isolated filter image
Rasteron_Image* channelImgOp(ref_image_t refImage, CHANNEL_Type channel); // creates averaged channel image
Rasteron_Image* splitImgOp(ref_image_t refImage, unsigned short levels); // splits image into levels

Rasteron_Image* colorSwitchImgOp(ref_image_t refImage, CHANNEL_Type channel1, CHANNEL_Type channel2); // switches data between two channels
Rasteron_Image* colorShiftImgOp(ref_image_t refImage, short redShift, short greenShift, short blueShift); // shifts data between all channels

// TODO: Add operations to map colors to different values including switching color bits


// --------------------------------   Mixing Operations    -------------------------------- //
//
//     Operations requiring two or more images to calculate or combine into a final color,
//     in most cases require matching width and height for both target images

Rasteron_Image* insertImgOp(ref_image_t image1, ref_image_t image2, double coordX, double coordY); // insert image at a given offset
Rasteron_Image* mixingImgOp(ref_image_t image1, ref_image_t image2, mixCallback callback); // creates mix per pixel from 2 images
Rasteron_Image* mixingExtImgOp(ref_image_t image1, ref_image_t image2, ref_image_t image3, ref_image_t image4, mixCallback4 callback); // creates mix per pixel from 4 images

Rasteron_Image* blendImgOp(ref_image_t image1, ref_image_t image2); // creates blended image
Rasteron_Image* fusionImgOp(ref_image_t image1, ref_image_t image2); // creates fused image
Rasteron_Image* warpingImgOp(ref_image_t refImage, ref_image_t domainImage); // creates domain warped image


// --------------------------------  Procedural Operations   -------------------------------- //
//
//      Generates images based on various callbacks with inputs as data and outputs as colors

Rasteron_Image* mapImgOp(ImageSize size, coordCallback callback); // callbacks from x and y coordinates
Rasteron_Image* fieldImgOp(ImageSize size, const ColorPointTable* colorPointTable, fieldCallback callback); // callbacks to nearest point w distance
Rasteron_Image* fieldExtImgOp(ImageSize size, const ColorPointTable* colorPointTable, fieldCallback3 callback); // callbacks to 3 nearest points w distance
Rasteron_Image* vornoiImgOp(ImageSize size, const ColorPointTable* colorPointTable); // implementation of vornoi algorithm

Rasteron_Image* gradientImgOp(ImageSize size, enum SIDE_Type side, unsigned color1, unsigned color2);
Rasteron_Image* linedImgOp(ImageSize size, unsigned color1, unsigned color2, unsigned short divs, double rotation);
Rasteron_Image* checkeredImgOp(ImageSize size, ColorGrid grid); // Rasteron_Image* tiledImgOp(ImageSize size, ColorGrid grid, TILE_Type)

// Add tiling section???


// --------------------------------  Noise Operations  -------------------------------- //
//
//      Operations for creating various types of noises

Rasteron_Image* noiseImgOp_white(ImageSize size, uint32_t color1, uint32_t color2); // white noise generated between two values
Rasteron_Image* noiseExtImgOp_value(ImageSize size, ColorGrid grid, float (*callback)(float)); // extended version that can be reused
Rasteron_Image* noiseImgOp_value(ImageSize size, ColorGrid grid); // imitation of perlin noise
Rasteron_Image* noiseImgOp_tiled(ImageSize size, ColorGrid grid); // variation of value noise with discrete tiles
Rasteron_Image* noiseImgOp_scratch(ImageSize size, ColorGrid grid); // variation of value noise with scratch effects

Rasteron_Image* noiseImgOp_octave(ImageSize size, ColorGrid grid, unsigned short octaves); // imitation of brown noise
Rasteron_Image* noiseImgOp_low(ImageSize size, ColorGrid grid, unsigned short octaves); // imitation of pink noise
Rasteron_Image* noiseImgOp_hi(ImageSize size, ColorGrid grid, unsigned short octaves); // imitation of blue noise
Rasteron_Image* noiseImgOp_diff(ImageSize size, ColorGrid grid, unsigned short octaves); // noise substraction for cool effects


// --------------------------------   Cellwise Opertaions    -------------------------------- //
//
//      For each pixel adjascent neighbors are determined and used to compute the final color

Rasteron_Image* cellwiseImgOp(ref_image_t refImage, nebrCallback8 callback); // 2D generated image from 8 neighbors
Rasteron_Image* cellwiseExtImgOp(ref_image_t refImage, nebrCallback8 callback, unsigned short iterations); // 2D iterated image from recursive 8 neighbors
Rasteron_Image* cellwiseRowImgOp(ref_image_t refImage, nebrCallback2 callback); // horizontal generated image from left & right neighbors
Rasteron_Image* cellwiseColImgOp(ref_image_t refImage, nebrCallback2 callback); // vertically generated image from top & down neighbors
Rasteron_Image* cellwiseOutImgOp(ImageSize size, unsigned color1, unsigned color2, nebrCallback5 callback); // expanding outwards sees neighbors from periphery

Rasteron_Image* antialiasImgOp(ref_image_t refImage, unsigned short times); // performs antialiasing operation


// --------------------------------   Extensible Types   -------------------------------- //

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
