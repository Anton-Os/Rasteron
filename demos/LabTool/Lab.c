#define RASTERON_ENABLE_ANIM
#define RASTERON_ENABLE_FONT

#include "Rasteron.h"

static char fullFilePath[1024];

extern ColorSwatch _swatch;

/*** Rules ***/

unsigned addlineRules(unsigned color, unsigned neighbors[2]){ return (neighbors[0] == neighbors[1])? neighbors[0] + neighbors[1] : color; }

unsigned serpinskyRules(unsigned color, unsigned neighbors[2]){ 
    unsigned short s = 0;
    if(color == _swatch.colors[SWATCH_Green_Add] || color == _swatch.colors[SWATCH_Light]) s++;
    if(neighbors[0] == _swatch.colors[SWATCH_Green_Add] || neighbors[0] == _swatch.colors[SWATCH_Light]) s++;
    if(neighbors[1] == _swatch.colors[SWATCH_Green_Add] || neighbors[1] == _swatch.colors[SWATCH_Light]) s++;
    return (s == 1)? _swatch.colors[SWATCH_Green_Add] : _swatch.colors[SWATCH_Red_Add];
}

unsigned conwayRules(unsigned color, unsigned neighbors[8]){ 
    unsigned short lives = neighbor_count(_swatch.colors[SWATCH_Green_Add], neighbors) + neighbor_count(_swatch.colors[SWATCH_Light], neighbors); // countLives(neighbors);
    unsigned short kills = neighbor_count(_swatch.colors[SWATCH_Red_Add], neighbors) + neighbor_count(_swatch.colors[SWATCH_Dark], neighbors); // countKills(neighbors);
    // printf("Lives is %d, %d", lives, kills);

    if((color == _swatch.colors[SWATCH_Green_Add] || color == _swatch.colors[SWATCH_Light]) && lives < 2) return _swatch.colors[SWATCH_Red_Add]; // Any live cell with fewer than two live neighbors dies, as if by underpopulation.
    else if((color == _swatch.colors[SWATCH_Green_Add] || color == _swatch.colors[SWATCH_Light]) && (lives == 2 || lives == 3)) return _swatch.colors[SWATCH_Green_Add]; // Any live cell with two or three live neighbors lives on to the next generation.
    else if((color == _swatch.colors[SWATCH_Green_Add] || color == _swatch.colors[SWATCH_Light]) && lives > 3) return _swatch.colors[SWATCH_Red_Add]; // Any live cell with more than three live neighbors dies, as if by overpopulation.
    else if((color == _swatch.colors[SWATCH_Red_Add] || color == _swatch.colors[SWATCH_Dark]) && lives == 3) return _swatch.colors[SWATCH_Light]; // Any kills cell with exactly three live neighbors becomes a live cell, as if by reproduction.
    else if(color == _swatch.colors[SWATCH_Red_Add]) return _swatch.colors[SWATCH_Dark];
    else return color; 
}

unsigned randWalkRules(unsigned color, unsigned neighbors[8]){
    unsigned short direction = 0;

    if(color == _swatch.colors[SWATCH_Light]) return _swatch.colors[SWATCH_Green_Add];
    else if(color == _swatch.colors[SWATCH_Red_Add]) return _swatch.colors[SWATCH_Dark];
    else if(neighbors[direction] == _swatch.colors[SWATCH_Light] || neighbors[direction] == _swatch.colors[SWATCH_Green_Add]){
        direction = rand() % 8; // randomize walk direction
        return _swatch.colors[SWATCH_Light];
    }
    else if(neighbors[direction] == _swatch.colors[SWATCH_Dark] || neighbors[direction] == _swatch.colors[SWATCH_Red_Add]){
        direction = rand() % 8; // randomize walk direction
        return _swatch.colors[SWATCH_Red_Add];
    }
    else return color;
}

unsigned amplifyRules(unsigned color, unsigned neighbors[8]){ 
    unsigned short lives = neighbor_count(_swatch.colors[SWATCH_Green_Add], neighbors) + neighbor_count(_swatch.colors[SWATCH_Light], neighbors); // countLives(neighbors);
    unsigned short kills = neighbor_count(_swatch.colors[SWATCH_Red_Add], neighbors) + neighbor_count(_swatch.colors[SWATCH_Dark], neighbors); // countKills(neighbors);
    // printf("Lives is %d, %d", lives, kills);

    if(lives > kills) return (color == _swatch.colors[SWATCH_Green_Add] || color == _swatch.colors[SWATCH_Light])? _swatch.colors[SWATCH_Green_Add] : _swatch.colors[SWATCH_Light]; // color_level(color, 0.5 + (lives * (0.5 / 8.0)));
    else if(kills > lives) return (color == _swatch.colors[SWATCH_Red_Add] || color == _swatch.colors[SWATCH_Dark])? _swatch.colors[SWATCH_Dark] : _swatch.colors[SWATCH_Red_Add]; // color_level(color, 0.5 - (lives * (0.5 / 8.0)));
    else return color;
}

unsigned recursiveRules(unsigned color, unsigned neighbors[8]){
    color += (neighbors[NEBR_Left] > neighbors[NEBR_Right])? 16 : -16;
    color += (neighbors[NEBR_Top] > neighbors[NEBR_Bot])? 16 : -16;
    color += (neighbors[NEBR_Top_Left] + neighbors[NEBR_Top_Right] > neighbors[NEBR_Bot_Left] + neighbors[NEBR_Bot_Right])? 32 : -32;
    return color;
}

