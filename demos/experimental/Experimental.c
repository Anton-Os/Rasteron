#include "experimental/Experimental.h"

static char fullFilePath[1024];

Rasteron_Image* nestboxesImgOp(double x, double y){
    Rasteron_Image* nestedImgs[9] = {
        solidImgOp((ImageSize){ 768, 768 }, 0xFFFF00FF),
        solidImgOp((ImageSize){ 512, 512 }, 0xFF00FFFF),
        solidImgOp((ImageSize){ 384, 384 }, 0xFFFF0000),
        solidImgOp((ImageSize){ 256, 256 }, 0xFF00FF00),
        solidImgOp((ImageSize){ 192, 192 }, 0xFF0000FF),
        solidImgOp((ImageSize){ 128, 128 }, 0xFFFC6B03),
        solidImgOp((ImageSize){ 96, 96 }, 0xFFFC03B5),
        solidImgOp((ImageSize){ 64, 64 }, 0xFF7303FC),
        solidImgOp((ImageSize){ 32, 32 }, 0xFFFFFFFF),
    };

    Rasteron_Image* combineImgs[8];
    combineImgs[0] = insertImgOp(nestedImgs[7], nestedImgs[8], x, y);
    for(unsigned i = 1; i < 8; i++) 
        combineImgs[i] = insertImgOp(combineImgs[i - 1], nestedImgs[8 - i - 1], x, y);

    Rasteron_Image* outerImg = solidImgOp((ImageSize){ 1024, 1024 }, 0xFFFFFF00);
    Rasteron_Image* finalImg = insertImgOp(combineImgs[7], outerImg, x, y);

    for(unsigned i = 0; i < 9; i++) dealloc_image(nestedImgs[i]);
    for(unsigned i = 0; i < 8; i++) dealloc_image(combineImgs[i]);  
    dealloc_image(outerImg);

    return finalImg;
}

Rasteron_Image* slicediceImgOp(enum FLIP_Type flip, double xCrop, double yCrop){ 
    genFullFilePath("Logroller.bmp", &fullFilePath);

    Rasteron_Image* loadedImg = loadImgOp(fullFilePath);

    enum CROP_Type cropX = (xCrop > 0.0)? CROP_Right : CROP_Left;
    Rasteron_Image* cropImgX = cropImgOp(loadedImg, cropX, xCrop);

    enum CROP_Type cropY = (yCrop > 0.0)? CROP_Top : CROP_Bottom;
    Rasteron_Image* cropImgY = cropImgOp(cropImgX, cropY, yCrop);

    Rasteron_Image* finalImg = flipImgOp(cropImgY, flip); // final operation is flip

    dealloc_image(loadedImg);
    dealloc_image(cropImgX, cropImgY);

    return finalImg;
}

static unsigned distill(unsigned color){
    uint8_t red = (color & RED_CHANNEL) >> 16;
    uint8_t green = (color & GREEN_CHANNEL) >> 8;
    uint8_t blue = color & BLUE_CHANNEL;

    if(red > green && red > blue) return 0xFF000000 | red << 16;
    else if(green > red && green > blue) return 0xFF000000 | green << 8;
    else if(blue > red && blue > green) return 0xFF000000 | blue;

    return color; // 0x00; // return black by default
}

Rasteron_Image* distillingImgOp(enum CHANNEL_Type channel){ 
    genFullFilePath("Gumdrops.bmp", &fullFilePath);

    Rasteron_Image* loadedImg = loadImgOp(fullFilePath);
    Rasteron_Image* finalImg = (channel >= 0 && channel <= 2) 
        ? filterImgOp(loadedImg, channel) 
        : recolorImgOp(loadedImg, distill);

    dealloc_image(loadedImg);

    return finalImg; 
}

static unsigned overlayer(unsigned color1, unsigned color2){
    // return (grayify8(color1) > grayify8(color2))? color1 - color2 : color2 + color1;
    return color1 * color2; // pretty cool effect by multiplying colors
}

