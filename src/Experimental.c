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
    genFullFilePath("Tw-Cen-MT.ttf", &fullFilePath);

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

    if(xDiff > yDiff) return colors_blend(0xFFFF00FF, 0xFFEEEEEE, xDiff - yDiff);
    else return colors_blend(0xFF00FF00, 0xFF111111, yDiff - xDiff);
}

Rasteron_Image* mozaicImgOp(double z1, double z2){
    zig = z1; zag = z2;
    Rasteron_Image* zigzagImg = mapImgOp((ImageSize){ 1024, 1024, }, zigzag);

    return zigzagImg;
}

#define INTERFERE_AMOUNT 64
#define INTERFERE_THRESH 0xAA

static unsigned interference(unsigned color, unsigned neighbors[2]){
    static unsigned k = INTERFERE_AMOUNT;

    if((neighbors[0] & 0xFF) + (neighbors[1] & 0xFF) > INTERFERE_THRESH) return color + k;
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

unsigned barcode1;
unsigned barcode2;

unsigned grainPattern(unsigned color){ 
    if(color == barcode1) return ((double)rand() / (double)RAND_MAX < 0.1)? 0xFF333333 : barcode1; 
    else return ((double)rand() / (double)RAND_MAX < 0.1)? barcode2 : color;
}

unsigned bark(unsigned color, unsigned neighbors[2]){ return (rand() % 2 == 0)? neighbors[0] : neighbors[1]; }
// unsigned napkin(unsigned color, unsigned neighbors[2]){ return colors_blend(neighbors[0], neighbors[1], 0.5); }

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

Rasteron_Image* combImgOp(unsigned short rows, unsigned short cols){
    Rasteron_Image* img1 = gradientImgOp((ImageSize){ 1024 / rows, 1024 / cols }, SIDE_Bottom, 0xFFFF0000, 0xFF0000FF);

    Rasteron_Image* comboImgs1[4] = {
        flipImgOp(img1, FLIP_Counter), flipImgOp(img1, FLIP_Upside), flipImgOp(img1, FLIP_None), flipImgOp(img1, FLIP_Clock)
    };

    Rasteron_Image* combImg = RASTERON_ALLOC("comb", 1024, 1024);

    for(unsigned p = 0; p < combImg->width * combImg->height; p++){
        double x = (1.0 / (double)1024) * (p % 1024);
		double y = (1.0 / (double)1024) * (p / 1024);

        unsigned c = x * rows;
        unsigned r = y * cols;

        *(combImg->data + p) = colors_blend(*(comboImgs1[c % 3]->data + (p % (1024 / rows))), 0xFF333333, pow(x, y));
        *(combImg->data + p) = colors_diff(*(combImg->data + p), color_invert(*(combImg->data + p)));
    }

    for(unsigned i = 0; i < 4; i++) RASTERON_DEALLOC(comboImgs1[i]);
    RASTERON_DEALLOC(img1);

    return combImg; 
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

double truschetX1 = 0.25; // double truschetX2 = 0.25;
double truschetY1 = 0.75; // double truschetY2 = 0.75;

static unsigned sharpTruschetTile(double x, double y){
    // return (sin(x * 30.0) > sin(y * 30.0))? colors_blend(0xFFFFFF00, 0xFFEEEEEE, x) : colors_blend(0xFFFF00FF, 0xFFEEEEEE, y - x);
    if(sin(x) < 0.5 && sin(-y) < 0.5) return colors_blend(0xFFFFFF00, 0xFF333333, x);
    else if(tan(x) > 0.5 && tan(y) > 0.5) return colors_blend(0xFFF00FFFF, 0xFFEEEEEE, y);
    else return 0xFFFF00FF;
}

Rasteron_Image* truschetImgOp(unsigned short wDiv, unsigned short hDiv){ 
    // Rasteron_Image* truchetTile = RASTERON_ALLOC("truschet_tile", 1024 / hDiv, 1024 / wDiv);
    Rasteron_Image* truschetTile = mapImgOp((ImageSize){ 1024 / hDiv, 1024 / wDiv }, sharpTruschetTile);
    Rasteron_Image* truschetTile2 = flipImgOp(truschetTile, FLIP_Upside);
    Rasteron_Image* truschetTile3 = flipImgOp(truschetTile, FLIP_Clock);
    Rasteron_Image* truschetTile4 = flipImgOp(truschetTile, FLIP_Counter);

    Rasteron_Image* finalImg = RASTERON_ALLOC("truschet", 1024, 1024);
    for(unsigned p = 0; p < 1024 * 1024; p++){
        double x = (1.0 / (double)1024) * (p % 1024);
		double y = (1.0 / (double)1024) * (p / 1024);

        unsigned c = x * wDiv;
        unsigned r = y * hDiv;

        Rasteron_Image** targetTileImg;
        if(c % 2 == 0) targetTileImg = (r % 2 == 0)? &truschetTile : &truschetTile2;
        else targetTileImg = (r % 2 == 0)? &truschetTile3 : &truschetTile4;

        *(finalImg->data + p) = pixPoint_color((PixelPoint){ (x * wDiv) - floor(x * wDiv), (y * hDiv) - floor(y * hDiv) }, *targetTileImg);
    }

    RASTERON_DEALLOC(truschetTile);
    RASTERON_DEALLOC(truschetTile2);
    RASTERON_DEALLOC(truschetTile3);
    RASTERON_DEALLOC(truschetTile4);

    return finalImg; 
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
        return (pow(smallX, smallY) > 0.5)? colors_blend(graterColor1, graterColor2, fabs(smallX - smallY)) : graterColor2;
    else return (pow(smallX, smallY) < 0.5)? colors_blend(graterColor1, graterColor2, fabs(smallX - smallY)) : graterColor2;
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
    genFullFilePath("Starcase.bmp", &fullFilePath);

    Rasteron_Image* loadedImg = loadImgOp(fullFilePath);
    Rasteron_Image* fuzzlikeImg = cellwiseExtImgOp(loadedImg, fuzzlike, iters);

    RASTERON_DEALLOC(loadedImg);

    return fuzzlikeImg;
}

Rasteron_Image* oozelikeImgOp(unsigned short colorMode){
    Rasteron_Image* noiseImg = noiseImgOp_scratch((ImageSize){ 1024, 1024 }, (ColorGrid){ 16, 16, 0xFF330088, 0xFF003388});

    Rasteron_Image* redNoiseImg = noiseImgOp_white((ImageSize){ 1024, 1024 }, 0xFF110088, 0xFFEE0088);
    Rasteron_Image* greenNoiseImg = noiseImgOp_white((ImageSize){ 1024, 1024 }, 0xFF001188, 0xFF00EE88);
    Rasteron_Image* blueNoiseImg = noiseImgOp_white((ImageSize){ 1024, 1024 }, 0xFF000011, 0xFF0000EE);

    Rasteron_Image* perturbImg1 = warpingImgOp(redNoiseImg, greenNoiseImg);
    Rasteron_Image* perturbImg2 = warpingImgOp(greenNoiseImg, redNoiseImg);
    Rasteron_Image* perturbBlendImg = blendImgOp(perturbImg1, perturbImg2);

    Rasteron_Image* oozelikeImg = warpingImgOp(perturbBlendImg, noiseImg); // cellwiseExtImgOp(noiseImg, hatch, 6);

    RASTERON_DEALLOC(noiseImg);
    RASTERON_DEALLOC(redNoiseImg); RASTERON_DEALLOC(greenNoiseImg); RASTERON_DEALLOC(blueNoiseImg);
    RASTERON_DEALLOC(perturbImg1); RASTERON_DEALLOC(perturbImg2); RASTERON_DEALLOC(perturbBlendImg);

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
    Rasteron_Image* recurrantImg = noiseImgOp_value((ImageSize){ 1024, 1024 }, colorGrid);

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
    double centerDist = sqrt(pow(x - 0.5, 2), pow(y - 0.5, 2));

    if((centerDist * swirl) - (floor(centerDist * swirl)) > (centerAngle * swirl) - (floor(centerAngle * swirl))) return 0xFF333333;
    else return 0xFFEEEEEE;
}

Rasteron_Image* swirlyImgOp(double swirlFactor){
    swirl = swirlFactor;
    return mapImgOp((ImageSize){ 1024, 1024 }, swirly);
}

Rasteron_Image* displacerImgOp(unsigned short iters, unsigned color1, unsigned color2){
    Rasteron_Image* noiseTileImg1 = noiseImgOp_tiled((ImageSize){ 1024, 1024 }, (ColorGrid){ iters * 5, iters, color1, color2 });
    Rasteron_Image* noiseTileImg2 = noiseImgOp_tiled((ImageSize){ 1024, 1024 }, (ColorGrid){ iters, iters * 5, color1, color2 });
    Rasteron_Image* gradientImg1 = gradientImgOp((ImageSize){ 1024, 1024 }, SIDE_Left, color1, color2);
    Rasteron_Image* gradientImg2 = gradientImgOp((ImageSize){ 1024, 1024 }, SIDE_Right, color_invert(color1), color_invert(color2));
    // Rasteron_Image* warpTileImg = noiseImgOp_value((ImageSize){ 1024, 1024 }, (ColorGrid){ cells, cells, 0xFF666666, 0xFFAAAAAA });

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

    for(unsigned p = 0; p < 1024; p++) colorLine[p] = colors_blend(color, color_invert(color), p / 1024.0); // color + (rand() % variant) - (variant / 2);

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
                else *(arcImg->data + p) = colors_blend(*(arcImg->data + p), arcTable.points[a].color, 0.5);
            } 
            else if(a == 0) *(arcImg->data + p) = arcDefColor;
        }

    return arcImg;
}