unsigned bloomRules(unsigned color, unsigned neighbors[8]){
    if(neighbors[NEBR_Bot] == _swatch.colors[SWATCH_Green_Add] || neighbors[NEBR_Top] == _swatch.colors[SWATCH_Green_Add] || neighbors[NEBR_Left] == _swatch.colors[SWATCH_Green_Add] || neighbors[NEBR_Right] == _swatch.colors[SWATCH_Green_Add])
        return _swatch.colors[SWATCH_Red_Add];
    else if(neighbors[NEBR_Bot_Left] == _swatch.colors[SWATCH_Red_Add] || neighbors[NEBR_Top_Right] == _swatch.colors[SWATCH_Red_Add] || neighbors[NEBR_Top_Left] == _swatch.colors[SWATCH_Red_Add] || neighbors[NEBR_Bot_Right] == _swatch.colors[SWATCH_Red_Add])
        return _swatch.colors[SWATCH_Green_Add];
    else return color;
}

unsigned unbloomRules(unsigned color, unsigned neighbors[8]){
    if(neighbors[NEBR_Bot_Left] == _swatch.colors[SWATCH_Red_Add] || neighbors[NEBR_Top_Right] == _swatch.colors[SWATCH_Red_Add] || neighbors[NEBR_Top_Left] == _swatch.colors[SWATCH_Red_Add] || neighbors[NEBR_Bot_Right] == _swatch.colors[SWATCH_Red_Add])
        return _swatch.colors[SWATCH_Green_Add];
    else if(neighbors[NEBR_Bot] == _swatch.colors[SWATCH_Green_Add] || neighbors[NEBR_Top] == _swatch.colors[SWATCH_Green_Add] || neighbors[NEBR_Left] == _swatch.colors[SWATCH_Green_Add] || neighbors[NEBR_Right] == _swatch.colors[SWATCH_Green_Add])
        return _swatch.colors[SWATCH_Red_Add];
    else return color;
}

unsigned levelRules(unsigned color, unsigned neighbors[8]){
    unsigned short lives = neighbor_count(_swatch.colors[SWATCH_Green_Add], neighbors) + neighbor_count(_swatch.colors[SWATCH_Light], neighbors); // countLives(neighbors);
    unsigned short kills = neighbor_count(_swatch.colors[SWATCH_Red_Add], neighbors) + neighbor_count(_swatch.colors[SWATCH_Dark], neighbors);
    unsigned short diff = abs((short)lives - (short)kills);

    if(diff == 0) return color;
    else if(diff % 2 == 1) return (diff == 1)? _swatch.colors[SWATCH_Red_Add] : _swatch.colors[SWATCH_Light];
    else return (diff == 2)? _swatch.colors[SWATCH_Green_Add] : _swatch.colors[SWATCH_Dark];
}

unsigned matchRules(unsigned color, unsigned neighbors[8]){
    if(neighbors[NEBR_Left] == neighbors[NEBR_Right]) return neighbors[NEBR_Left];
    else if(neighbors[NEBR_Top] == neighbors[NEBR_Bot]) return neighbors[NEBR_Top];
    else if(neighbors[NEBR_Top_Left] == neighbors[NEBR_Bot_Right]) return neighbors[NEBR_Bot_Right];
    else if(neighbors[NEBR_Bot_Left] == neighbors[NEBR_Top_Right]) return neighbors[NEBR_Top_Right];
    else return neighbors[rand() % 8];
}

unsigned colorizeRules(unsigned color, unsigned neighbors[8]){
    if(color == _swatch.colors[SWATCH_Light] || color == _swatch.colors[SWATCH_Green_Add]) return blend_colors(color, _swatch.colors[SWATCH_Green_Add], (float)rand() / RAND_MAX);
    else if(color == _swatch.colors[SWATCH_Dark] || color == _swatch.colors[SWATCH_Red_Add]) return blend_colors(color, _swatch.colors[SWATCH_Red_Add], (float)rand() / RAND_MAX);
    else return color;
}

unsigned scatterRules(unsigned color, unsigned neighbors[8]){
    if(neighbors[NEBR_Left] == neighbors[NEBR_Right] && neighbors[NEBR_Top] == neighbors[NEBR_Bot]) return _swatch.colors[rand() % 8];
    else return color;
}

unsigned flipRules(unsigned color, unsigned neighbors[8]){
    if(color == _swatch.colors[SWATCH_Green_Add] || color == _swatch.colors[SWATCH_Light]){
        unsigned lifeCount = 0;
        for(unsigned d = 0; d < 8; d++) 
            if(neighbors[d] == _swatch.colors[SWATCH_Green_Add] || neighbors[d] == _swatch.colors[SWATCH_Light]) lifeCount++;
        return (lifeCount % 2 == 0)? _swatch.colors[SWATCH_Red_Add] : _swatch.colors[SWATCH_Dark];
    } else if(color == _swatch.colors[SWATCH_Red_Add] || color == _swatch.colors[SWATCH_Dark]){
        unsigned deadCount = 0;
        for(unsigned d = 0; d < 8; d++) 
            if(neighbors[d] == _swatch.colors[SWATCH_Red_Add] || neighbors[d] == _swatch.colors[SWATCH_Dark]) deadCount++;
        return (deadCount % 2 == 0)? _swatch.colors[SWATCH_Green_Add] : _swatch.colors[SWATCH_Light];
    }
    return color;
}



/*** Experimental Image Algorithms ***/

