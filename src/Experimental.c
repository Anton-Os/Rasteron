#include "Experimental.h"

static char fullFilePath[1024];

Rasteron_Image* oragamiImgOp(enum FLIP_Type flip, double xCrop, double yCrop){ 
    genFullFilePath("Logroller.bmp", &fullFilePath);

    Rasteron_Image* loadedImg = loadImgOp(fullFilePath);

    enum CROP_Type cropX = (xCrop > 0.0)? CROP_Right : CROP_Left;
    Rasteron_Image* cropImgX = cropImgOp(loadedImg, cropX, xCrop);
    enum CROP_Type cropY = (yCrop > 0.0)? CROP_Top : CROP_Bottom;
    Rasteron_Image* cropImgY = cropImgOp(cropImgX, cropY, yCrop);

    Rasteron_Image* flipImg = flipImgOp(cropImgY, flip);

    Rasteron_Image* finalImg = resizeImgOp((ImageSize){ 512, 512 }, flipImg); // attempting to resize

    dealloc_image(loadedImg);
    dealloc_image(cropImgX, cropImgY);
    dealloc_image(flipImg);

    return finalImg;
}

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
    Rasteron_Image* flipImg = insertImgOp(combineImgs[7], outerImg, x, y);

    for(unsigned i = 0; i < 9; i++) dealloc_image(nestedImgs[i]);
    for(unsigned i = 0; i < 8; i++) dealloc_image(combineImgs[i]);  
    dealloc_image(outerImg);

    return flipImg;
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
    Rasteron_Image* flipImg = (channel >= 0 && channel <= 2) 
        ? filterImgOp(loadedImg, channel) 
        : recolorImgOp(loadedImg, distill);

    dealloc_image(loadedImg);

    return flipImg; 
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

    // Rasteron_Image* flipImg = mixingImgOp(gradientImgs[0], gradientImgs[1], overlayer);
    Rasteron_Image* flipImg; // = mixingImgOp(mixImg3, gradientImgs[4], overlayer);
    switch(pArg){
        case 0: flipImg = copyImgOp(mixImg1); break;
        case 1: flipImg = copyImgOp(mixImg2); break;
        case 2: flipImg = copyImgOp(mixImg3); break;
        default: flipImg = mixingImgOp(mixImg2, gradientImgs[4], overlayer); break;
    }
    
    for(unsigned g = 0; g < 5; g++) dealloc_image(gradientImgs[g]);
    dealloc_image(mixImg1); dealloc_image(mixImg2); dealloc_image(mixImg3);

    return flipImg; 
} 

Rasteron_Image* multiNoiseImgOp(int noiseOp){ 
    ColorGrid grid1 = (ColorGrid){ 8, 8, 0xFFFFFF00, 0xFFFFFFFF };
    ColorGrid grid2 = (ColorGrid){ 32, 32, 0xFFFF00FF, 0xFF888888 };
    ColorGrid grid3 = (ColorGrid){ 128, 128, 0xFF00FFFF, 0xFF000000 };

    ColorGridTable gridTable;
    gridTable.gridCount = 3;
    gridTable.grids[0] = grid1;
    gridTable.grids[1] = grid2;
    gridTable.grids[2] = grid3;

    Rasteron_Image* noiseImg1 = noiseImgOp_grid((ImageSize){ 1024, 1024}, grid1);
    Rasteron_Image* noiseImg2 = noiseImgOp_grid((ImageSize){ 1024, 1024}, grid2);
    Rasteron_Image* noiseImg3 = noiseImgOp_grid((ImageSize){ 1024, 1024}, grid3);

    Rasteron_Image* blendImg = (noiseOp % 2 == 0)? blendImgOp(noiseImg1, noiseImg2) : blendImgOp(noiseImg3, noiseImg2);
    Rasteron_Image* finalImg;

    switch(noiseOp){
        case 0: finalImg = blendImgOp(blendImg, noiseImg3); break;
        case 1: finalImg = blendImgOp(blendImg, noiseImg1); break;
        case 2: finalImg = fusionImgOp(noiseImg1, noiseImg3); break;
        default: finalImg = fusionImgOp(blendImg, blendImg); break;
    }

    dealloc_image(noiseImg1); dealloc_image(noiseImg2); dealloc_image(noiseImg3);
    dealloc_image(blendImg);

    return finalImg;
}

Rasteron_Image* cellAutomataImgOp(int seedOp){
    genFullFilePath("Starcase.bmp", &fullFilePath);

    ColorPointTable table;
    table.pointCount = 0;
    for(unsigned p = 0; p < MAX_PIXELPOINTS; p++)
        colorPointToTable(&table, RAND_COLOR(), rand() / (double)RAND_MAX, rand() / (double)RAND_MAX);

    Rasteron_Image* loadedImg = loadImgOp(fullFilePath);
    Rasteron_Image* seedImg;

    seedImg = seededImgOp(loadedImg, &table);
 
    dealloc_image(loadedImg);
    return seedImg; 
}

void proxPattern(unsigned color, double distance){ return color; }

Rasteron_Image* proxPatternImgOp(unsigned short points){ 
    ColorPointTable colorPointTable;
    colorPointTable.pointCount = points;
    for(unsigned p = 0; p < points; p++){
        colorPointTable.points[p] = (ColorPoint){
            (double)rand() / RAND_MAX,
            (double)rand() / RAND_MAX,
            RAND_COLOR()
        };
    }

    return fieldImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, proxPattern); 
}

Rasteron_Image* wordsmithImgOp(const char* text){ 
    genFullFilePath("Tw-Cen-MT.ttf", &fullFilePath);

    Rasteron_Text textObj;
    textObj.fontFile = fullFilePath;
    textObj.text = text;
    textObj.bkColor = 0xFF000000;
    textObj.fgColor = 0xFFEEEEEE;

    Rasteron_Message messageObj;
    messageObj.fontFile = fullFilePath;
    messageObj.messageCount = 3;
    messageObj.messages[0] = "Rasteron is Dope!";
    messageObj.messages[1] = "Rasteron is Dope!!";
    messageObj.messages[2] = "Rasteron is Dope!!!";
    messageObj.bkColor = 0xFF000000;
    messageObj.fgColor = 0xFFEEEEEE;

    Rasteron_Image* textImg = textImgOp(&textObj, FONT_SIZE_MED);
    Rasteron_Image* finalImg = antialiasImgOp(textImg);

    dealloc_image(textImg);
    return finalImg; 
}
