#include "Experimental.h"

static char fullFilePath[1024];

Rasteron_Image* oragamiImgOp(enum FLIP_Type flip, double xCrop, double yCrop){ 
    genFullFilePath("Logroller.bmp", &fullFilePath);

    Rasteron_Image* loadedImg = loadImgOp(fullFilePath);

    enum SIDE_Type cropX = (xCrop > 0.0)? SIDE_Right : SIDE_Left;
    Rasteron_Image* cropImgX = cropImgOp(loadedImg, cropX, xCrop);
    enum SIDE_Type cropY = (yCrop > 0.0)? SIDE_Top : SIDE_Bottom;
    Rasteron_Image* cropImgY = cropImgOp(cropImgX, cropY, yCrop);

    Rasteron_Image* flipImg = flipImgOp(cropImgY, flip);

    Rasteron_Image* finalImg = resizeImgOp((ImageSize){ 512, 512 }, flipImg); // attempting to resize

    RASTERON_DEALLOC(loadedImg);
    RASTERON_DEALLOC(cropImgX, cropImgY);
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

Rasteron_Image* lensesImgOp(enum CHANNEL_Type channel){ 
    genFullFilePath("Gumdrops.bmp", &fullFilePath);

    Rasteron_Image* loadedImg = loadImgOp(fullFilePath);
    Rasteron_Image* flipImg = (channel >= 0 && channel <= 2) 
        ? filterImgOp(loadedImg, channel) 
        : recolorImgOp(loadedImg, distill);

    RASTERON_DEALLOC(loadedImg);

    return flipImg; 
}

static unsigned gradientMix(unsigned color1, unsigned color2){
    return color1 * color2; // pretty cool effect by multiplying colors
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

    Rasteron_Image* mixImg1 = mixingImgOp(gradientImgs[0], gradientImgs[1], gradientMix);
    Rasteron_Image* mixImg2 = mixingImgOp(gradientImgs[2], gradientImgs[0], gradientMix);
    Rasteron_Image* mixImg3 = mixingImgOp(gradientImgs[1], gradientImgs[3], gradientMix);

    // Rasteron_Image* hypnosisImg = mixingImgOp(gradientImgs[0], gradientImgs[1], gradientMix);
    Rasteron_Image* hypnosisImg; // = mixingImgOp(mixImg3, gradientImgs[4], gradientMix);
    switch(pArg){
        case 0: hypnosisImg = mixingImgOp(mixImg1, gradientImgs[4], gradientMix); break;
        case 1: hypnosisImg = mixingImgOp(mixImg2, gradientImgs[4], gradientMix); break;
        case 2: hypnosisImg = mixingImgOp(mixImg3, gradientImgs[4], gradientMix); break;
        default: hypnosisImg = mixingImgOp(gradientImgs[4], gradientImgs[4], gradientMix); break;
    }
    
    for(unsigned g = 0; g < 5; g++) RASTERON_DEALLOC(gradientImgs[g]);
    RASTERON_DEALLOC(mixImg1); RASTERON_DEALLOC(mixImg2); RASTERON_DEALLOC(mixImg3);

    // RASTERON_DEALLOC(gradientImg1);

    return hypnosisImg; 
} 

static unsigned gecko(unsigned color, double distance){
    return (distance < 0.15)? color_level(color, 0.15) : (distance < 0.3)? color : color_level(color, 0.85);
}

Rasteron_Image* geckoImgOp(unsigned short points){ 
    ColorPointTable colorPointTable;
    colorPointTable.pointCount = points;

    for(unsigned p = 0; p < points; p++){
        colorPointTable.points[p] = (ColorPoint){
            (double)rand() / RAND_MAX,
            (double)rand() / RAND_MAX,
            RAND_COLOR()
        };
    }

    return fieldImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, gecko); 
}

Rasteron_Image* typographyImgOp(unsigned bgColor, unsigned textColor){ 
    genFullFilePath("Tw-Cen-MT.ttf", &fullFilePath);

    /* Rasteron_Text textObj;
    textObj.fontFile = fullFilePath;
    textObj.text = text;
    textObj.bkColor = 0xFF000000;
    textObj.fgColor = 0xFFEEEEEE; */

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

    xDiff *= xDiff;
    yDiff *= yDiff;

    if(xDiff > yDiff) return colors_blend(0xFFFF00FF, 0xFFEEEEEE, xDiff - yDiff);
    else return colors_blend(0xFF00FF00, 0xFF111111, yDiff - xDiff);
}