Rasteron_Image* oragamiImgOp(enum FLIP_Type flip, double xCrop, double yCrop){ 
    genFullFilePath("Logroller.bmp", fullFilePath);

    Rasteron_Image* loadedImg = loadImgOp(fullFilePath);

    enum SIDE_Type cropX = (xCrop > 0.0)? SIDE_Right : SIDE_Left;
    Rasteron_Image* cropImgX = cropImgOp(loadedImg, cropX, xCrop);
    enum SIDE_Type cropY = (yCrop > 0.0)? SIDE_Top : SIDE_Bottom;
    Rasteron_Image* cropImgY = cropImgOp(cropImgX, cropY, yCrop);

    Rasteron_Image* flipImg = flipImgOp(cropImgY, flip);

    Rasteron_Image* finalImg = cornerImgOp(flipImg, 1.0, 0.75, 0.5, 0.25);

    RASTERON_DEALLOC(loadedImg);
    RASTERON_DEALLOC(cropImgX); RASTERON_DEALLOC(cropImgY);
    RASTERON_DEALLOC(flipImg);

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

    for(unsigned i = 0; i < 9; i++) RASTERON_DEALLOC(nestedImgs[i]);
    for(unsigned i = 0; i < 8; i++) RASTERON_DEALLOC(combineImgs[i]);  
    RASTERON_DEALLOC(outerImg);

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

Rasteron_Image* lensesImgOp(int channel){
    genFullFilePath("Gumdrops.bmp", fullFilePath);

    Rasteron_Image* loadedImg = loadImgOp(fullFilePath);
    Rasteron_Image* flipImg = (channel >= 0 && channel <= 2) 
        ? filterImgOp(loadedImg, channel) 
        : recolorImgOp(loadedImg, distill);

    RASTERON_DEALLOC(loadedImg);

    return flipImg; 
}

static unsigned hypnoticMix(unsigned color1, unsigned color2){
    // return mult_colors(color1, color2);
    return root_colors(color1, color2);
}

Rasteron_Image* hypnosisImgOp(unsigned pArg, unsigned color1, unsigned color2){ 
    // Rasteron_Image* gradientImg1 = gradientImgOp((ImageSize){ 1024, 1024 }, SIDE_Left, color1, color2);

    Rasteron_Image* gradientImgs[5]= { 
        gradientImgOp((ImageSize){ 1024, 1024 }, SIDE_Left, color1, color2),
        gradientImgOp((ImageSize){ 1024, 1024 }, SIDE_Right, color1, color2),
        gradientImgOp((ImageSize){ 1024, 1024 }, SIDE_Top, color1, color2),
        gradientImgOp((ImageSize){ 1024, 1024 }, SIDE_Bottom, color1, color2),
        gradientImgOp((ImageSize){ 1024, 1024 }, SIDE_Radial, color1, color2),
    };

    Rasteron_Image* mixImg1 = mixingImgOp(gradientImgs[0], gradientImgs[1], hypnoticMix);
    Rasteron_Image* mixImg2 = mixingImgOp(gradientImgs[2], gradientImgs[0], hypnoticMix);
    Rasteron_Image* mixImg3 = mixingImgOp(gradientImgs[1], gradientImgs[3], hypnoticMix);

    // Rasteron_Image* hypnosisImg = mixingImgOp(gradientImgs[0], gradientImgs[1], hypnoticMix);
    Rasteron_Image* hypnosisImg; // = mixingImgOp(mixImg3, gradientImgs[4], hypnoticMix);
    switch(pArg){
        case 0: hypnosisImg = mixingImgOp(mixImg1, gradientImgs[4], hypnoticMix); break;
        case 1: hypnosisImg = mixingImgOp(mixImg2, gradientImgs[4], hypnoticMix); break;
        case 2: hypnosisImg = mixingImgOp(mixImg3, gradientImgs[4], hypnoticMix); break;
        default: hypnosisImg = mixingImgOp(gradientImgs[4], gradientImgs[4], hypnoticMix); break;
    }
    
    for(unsigned g = 0; g < 5; g++) RASTERON_DEALLOC(gradientImgs[g]);
    RASTERON_DEALLOC(mixImg1); RASTERON_DEALLOC(mixImg2); RASTERON_DEALLOC(mixImg3);

    // RASTERON_DEALLOC(gradientImg1);

    return hypnosisImg; 
} 

static unsigned fisheye(unsigned color, double distance, PixelPoint pixPoint){
    return (distance < 0.15)? color_level(color, 0.15) : (distance < 0.3)? color : color_level(color, 0.85);
}

Rasteron_Image* fisheyeImgOp(unsigned short points){ 
    ColorPointTable colorPointTable;
    colorPointTable.pointCount = points;

    for(unsigned p = 0; p < points; p++){
        colorPointTable.points[p] = (ColorPoint){
            (double)rand() / RAND_MAX,
            (double)rand() / RAND_MAX,
            RAND_COLOR()
        };
    }

    return fieldImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, fisheye); 
}

Rasteron_Image* typographyImgOp(unsigned bgColor, unsigned textColor){ 
    genFullFilePath("Tw-Cen-MT.ttf", fullFilePath);

    /* Rasteron_Text textObj;
    textObj.fontFile = fullFilePath;
    textObj.text = "Howdy";
    textObj.bkColor = 0xFF000000;
    textObj.fgColor = 0xFFEEEEEE;

    unsigned short padding[4] = { 100, 0, 0, 100 };
    Rasteron_Image* textPadImg = textPadImgOp(&textObj, FONT_SIZE_LARGE, padding);
    RASTERON_DEALLOC(textPadImg); */

    Rasteron_Message messageObj;
    messageObj.fontFile = fullFilePath;
    messageObj.alignment = TEXT_ALIGN_LEFT;
    messageObj.messageCount = 4;
    messageObj.messages[0] = "Hello World";
    messageObj.messages[1] = "Ive come to say";
    messageObj.messages[2] = "Have a lovely day";
    messageObj.messages[3] = "And a good night";
    messageObj.bkColor = bgColor;
    messageObj.fgColor = textColor;

    // Rasteron_Image* textImg = textImgOp(&textObj, FONT_SIZE_TINY);
    Rasteron_Image* messageImg = messageImgOp(&messageObj, FONT_SIZE_LARGE);
    Rasteron_Image* finalImg = antialiasImgOp(messageImg, 1);

    // RASTERON_DEALLOC(textImg);
    RASTERON_DEALLOC(messageImg);

    return finalImg; 
}