Rasteron_Image* overlayerImgOp(unsigned pArg, unsigned color1, unsigned color2){ 
    Rasteron_Image* gradientImgs[5]= { 
        alloc_image("gradient-left", 1024, 1024),
        alloc_image("gradient-right", 1024, 1024),
        alloc_image("gradient-top", 1024, 1024),
        alloc_image("gradient-bottom", 1024, 1024),
        alloc_image("gradient-center", 1024, 1024)
    };

    for(unsigned p = 0; p < 1024 * 1024; p++){
		double x = (1.0 / (double)1024) * (unsigned)(p % 1024);
		double y = (1.0 / (double)1024) * (double)(p / 1024.0);

        *(gradientImgs[0]->data + p) = fuseColors(0xFFFFFFFF, color1, x);
        *(gradientImgs[1]->data + p) = fuseColors(0xFFFFFFFF, color2, 1.0 - x);
        *(gradientImgs[2]->data + p) = fuseColors(0xFFFFFFFF, invertColor(color1), 1.0 - y);
        *(gradientImgs[3]->data + p) = fuseColors(0xFFFFFFFF, invertColor(color2), y);

        // double centerDist = sqrt((fabs(0.5 - x) * fabs(0.5 - x)) + (fabs(0.5 - y) * fabs(0.5 - y)));
        double centerDist = pixelDistance(p, ((1024 * 1024) / 2) + (1024 / 2), 1024) * (1.0 / (double)1024);
        *(gradientImgs[4]->data + p) = fuseColors(0xFFFFFFFF, 0xFF000000, centerDist);
	}

    Rasteron_Image* mixImg1 = mixingImgOp(gradientImgs[0], gradientImgs[1], overlayer);
    Rasteron_Image* mixImg2 = mixingImgOp(gradientImgs[2], gradientImgs[3], overlayer);
    Rasteron_Image* mixImg3 = mixingImgOp(mixImg1, mixImg2, overlayer);

    // Rasteron_Image* finalImg = mixingImgOp(gradientImgs[0], gradientImgs[1], overlayer);
    Rasteron_Image* finalImg; // = mixingImgOp(mixImg3, gradientImgs[4], overlayer);
    switch(pArg){
        case 0: finalImg = copyImgOp(mixImg1); break;
        case 1: finalImg = copyImgOp(mixImg2); break;
        case 2: finalImg = copyImgOp(mixImg3); break;
        default: finalImg = mixingImgOp(mixImg2, gradientImgs[4], overlayer); break;
    }
    
    for(unsigned g = 0; g < 5; g++) dealloc_image(gradientImgs[g]);
    dealloc_image(mixImg1); dealloc_image(mixImg2); dealloc_image(mixImg3);

    return finalImg; 
} 

Rasteron_Image* multiNoiseImgOp(){ 
    // TODO: Add body
    return solidImgOp((ImageSize){ 1024, 1024 }, genRandColorVal());
}

Rasteron_Image* cellAutomataImgOp(){
    // TODO: Add body 
    return solidImgOp((ImageSize){ 1024, 1024 }, genRandColorVal()); 
}

Rasteron_Image* proxPatternImgOp(){ 
    // TODO: Add body
    return solidImgOp((ImageSize){ 1024, 1024 }, genRandColorVal()); 
}

Rasteron_Image* dynamicTextImgOp(const char* text){ 
    genFullFilePath("Tw-Cen-MT.ttf", &fullFilePath);

    Rasteron_Text textObj;
    textObj.fileName = fullFilePath;
    textObj.text = text;
    textObj.bkColor = 0xFF000000;
    textObj.fgColor = 0xFFFFFFFF;

    Rasteron_Image* outerImg = solidImgOp((ImageSize){1024, 1024}, 0xDDDDDDDD);

    Rasteron_Image* textImgs[5] = { 
        bakeText(&textObj, FONT_SIZE_TINY),
        bakeText(&textObj, FONT_SIZE_SMALL),
        bakeText(&textObj, FONT_SIZE_MED),
        bakeText(&textObj, FONT_SIZE_LARGE),
        bakeText(&textObj, FONT_SIZE_XL)
    };

    Rasteron_Image* finalImg = copyImgOp(textImgs[2]);
    
    for(unsigned t = 0; t < 5; t++) dealloc_image(textImgs[t]);
    dealloc_image(outerImg);

    return finalImg; 
}
