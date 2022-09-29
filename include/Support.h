
#ifndef RASTERON_SUPPORT_H

#define ASSETS_DIR "C:/AntonDocs/Codex/Ao-Project/Rasteron/master/assets"
#define FONTS_DIR ""

#define USE_IMG_TIFF // Enable TIFF Support
#define USE_IMG_PNG // Enable PNG Support
#define USE_IMG_BMP // Enable BMP Support

#define ZERO_COLOR 0x00000000
#define WHITE_COLOR 0xFFFFFFFF
#define BLACK_COLOR 0xFF000000

#define ALPHA_CHANNEL 0xFF000000 // alpha channel represented by first two bytes
#define RED_CHANNEL 0x00FF0000 // red channel represented by second two bytes
#define GREEN_CHANNEL 0x0000FF00 // green channel represented by third two bytes
#define BLUE_CHANNEL 0x000000FF // blue channel represented by last two bytes

typedef enum {
    CHANNEL_Alpha,
    CHANNEL_Red,
    CHANNEL_Green,
    CHANNEL_Blue
} CHANNEL_Type;

#define TWOPOWER(x) (1 << (x))

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define RASTERON_SUPPORT_H
#endif