static double zig = 10.0;
static double zag = 10.0;

static unsigned zigzag(double x, double y){
    x *= zig; y *= zag; // split into 10 discrete sections
    double xDiff = ((x / zig) > 0.5)? x - ((unsigned)x) : -x - ((unsigned)x);
    double yDiff = ((y / zag) > 0.5)? y - ((unsigned)y) : -y - ((unsigned)y);

    xDiff *= xDiff / (1.0 - y) / y;
    yDiff *= yDiff / (1.0 - x) / x;

    if(xDiff > yDiff) return blend_colors(0xFFFF00FF, 0xFFEEEEEE, xDiff - yDiff);
    else return blend_colors(0xFF00FF00, 0xFF111111, yDiff - xDiff);
}

Rasteron_Image* mozaicImgOp(double z1, double z2){
    zig = z1; zag = z2;
    Rasteron_Image* zigzagImg = mapImgOp((ImageSize){ 1024, 1024, }, zigzag);

    return zigzagImg;
}

#define INTERFERE_THRESH 0xAA
unsigned short interfere_inc = 16;
unsigned short interfere_dec = 16;

static unsigned interference(unsigned color, unsigned neighbors[2]){
    if((neighbors[0] & 0xFF0000 >> 16) + (neighbors[1] & 0xFF00 >> 8) < INTERFERE_THRESH) return color + interfere_inc;
    else return color - interfere_dec;
}

Rasteron_Image* interferenceImgOp(unsigned short inc, unsigned short dec){
    interfere_inc = inc;
    interfere_dec = dec;

    Rasteron_Image* patternImg = noiseImgOp_white((ImageSize){ 1024, 1024 }, 0xFF000000, 0xFFEEEE00); // solidImgOp((ImageSize){ 1024, 1024 }, RAND_COLOR());

    Rasteron_Image* interferenceImg1 = cellwiseRowImgOp(patternImg, interference);
    Rasteron_Image* interferenceImg2 = cellwiseColImgOp(interferenceImg1, interference);

    RASTERON_DEALLOC(interferenceImg1);
    RASTERON_DEALLOC(patternImg);

    return interferenceImg2;
}

static unsigned perturb(double x, double y){
    unsigned perturbColor1 = 0xFF00FF00;
    unsigned perturbColor2 = 0xFFFF0000;

    return blend_colors(0xFFFF0000, 0xFF00FF00, (x * 2) * (y * 2)) | 0x88;
    // return blend_colors(perturbColor1, perturbColor2, x * y) | 0x33;
}

Rasteron_Image* perturbImgOp(double xCenter, double yCenter){
    ColorGrid grid = { 9, 9, 0xFFFF0000, 0xFF00FF00 };

    Rasteron_Image* coordImg = mapImgOp((ImageSize){ 1024, 1024 }, perturb);
    Rasteron_Image* checkerImg = checkeredImgOp((ImageSize){ 1024, 1024 }, grid);

    Rasteron_Image* perturbImg = warpingImgOp(checkerImg, coordImg);

    RASTERON_DEALLOC(coordImg);    
    RASTERON_DEALLOC(checkerImg);

    return perturbImg;
}

Rasteron_Image* ballingImgOp(double size){
    double torusX = (double)rand() / RAND_MAX; double torusY = (double)rand() / RAND_MAX;
    double blobX1 = (double)rand() / RAND_MAX; double blobY1 = (double)rand() / RAND_MAX;
    double blobX2 = (double)rand() / RAND_MAX; double blobY2 = (double)rand() / RAND_MAX;

    Rasteron_Image* ballingImg = solidImgOp((ImageSize){ 1024, 1024 }, 0xFF333333);

    for(unsigned p = 0; p < 1024 * 1024; p++){
        double torusDist = pix_dist(p, pixPoint_offset((PixelPoint){ torusX, torusY }, ballingImg), 1024);
        double blobDist1 = pix_dist(p, pixPoint_offset((PixelPoint){ blobX1, blobY1 }, ballingImg), 1024);
        double blobDist2 = pix_dist(p, pixPoint_offset((PixelPoint){ blobX2, blobY2 }, ballingImg), 1024);

        if(sin(torusDist) * size < (300.0 * size)) 
            *(ballingImg->data + p) = blend_colors(0xFF333333, 0xFFEEEEEE, (torusDist - 150.0) / (300.00 * size));
        if(blobDist1 < (200.0 * size)) 
            *(ballingImg->data + p) = blend_colors(0xFF00EE00, 0xFF333333, (blobDist1 - 50.0) / (200.0 * size));
        if(blobDist2 < (100.0 * size)) 
            *(ballingImg->data + p) = blend_colors(0xFF0000EE, 0xFF333333, (blobDist2 - 50.0) / (100.0 * size));
    }

    return ballingImg;
}

Rasteron_Image* stratifyImgOp(unsigned short levels){
    genFullFilePath("Zero+.bmp", fullFilePath);

    Rasteron_Image* loadedImg = loadImgOp(fullFilePath);
    Rasteron_Image* resizeImg = resizeImgOp((ImageSize){ loadedImg->height / 2, loadedImg->width / 2 }, loadedImg);
    Rasteron_Image* stratifyImg = splitImgOp(resizeImg, levels);

    RASTERON_DEALLOC(loadedImg);
    RASTERON_DEALLOC(resizeImg);

    return stratifyImg;
}

unsigned barcode1;
unsigned barcode2;

