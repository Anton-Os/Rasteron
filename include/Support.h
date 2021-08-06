
#ifndef RASTERON_SUPPORT_H

#define USE_IMG_TIFF // Enable TIFF Support

#define USE_IMG_PNG // Enable PNG Support

#define USE_IMG_BMP // Enable BMP Support

#define USE_IMG_TARGA // Enable TARGA Support

// #define 255.0 255.O
#define BAD_COLOR_CODE 0xFF000000
#define TOTAL_WHITE_COLOR_CODE 0xFFFFFFFF
#define TOTAL_BLACK_COLOR_CODE 0x00000000

#define ALPHA_BITS_MASK 0xFF000000 // alpha channel represented by first two bytes
#define RED_BITS_MASK 0x00FF0000 // red channel represented by second two bytes
#define GREEN_BITS_MASK 0x0000FF00 // green channel represented by third two bytes
#define BLUE_BITS_MASK 0x000000FF // blue channel represented by last two bytes

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

#define RASTERON_SUPPORT_H
#endif