double ultX1 = 1.0; double ultX2 = 1.0;
double ultY1 = 1.0; double ultY2 = 1.0;
unsigned ultM = 64;

unsigned ultCoord(double x, double y){
    unsigned color = 0xFF333333;

    unsigned short xDivs = 2;
    unsigned short yDivs = 2;

    for(unsigned u = 0; u < 5; u++){
        double xInc = (pow(xDivs, u + 1) * x) - floor(pow(xDivs, u + 1) * x);
        double yInc = (pow(yDivs, u + 1) * y) - floor(pow(yDivs, y + 1) * x);

        if(color == 0xFF333333)
            if(fabs(xInc / yInc) > 0.9 * ultX1 && fabs(xInc / yInc) < 1.1 * ultY1) color = colors_blend(0xFFFF0000, 0xFF0000FF, (xInc * ultX2) * (yInc * ultY2));
        else break;
    }

    return color;
}

/* unsigned ultMix(unsigned color1, unsigned color2, unsigned color3, unsigned color4){ 
    return (color1 - color2) + (color2 - color1) + (color4 - color3) + (color3 - color4);
} */

unsigned ultMix(unsigned color1, unsigned color2){
    for(unsigned c = 0; c < 5; c++){
        if(color1 % ultM > color2 % ultM){ 
            color1 += color2 % (ultM / 2);
            color2 -= color1 % (ultM / 2);
            if(c == 4) return color1;
        }
        else {
            color1 -= color2 % (ultM / 2);
            color2 += color1 % (ultM / 2);
            if(c == 4) return color2;
        }
    }
    return colors_blend(color1, color2, 0.5);
}