unsigned grainPattern(unsigned color){ 
    if(color == barcode1) return ((double)rand() / (double)RAND_MAX < 0.1)? 0xFF333333 : barcode1; 
    else return ((double)rand() / (double)RAND_MAX < 0.1)? barcode2 : color;
}

unsigned bark(unsigned color, unsigned neighbors[2]){ return (rand() % 2 == 0)? neighbors[0] : neighbors[1]; }
// unsigned napkin(unsigned color, unsigned neighbors[2]){ return blend_colors(neighbors[0], neighbors[1], 0.5); }

Rasteron_Image* barkodeImgOp(unsigned short iters, unsigned color1, unsigned color2){
    barcode1 = color1; barcode2 = color2;

    Rasteron_Image* seedImg = solidImgOp((ImageSize){ 1024, 1024 }, color1);
    for(unsigned i = 0; i < iters; i++){
        Rasteron_Image* tempImg = copyImgOp(seedImg);
        RASTERON_DEALLOC(seedImg);
        seedImg = recolorImgOp(tempImg, grainPattern);
        RASTERON_DEALLOC(tempImg);
    }

    Rasteron_Image* finalImg = cellwiseRowImgOp(seedImg, bark);

    RASTERON_DEALLOC(seedImg);

    return finalImg;
}

Rasteron_Image* chaosImgOp(unsigned short rows, unsigned short cols){
    Rasteron_Image* img1 = gradientImgOp((ImageSize){ 1024 / rows, 1024 / cols }, SIDE_Bottom, 0xFFFF0000, 0xFF0000FF);
    Rasteron_Image* img2 = gradientImgOp((ImageSize){ 1024 / rows, 1024 / cols }, SIDE_Top, 0xFF00FFFF, 0xFFFFFF00);
    Rasteron_Image* img3 = gradientImgOp((ImageSize){ 1024 / rows, 1024 / cols }, SIDE_Right, 0xFF00FF00, 0xFFFF00FF);
    Rasteron_Image* img4 = gradientImgOp((ImageSize){ 1024 / rows, 1024 / cols }, SIDE_Radial, RAND_COLOR(), RAND_COLOR());
    // Rasteron_Image* img1 = noiseImgOp((ImageSize){ 1024 / rows, 1024 / cols }, (ColorGrid){ 0xFFFF0000, 0xFF0000FF, rows, cols });

    Rasteron_Image* chaosoImgs[4] = {
        flipImgOp(img1, FLIP_Counter), flipImgOp(img2, FLIP_Upside), flipImgOp(img3, FLIP_None), flipImgOp(img4, FLIP_Clock)
    };

    Rasteron_Image* chaosImg = RASTERON_ALLOC("chaos", 1024, 1024);

    for(unsigned p = 0; p < chaosImg->width * chaosImg->height; p++){
        double x = (1.0 / (double)1024) * (p % 1024);
		double y = (1.0 / (double)1024) * (p / 1024);

        unsigned c = x * rows;
        unsigned r = y * cols;

        *(chaosImg->data + p) = fuse_colors(*(chaosoImgs[c % 3]->data + (p % (1024 / rows))), *(chaosoImgs[r % 3]->data + (p % (1024 / rows))), atan(x + y));
        *(chaosImg->data + p) = diff_colors(*(chaosImg->data + p), *(chaosoImgs[(c + 3) % 3]->data + p));
    }

    for(unsigned i = 0; i < 4; i++) RASTERON_DEALLOC(chaosoImgs[i]);
    RASTERON_DEALLOC(img1);

    return chaosImg;
}

// Placeholder Images

Rasteron_Image* hourglassesImgOp(unsigned color1, unsigned color2){
    Rasteron_Image* paintImg = solidImgOp((ImageSize){ 1024, 1024 }, RAND_COLOR());

    int direction = 1; // positive

    unsigned color = 0xFF000000 | RAND_COLOR();

    for(unsigned p = 0; p < 1024 * 1024; p++){
        *(paintImg->data + p) = 0xFF000000 | color;

        if(direction == 1 && color > color1) direction = 0;
        else if(direction == 0 && color < color2) direction = 1;

        if(direction) color += rand() % 256;
        else color -= rand() % 256;
    }

    Rasteron_Image* mirrorImg = mirrorImgOp(paintImg);
    Rasteron_Image* experimentalImg = blendImgOp(paintImg, mirrorImg);

    return experimentalImg;
}

static unsigned ditherThresh;
static unsigned ditherColor1;
static unsigned ditherColor2;

unsigned dither(unsigned color, unsigned neighbors[8]){
    float probability = (float)rand() / (float)RAND_MAX;

    long addColor = color;
    for(unsigned n = 0; n < 8; n++) addColor += neighbors[n];

    unsigned avgColor = addColor / 9;
    unsigned avgChannel = ((avgColor & BLUE_CHANNEL) + ((avgColor & GREEN_CHANNEL) >> 8) + ((avgColor & RED_CHANNEL) >> 16)) / 3;
    
    return (avgChannel > ditherThresh)? ditherColor1 : ditherColor2;
}

Rasteron_Image* ditherImgOp(ref_image_t refImg, uint8_t thresh, unsigned color1, unsigned color2){
    ditherThresh = thresh;
    ditherColor1 = color1;
    ditherColor2 = color2;

    return cellwiseImgOp(refImg, dither);
}

ColorPointTable tilingTable;

static double euclidX = 0.01;
static double euclidY = 0.01;

static unsigned euclidTile(unsigned color, double distance, PixelPoint pixPoint){
    if((pixPoint.x < euclidX && pixPoint.x > -euclidX) || (pixPoint.y < euclidY && pixPoint.y > -euclidY))
        return color;
    else return color_invert(color);
}

