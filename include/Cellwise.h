#ifndef IMAGE_LOADER_H
    #include "ImageLoader.h"
#endif

#define BOT_RIGHT_NBR_FLAG 1
#define BOT_NBR_FLAG 2
#define BOT_LEFT_NBR_FLAG 4
#define LEFT_NBR_FLAG 8
#define RIGHT_NBR_FLAG 16
#define TOP_RIGHT_NBR_FLAG 32
#define TOP_NBR_FLAG 64
#define TOP_LEFT_NBR_FLAG 128

typedef uint8_t nbrExistFlags;

nbrExistFlags checkNebrs(uint32_t imgWidth, uint32_t imgHeight, uin32_t index);