Rasteron_Image* mosaicImgOp(double z1, double z2){
    zig = z1; zag = z2;
    Rasteron_Image* zigzagImg = mapImgOp((ImageSize){ 1024, 1024, }, zigzag);

    return zigzagImg;
}

#define KNIT_AMOUNT 64
#define KNIT_THRESH 0xFF

static unsigned interference(unsigned color, unsigned neighbors[2]){
    static unsigned k = KNIT_AMOUNT;

    if((neighbors[0] & 0xFF) + (neighbors[1] & 0xFF) > KNIT_THRESH) return color + k;
    else return color - k;
}

Rasteron_Image* interferenceImgOp(unsigned short inc, unsigned short dec){
    Rasteron_Image* patternImg = noiseImgOp_white((ImageSize){ 1024, 1024 }, 0xFF000000, 0xFFEEEEEE); // solidImgOp((ImageSize){ 1024, 1024 }, RAND_COLOR());

    Rasteron_Image* interferenceImg1 = cellwiseRowImgOp(patternImg, interference);
    Rasteron_Image* interferenceImg2 = cellwiseColImgOp(interferenceImg1, interference);

    RASTERON_DEALLOC(interferenceImg1);
    RASTERON_DEALLOC(patternImg);

    return interferenceImg2;
}

static unsigned perturb(double x, double y){
    unsigned perturbColor1 = 0xFF00FF00;
    unsigned perturbColor2 = 0xFFFF0000;

    return colors_blend(0xFFFF0000, 0xFF00FF00, (x * 2) * (y * 2)) | 0x88;
    // return colors_blend(perturbColor1, perturbColor2, x * y) | 0x33;
}

Rasteron_Image* perturbImgOp(double xCenter, double yCenter){
    ColorGrid grid = { 9, 9, 0xFFFF0000, 0xFF00FF00 };

    Rasteron_Image* coordImg = mapImgOp((ImageSize){ 1024, 1024 }, perturb);
    Rasteron_Image* checkerImg = checkerImgOp((ImageSize){ 1024, 1024 }, grid);

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
            *(ballingImg->data + p) = colors_blend(0xFF333333, 0xFFEEEEEE, (torusDist - 150.0) / (300.00 * size));
        if(blobDist1 < (200.0 * size)) 
            *(ballingImg->data + p) = colors_blend(0xFF00EE00, 0xFF333333, (blobDist1 - 50.0) / (200.0 * size));
        if(blobDist2 < (100.0 * size)) 
            *(ballingImg->data + p) = colors_blend(0xFF0000EE, 0xFF333333, (blobDist2 - 50.0) / (100.0 * size));
    }

    return ballingImg;
}

Rasteron_Image* stratifyImgOp(unsigned short levels){
    genFullFilePath("Zero+.bmp", &fullFilePath);

    Rasteron_Image* loadedImg = loadImgOp(fullFilePath);
    Rasteron_Image* resizeImg = resizeImgOp((ImageSize){ loadedImg->height / 2, loadedImg->width / 2 }, loadedImg);
    Rasteron_Image* stratifyImg = splitImgOp(resizeImg, levels);

    RASTERON_DEALLOC(loadedImg);
    RASTERON_DEALLOC(resizeImg);

    return stratifyImg;
}

#define REACTDIFF_ITERS 5

static double killRate = 0.1; // 0.061;
static unsigned chemical1 = 0xFFFF0000;
static double feedRate = 0.1; // 0.025;
static unsigned chemical2 = 0xFF0000FF;

/* unsigned reactDiffuse(unsigned color, unsigned neighbors[8]){ 
    if(neighbors[0] == chemical2 || neighbors[1] == chemical2 || neighbors[2] == chemical2 || neighbors[3] == chemical2 || neighbors[4] == chemical2 || neighbors[5] == chemical2 || neighbors[6] == chemical2 || neighbors[7] == chemical2)
        return ((double)rand() / (double)RAND_MAX < feedRate)? chemical2 : color;
    if(neighbors[0] == chemical1 || neighbors[1] == chemical1 || neighbors[2] == chemical1 || neighbors[3] == chemical1 || neighbors[4] == chemical1 || neighbors[5] == chemical1 || neighbors[6] == chemical1 || neighbors[7] == chemical1)
        return ((double)rand() / (double)RAND_MAX < killRate)? 0xFF333333 : chemical1;
    return color; 
} */

unsigned grainPattern(unsigned color){ 
    if(color == chemical1) return ((double)rand() / (double)RAND_MAX < 0.1)? 0xFF333333 : chemical1; 
    else return ((double)rand() / (double)RAND_MAX < 0.1)? chemical2 : color;
}

unsigned napkin(unsigned color, unsigned neighbors[2]){ return colors_blend(neighbors[0], neighbors[1], 0.5); }