Rasteron_Image* euclidTileImgOp(int mode, unsigned short density, double xFactor, double yFactor){
    euclidX = xFactor;
    euclidY = yFactor;

    tilingTable.pointCount = 0;

    for(unsigned r = 0; r < density + 1; r++){
        for(unsigned c = 0; c < density + 1; c++){
            switch(mode){
                case 0: // regular grid tiling
                    colorPointToTable(&tilingTable, (c % 2 == 0)? 0xFFFFFF00 : 0xFFFF00FF, (1.0 / density) * c, (1.0 / density) * r); 
                    break;
                case 1: // triangular tiling attempt
                    if((r % 2 == 0 && c % 2 == 0) || (r % 2 == 1 && c % 2 == 1))
                        colorPointToTable(&tilingTable, (c % 2 == 0)? 0xFFFFFF00 : 0xFFFF00FF, (1.0 / density) * c, (1.0 / density) * r); 
                    break;
                case 2: // hexagonal tiling attempt
                    if((r % 2 == 0 && c % 3 != 0) || (r % 2 == 1 && c % 3 == 0))
                        colorPointToTable(&tilingTable, (c % 2 == 0)? 0xFFFFFF00 : 0xFFFF00FF, (1.0 / density) * c, (1.0 / density) * r);
                    break;
                default: colorPointToTable(&tilingTable, (c % 2 == 0)? 0xFFFFFF00 : 0xFFFF00FF, (double)rand() / (double)RAND_MAX, (double)rand() / (double)RAND_MAX); // random
            }
        }
    }

    return fieldImgOp((ImageSize){ 1024, 1024 }, &tilingTable, euclidTile);
}

static unsigned nuTile(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]){
    // if((pixPoints[0].x < euclidX && pixPoints[1].x > -euclidX) || (pixPoints[0].y < euclidY && pixPoints[1].y > -euclidY))
    if((pixPoints[0].x < euclidX && pixPoints[2].x > -euclidX) || (pixPoints[0].y < euclidY && pixPoints[2].y > -euclidY))
    // if((pixPoints[1].x < euclidX && pixPoints[2].x > -euclidX) || (pixPoints[1].y < euclidY && pixPoints[2].y > -euclidY))
        return colors[0];
    else return color_invert(colors[0]);
}

Rasteron_Image* nuTileImgOp(int mode, unsigned short density, double xFactor, double yFactor){
    euclidX = xFactor;
    euclidY = yFactor;

    tilingTable.pointCount = 0;

    for(unsigned r = 0; r < density + 1; r++){
        for(unsigned c = 0; c < density + 1; c++){
            switch(mode){
                case TILE_Square: // regular grid tiling
                    colorPointToTable(&tilingTable, (c % 2 == 0)? 0xFFFFFF00 : 0xFFFF00FF, (1.0 / density) * c, (1.0 / density) * r); 
                    break;
                case TILE_Triangle: // triangular tiling attempt
                    if((r % 2 == 0 && c % 2 == 0) || (r % 2 == 1 && c % 2 == 1))
                        colorPointToTable(&tilingTable, (c % 2 == 0)? 0xFFFFFF00 : 0xFFFF00FF, (1.0 / density) * c, (1.0 / density) * r); 
                    break;
                case TILE_Hexagon: // hexagonal tiling attempt
                    if((r % 2 == 0 && c % 3 != 0) || (r % 2 == 1 && c % 3 == 0))
                        colorPointToTable(&tilingTable, (c % 2 == 0)? 0xFFFFFF00 : 0xFFFF00FF, (1.0 / density) * c, (1.0 / density) * r);
                    break;
                case TILE_Diagonal: // diagonal tiling attempt
                    if(c == r || c == density + 1 - r || r == density + 1 - c)
                        colorPointToTable(&tilingTable, (c % 2 == 0)? 0xFFFFFF00 : 0xFFFF00FF, (1.0 / density) * c, (1.0 / density) * r);
                    break;
                case TILE_Perimiter: // perimiter tiling attempt
                    if(c == 0 || c == density + 1 || r == 0 || r == density + 1)
                        colorPointToTable(&tilingTable, (c % 2 == 0)? 0xFFFFFF00 : 0xFFFF00FF, (1.0 / density) * c, (1.0 / density) * r);
                    break;
                default: colorPointToTable(&tilingTable, (c % 2 == 0)? 0xFFFFFF00 : 0xFFFF00FF, (double)rand() / (double)RAND_MAX, (double)rand() / (double)RAND_MAX); // random
            }
        }
    }

    return fieldExtImgOp((ImageSize){ 1024, 1024 }, &tilingTable, nuTile);
}

unsigned graterColor1 = 0xFF333333;
unsigned graterColor2 = 0xFFEEEEEE;

static unsigned grater(double x, double y){
    double largeX = x * 10.0; double smallX = largeX - floor(largeX);
    double largeY = y * 10.0; double smallY = largeY - floor(largeY);

    if(largeX / largeY > largeY / largeX) 
        return (pow(smallX, smallY) > 0.5)? blend_colors(graterColor1, graterColor2, fabs(smallX - smallY)) : graterColor2;
    else return (pow(smallX, smallY) < 0.5)? blend_colors(graterColor1, graterColor2, fabs(smallX - smallY)) : graterColor2;
}

Rasteron_Image* graterImgOp(unsigned color1, unsigned color2){
    graterColor1 = color1;
    graterColor2 = color2;

    return mapImgOp((ImageSize){ 1024, 1024 }, grater);
}

static unsigned fuzzlike(unsigned color, unsigned neighbors[8]){
    /* if(neighbors[NEBR_Bot] & 0xFF00 > color & 0xFF00) return neighbors[NEBR_Bot];
    else if(neighbors[NEBR_Right] & 0xFF > color & 0xFF) return neighbors[NEBR_Right];
    else if(neighbors[NEBR_Top] & 0xFF00 > color & 0xFF00) return neighbors[NEBR_Top];
    else if(neighbors[NEBR_Left] & 0xFF > color & 0xFF) return neighbors[NEBR_Left];
    else return color; */
    return neighbors[rand() % 8];
}