unsigned ultCellwise(unsigned cell, unsigned nebrs[8]){
    static unsigned i = 0;
    i++;
    return cell; // nebrs[(i % 2 == 0)? nebrs[NEBR_Left] : nebrs[NEBR_Right]];
}

Rasteron_Image* ultImgOp(short seed, unsigned short factor, double x1, double x2, double y1, double y2){
    ultX1 = x1; ultX2 = x2;
    ultY1 = y1; ultY2 = y2;
    ultM = factor;
    
    Rasteron_Image* coordImg = mapImgOp((ImageSize){ 256, 256 }, ultCoord);
    Rasteron_Image* coordVarImgs[3] = {
        copyImgOp(coordImg), copyImgOp(coordImg), copyImgOp(coordImg)
    };

    for(unsigned p = 0; p < 256 * 256; p++){
        *(coordVarImgs[0]->data + p) = color_invert(*(coordImg->data + p));
        *(coordVarImgs[1]->data + p) = color_level(*(coordImg->data + p), 0.5);
        *(coordVarImgs[2]->data + p) = colors_diff(*(coordImg->data + p), color_unique());
    }

    // Rasteron_Image* mixImg = mixingExtImgOp(coordImg, coordVarImgs[0], coordVarImgs[1],  coordVarImgs[2], ultMix);
    Rasteron_Image* mixImg = mixingImgOp(coordImg, coordVarImgs[seed % 3], ultMix);
    Rasteron_Image* ultImg = cellwiseExtImgOp(mixImg, ultCellwise, 3);

    RASTERON_DEALLOC(coordImg);
    for(unsigned i = 0; i < 3; i++) RASTERON_DEALLOC(coordVarImgs[i]);
    RASTERON_DEALLOC(mixImg);

    return ultImg;
}