Rasteron_Image* foldsImgOp(unsigned color1, unsigned color2){
    chemical1 = color1; chemical2 = color2;

    Rasteron_Image* seedImg = solidImgOp((ImageSize){ 1024, 1024 }, color1);
    for(unsigned i = 0; i < REACTDIFF_ITERS; i++){
        Rasteron_Image* tempImg = copyImgOp(seedImg);
        RASTERON_DEALLOC(seedImg);
        seedImg = recolorImgOp(tempImg, grainPattern);
        RASTERON_DEALLOC(tempImg);
    }
    Rasteron_Image* napkinImg = cellwiseRowImgOp(seedImg, napkin);// cellwiseExtImgOp(seedImg, reactDiffuse, 1);
    Rasteron_Image* gradientImg = gradientImgOp((ImageSize){ 1024, 1024}, SIDE_Bottom, color1, color2);
    Rasteron_Image* finalImg = copyImgOp(napkinImg); // fusionImgOp(gradientImg, napkinImg, 0.75);

    RASTERON_DEALLOC(seedImg);
    RASTERON_DEALLOC(gradientImg);
    RASTERON_DEALLOC(napkinImg);

    return finalImg;
}

Rasteron_Image* edgesImgOp(unsigned short rows, unsigned short cols){
    Rasteron_Image* img1 = gradientImgOp((ImageSize){ 1024 / rows, 1024 / cols }, SIDE_Bottom, 0xFFFF0000, 0xFF0000FF);

    Rasteron_Image* comboImgs1[4] = {
        flipImgOp(img1, FLIP_Counter), flipImgOp(img1, FLIP_Upside), flipImgOp(img1, FLIP_None), flipImgOp(img1, FLIP_Clock)
    };

    Rasteron_Image* edgesImg = RASTERON_ALLOC("edges", 1024, 1024);

    for(unsigned p = 0; p < edgesImg->width * edgesImg->height; p++){
        double x = (1.0 / (double)1024) * (p % 1024);
		double y = (1.0 / (double)1024) * (p / 1024);

        unsigned c = x * rows;
        unsigned r = y * cols;

        /* if((c % 2 == 0 && r % 2 == 0) || (c % 2 == 1 && r % 2 == 1)) *(edgesImg->data + p) = *(comboImgs1[c % 4]->data + (p % (1024 / rows)));
        else *(edgesImg->data + p) = *(comboImgs2[r % 4]->data + (p % (1024 / rows))); */

        *(edgesImg->data + p) = *(comboImgs1[c % 3]->data + (p % (1024 / rows)));
    }

    for(unsigned i = 0; i < 4; i++) RASTERON_DEALLOC(comboImgs1[i]);
    RASTERON_DEALLOC(img1);

    return edgesImg; 
}

// Placeholder Images

Rasteron_Image* expImgOp1(){
    Rasteron_Image* paintImg = solidImgOp((ImageSize){ 1024, 1024 }, RAND_COLOR());

    int direction = 1; // positive

    unsigned color = 0xFF000000 | RAND_COLOR();

    for(unsigned p = 0; p < 1024 * 1024; p++){
        *(paintImg->data + p) = 0xFF000000 | color;

        if(direction == 1 && color > 0xFFAAFFFF) direction = 0;
        else if(direction == 0 && color < 0xFF0000AA) direction = 1;

        if(direction) color += rand() % 256;
        else color -= rand() % 256;
    }

    Rasteron_Image* mirrorImg = mirrorImgOp(paintImg);
    Rasteron_Image* experimentalImg = blendImgOp(paintImg, mirrorImg);

    return experimentalImg;
}

Rasteron_Image* expImgOp2(){ return expImgOp1(); }
Rasteron_Image* expImgOp3(){ return expImgOp1(); }
Rasteron_Image* expImgOp4(){ return expImgOp1(); }
Rasteron_Image* expImgOp5(){ return expImgOp1(); }
Rasteron_Image* expImgOp6(){ return expImgOp1(); }
Rasteron_Image* expImgOp7(){ return expImgOp1(); }
Rasteron_Image* expImgOp8(){ return expImgOp1(); }
Rasteron_Image* expImgOp9(){ return expImgOp1(); }
Rasteron_Image* expImgOp10(){ return expImgOp1(); }
Rasteron_Image* expImgOp11(){ return expImgOp1(); }
Rasteron_Image* expImgOp12(){ return expImgOp1(); }
Rasteron_Image* expImgOp13(){ return expImgOp1(); }
Rasteron_Image* expImgOp14(){ return expImgOp1(); }