Rasteron_Image* fuzzlikeImgOp(unsigned short iters){
    genFullFilePath("Starcase.bmp", fullFilePath);

    Rasteron_Image* loadedImg = loadImgOp(fullFilePath);
    Rasteron_Image* fuzzlikeImg = cellwiseExtImgOp(loadedImg, fuzzlike, iters);

    RASTERON_DEALLOC(loadedImg);

    return fuzzlikeImg;
}

Rasteron_Image* oozelikeImgOp(unsigned short colorMode){
    Rasteron_Image* noiseImg = noiseImgOp_hi((ImageSize){ 1024, 1024 }, (ColorGrid){ 16, 16, 0xFF330088, 0xFF003388}, 1);

    Rasteron_Image* redNoiseImg = noiseImgOp_white((ImageSize){ 1024, 1024 }, 0xFF11EE88, 0xFFEE0088);
    Rasteron_Image* greenNoiseImg = noiseImgOp_white((ImageSize){ 1024, 1024 }, 0xFF001188, 0xFF00EE88);
    Rasteron_Image* blueNoiseImg = noiseImgOp_white((ImageSize){ 1024, 1024 }, 0xFF443311, 0xFF9900EE);

    Rasteron_Image* perturbImg1 = warpingImgOp(redNoiseImg, noiseImg);
    Rasteron_Image* perturbImg2 = warpingImgOp(blueNoiseImg, noiseImg);
    Rasteron_Image* perturbBlendImg = blendImgOp(perturbImg1, perturbImg2);

    Rasteron_Image* warpingImg = warpingImgOp(perturbBlendImg, noiseImg); // cellwiseExtImgOp(noiseImg, hatch, 6);
    Rasteron_Image* oozelikeImg = antialiasImgOp(warpingImg, 1);

    RASTERON_DEALLOC(noiseImg);
    RASTERON_DEALLOC(redNoiseImg); RASTERON_DEALLOC(greenNoiseImg); RASTERON_DEALLOC(blueNoiseImg);
    RASTERON_DEALLOC(perturbImg1); RASTERON_DEALLOC(perturbImg2); RASTERON_DEALLOC(perturbBlendImg);
    RASTERON_DEALLOC(warpingImg);

    Rasteron_Image* swapImg;
    switch(colorMode){
        case 0: 
            swapImg = colorSwitchImgOp(oozelikeImg, CHANNEL_Blue, CHANNEL_Green);
            RASTERON_DEALLOC(oozelikeImg);
            return swapImg;
        case 1: 
            swapImg = colorSwitchImgOp(oozelikeImg, CHANNEL_Blue, CHANNEL_Red);
            RASTERON_DEALLOC(oozelikeImg);
            return swapImg;
        case 2: 
            swapImg = colorSwitchImgOp(oozelikeImg, CHANNEL_Red, CHANNEL_Green);
            RASTERON_DEALLOC(oozelikeImg);
            return swapImg;
        default: return oozelikeImg;
    }
}

unsigned recurrantMix(unsigned color1, unsigned color2){
    if(color2 % 10 == 0 || color2 % 10 == 1 || color2 % 10 == 2) return color1 + 0xF;
    else if(color2 % 10 == 3 || color2 % 10 == 4 || color2 % 5 == 7) return color1 + 0xF00;
    else if(color2 % 10 == 6 || color2 % 10 == 7 || color2 % 6 == 8) return color1 + 0xF0000;
    else return 0xFF333333;
}

Rasteron_Image* recurrantImgOp(unsigned short iters){
    ColorGrid colorGrid = { 2, 2, 0xFF333333, 0xFFEEEEEE };

    Rasteron_Image* noiseImg = noiseImgOp_octave((ImageSize){ 1024, 1024 }, colorGrid, iters);
    Rasteron_Image* recurrantImg = noiseImgOp((ImageSize){ 1024, 1024 }, colorGrid);

    for(unsigned i = 0; i < iters; i++){
        Rasteron_Image* tempImg = mixingImgOp(noiseImg, recurrantImg, recurrantMix);
        RASTERON_DEALLOC(recurrantImg);
        recurrantImg = copyImgOp(tempImg);
        RASTERON_DEALLOC(tempImg);
    }

    RASTERON_DEALLOC(noiseImg);

    return recurrantImg;
}

double intersectAngle = 0.0;
double intersectThresh = 0.1;

unsigned intersect(double x, double y){
    double xTen = x * 10;
    double xDiv = xTen - floor(xTen);
    double yTen = y * 10;
    double yDiv = yTen - floor(yTen);

    // if((y * intersectAngle) / x > -intersectThresh && (y * intersectAngle) / x < intersectThresh) return 0xFF00FF00;
    // if(x / y > 1.0 - intersectThresh && x / y < 1.0 + intersectThresh) return 0xFF00FF00;
    // if(y > 0.5) return 0xFF00FF00;
    if(abs(intersectAngle) == 1.0 && xDiv < intersectThresh) return 0xFF00FF00;
    else if(abs(intersectAngle) == 0.0 && yDiv < intersectThresh) return 0xFF00FF00;
    // else if((xDiv / intersectAngle) / (yDiv * intersectAngle) < intersectThresh) return 0xFF00FF00;
    else return 0xFFFF00FF;
}

unsigned intersectMix(unsigned color1, unsigned color2){
    if(color1 == 0xFF00FF00 && color2 == 0xFF00FF00) return 0xFFFFFF00;
    else if(color1 == 0xFFFF00FF && color2 == 0xFFFF00FF) return 0xFF00FFFF;
    else if(color1 == 0xFF00FF00 && color2 == 0xFFFF00FF) return 0xFF00FF00;
    else return 0xFFFF00FF;
}

Rasteron_Image* intersectImgOp(double angle1, double angle2){
    intersectAngle = angle1;
    Rasteron_Image* intersectImg1 = mapImgOp((ImageSize){ 1024, 1024 }, intersect);
    intersectAngle = angle2;
    Rasteron_Image* intersectImg2 = mapImgOp((ImageSize){ 1024, 1024 }, intersect);

    Rasteron_Image* scripticImg = mixingImgOp(intersectImg1, intersectImg2, intersectMix);

    RASTERON_DEALLOC(intersectImg1);
    RASTERON_DEALLOC(intersectImg2);

    return scripticImg;
}

double swirl = 10.0;

unsigned swirly(double x, double y){
    double centerAngle = atan((y - 0.5) / (x - 0.5));
    double centerDist = sqrt(pow(x - 0.5, 2) + pow(y - 0.5, 2));

    if((centerDist * swirl) - (floor(centerDist * swirl)) > (centerAngle * swirl) - (floor(centerAngle * swirl))) return 0xFF333333;
    else return 0xFFEEEEEE;
}

Rasteron_Image* swirlyImgOp(double swirlFactor){
    swirl = swirlFactor;
    return mapImgOp((ImageSize){ 1024, 1024 }, swirly);
}

Rasteron_Image* displacerImgOp(unsigned short iters, unsigned color1, unsigned color2){
    Rasteron_Image* noiseTileImg1 = noiseImgOp((ImageSize){ 1024, 1024 }, (ColorGrid){ iters * 5, iters, color1, color2 });
    Rasteron_Image* noiseTileImg2 = noiseImgOp((ImageSize){ 1024, 1024 }, (ColorGrid){ iters, iters * 5, color1, color2 });
    Rasteron_Image* gradientImg1 = gradientImgOp((ImageSize){ 1024, 1024 }, SIDE_Left, color1, color2);
    Rasteron_Image* gradientImg2 = gradientImgOp((ImageSize){ 1024, 1024 }, SIDE_Right, color_invert(color1), color_invert(color2));
    // Rasteron_Image* warpTileImg = noiseImgOp((ImageSize){ 1024, 1024 }, (ColorGrid){ cells, cells, 0xFF666666, 0xFFAAAAAA });

    Rasteron_Image* displacerImg = warpingImgOp(gradientImg1, noiseTileImg2);
    for(int i = 0; i < iters; i++){
        Rasteron_Image* tempImg = copyImgOp(displacerImg);
        RASTERON_DEALLOC(displacerImg);
        displacerImg = warpingImgOp(tempImg, (i % 2 == 0)? noiseTileImg1 : noiseTileImg2);
        RASTERON_DEALLOC(tempImg);
    }

    RASTERON_DEALLOC(noiseTileImg1); RASTERON_DEALLOC(noiseTileImg2);
    RASTERON_DEALLOC(gradientImg1); RASTERON_DEALLOC(gradientImg2); // RASTERON_DEALLOC(solidImg1); RASTERON_DEALLOC(solidImg2);
    // RASTERON_DEALLOC(warpTileImg);

    return displacerImg;
}

unsigned biline(unsigned color, unsigned neighbors[2]){ return (neighbors[0] < color)? neighbors[1] : neighbors[0]; }

Rasteron_Image* bilineImgOp(unsigned color, unsigned short variant){
    unsigned colorLine[1024];

    for(unsigned p = 0; p < 1024; p++) colorLine[p] = blend_colors(color, color_invert(color), (p % 256) / 256.0); // color + (rand() % variant) - (variant / 2);

    Rasteron_Image* lineImg = RASTERON_ALLOC("biline", 1024, 1024);
    for(unsigned l = 0; l < 1024; l++){
        int v = (rand() % variant) - (variant / 2);
        for(unsigned p = 0; p < 1024; p++) *(lineImg->data + (l * 1024) + p) = colorLine[(p + abs(v)) % 1024];
    }

    Rasteron_Image* bilineImg = cellwiseColImgOp(lineImg, biline);

    RASTERON_DEALLOC(lineImg);

    return bilineImg;
}

ColorPointTable arcTable;
unsigned arcDefColor = 0xFF333333;

Rasteron_Image* arcaneImgOp(double radius, unsigned short count){
    arcTable.pointCount = 0;
    // arcCount = count;

    for(unsigned a = 0; a < count; a++)
       colorPointToTable(&arcTable, RAND_COLOR(), (double)rand() / (double)RAND_MAX, (double)rand() / (double)RAND_MAX);

    Rasteron_Image* arcImg = RASTERON_ALLOC("arc", 1024, 1024);
    for(unsigned a = 0; a < count; a++)
        for(unsigned p = 0; p < 1024 * 1024; p++){
            double x = (1.0 / (double)1024) * (p % 1024);
		    double y = (1.0 / (double)1024) * (p / 1024);

            double dist = pix_dist(p, pixPoint_offset((PixelPoint){ arcTable.points[a].x, arcTable.points[a].y}, arcImg), 1024);
            if(dist < radius * sin(x * radius) * cos(y * radius)){
                if(a == 0) *(arcImg->data + p) = arcTable.points[a].color;
                else if(*(arcImg->data + p) == arcDefColor) *(arcImg->data + p) = arcTable.points[a].color;
                else *(arcImg->data + p) = blend_colors(*(arcImg->data + p), arcTable.points[a].color, 0.5);
            } 
            else if(a == 0) *(arcImg->data + p) = arcDefColor;
        }

    return arcImg;
}