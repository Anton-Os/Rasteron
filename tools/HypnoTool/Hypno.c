#include "HypnoTool.h"

extern double segs;
extern double dist;
extern unsigned color1;
extern unsigned color2;

//Spirals

unsigned spirals(double x, double y){
    double centerAngle = atan((y - 0.5) / (x - 0.5));
    double centerDist = sqrt(pow(x - 0.5, 2) + pow(y - 0.5, 2));

    double factor = (centerAngle * segs) - floor(centerAngle * segs) * tan(centerDist * (segs + dist));
    return fuse_colors(color_level(color1, sin(centerAngle * segs)), color_level(color2, cos(centerDist * dist)), factor);
}

Rasteron_Image* spiralsImgOp(double s, double d){
    segs = s;
    dist = d;

    return mapImgOp((ImageSize){ 1024, 1024 }, spirals);
}

// Swirls

static double swirl = 10.0;
static unsigned swirlColor1 = 0xFF000000;
static unsigned swirlColor2 = 0xFFFFFFFF;

unsigned swirly(double x, double y) {
    double centerAngle = atan((y - 0.5) / (x - 0.5));
    double centerDist = sqrt(pow(x - 0.5, 2) + pow(y - 0.5, 2));

    // if ((centerDist * swirl) - (floor(centerDist * swirl)) > (centerAngle * swirl) - (floor(centerAngle * swirl)))
    if (sin(centerDist * swirl) - sin(floor(centerDist * swirl)) > cos(centerAngle * swirl) - cos(floor(centerAngle * swirl)))
    // if (atan(centerDist * swirl) - atan(floor(centerDist * swirl)) > tan(centerAngle * swirl) - tan(floor(centerAngle * swirl))) 
    // if (pow(swirl, centerDist) - (floor(pow(swirl, centerDist))) > pow(centerAngle, swirl) - (floor(pow(centerAngle, swirl))))
        return blend_colors(swirlColor1, swirlColor2, centerAngle); // swirlColor1
    else return blend_colors(swirlColor1, swirlColor2, centerDist); // swirlColor2
}

Rasteron_Image* swirlyImgOp(double swirlFactor, unsigned color1, unsigned color2) {
    swirl = swirlFactor;
    swirlColor1 = color1;
    swirlColor2 = color2;

    return mapImgOp((ImageSize) { 1024, 1024 }, swirly);
}

// Radial

Rasteron_Image* radialImgOp(unsigned colors[3], mixCallback3 mix_callback){
    Rasteron_Image* radialImgs[3] = {
        gradientImgOp((ImageSize){ 1024, 1024 }, SIDE_Radial, colors[0], colors[1]),
        gradientImgOp((ImageSize){ 1024 / 2, 1024 / 2 }, SIDE_Radial, colors[1], colors[2]),
        gradientImgOp((ImageSize){ 1024 / 4, 1024 / 4 }, SIDE_Radial, colors[0], colors[2]),
    };

    d = RAND_COLOR(); // for callback functions
    Rasteron_Image* radialImg = mixingExtImgOp(radialImgs[0], radialImgs[1], radialImgs[2], mix_callback);

    for(unsigned i = 0; i < 3; i++) RASTERON_DEALLOC(radialImgs[i]);

    return radialImg;
}

// Hypnosis

Rasteron_Image* hypnosisImgOp(unsigned color1, unsigned color2, unsigned short iters, mixCallback callback) {
    Rasteron_Image* gradientImgs[5] = {
        gradientImgOp((ImageSize) { 1024, 1024 }, SIDE_Left, color1, color2),
        gradientImgOp((ImageSize) { 1024, 1024 }, SIDE_Right, color1, color2),
        gradientImgOp((ImageSize) { 1024, 1024 }, SIDE_Top, color1, color2),
        gradientImgOp((ImageSize) { 1024, 1024 }, SIDE_Bottom, color1, color2),
        gradientImgOp((ImageSize) { 1024, 1024 }, SIDE_Radial, color1, color2),
    };

    Rasteron_Image* mixImg1 = mixingImgOp(gradientImgs[0], gradientImgs[1], callback);
    Rasteron_Image* mixImg2 = mixingImgOp(gradientImgs[2], gradientImgs[0], callback);
    Rasteron_Image* mixImg3 = mixingImgOp(gradientImgs[1], gradientImgs[3], callback);
    Rasteron_Image* mixImgs[3] = { mixImg1, mixImg2, mixImg3 };

    Rasteron_Image* hypnosisImg = mixingImgOp(mixImgs[iters % 3], gradientImgs[4], callback);
    if (iters > 3) {
        unsigned m = 0;
        for (unsigned i = 0; i < iters / 3; i++) {
            Rasteron_Image* stagingImg = mixingImgOp(mixImgs[m % 3], hypnosisImg, callback);
            RASTERON_DEALLOC(hypnosisImg);
            hypnosisImg = copyImgOp(stagingImg);
            RASTERON_DEALLOC(stagingImg);
            m++;
        }
    }

    for (unsigned g = 0; g < 5; g++) RASTERON_DEALLOC(gradientImgs[g]);
    RASTERON_DEALLOC(mixImg1); RASTERON_DEALLOC(mixImg2); RASTERON_DEALLOC(mixImg3);

    // RASTERON_DEALLOC(gradientImg1);

    return hypnosisImg;
}

// Mandala

float (*interpolate)(unsigned, float[2], float, float) = &mandalaInterp7; // from HypnoTool.h

unsigned mandalaMap(double x, double y){
    static unsigned invocation = 0;

    float coords[2] = { x, y };
    double centerAngle = atan((y - 0.5) / (x - 0.5));
    double centerDist = sqrt(pow(x - 0.5, 2) + pow(y - 0.5, 2));

    unsigned mandalaColor = blend_colors(
        fuse_colors(color1, color2, sin(y / centerAngle) + cos(x / centerDist)), 
        diff_colors(color_invert(color1), color_invert(color2)),
        interpolate(invocation, coords, centerAngle, centerDist)
    );

    invocation++;
    return mandalaColor;
}

Rasteron_Image* mandalaImgOp(coordCallback coord_callback, mixCallback mix_callback, float (*interp_callback)(unsigned, float[2], float, float)){
    if(interp_callback != NULL) interpolate = interp_callback;

    Rasteron_Image* mapImg1 = mapImgOp((ImageSize){ 1024, 1024 }, coord_callback);
    Rasteron_Image* mapImg2 = mapImgOp((ImageSize){ 1024, 1024 }, coord_callback);

    Rasteron_Image* symetricImg1 = mixingImgOp(mapImg1, mapImg2, mix_callback);
    Rasteron_Image* symetricImg2 = flipImgOp(symetricImg1, FLIP_Clock);

    Rasteron_Image* mandalaImg = mixingImgOp(symetricImg1, symetricImg2, restoreMix); // TODO: Find a way to simply copy

    RASTERON_DEALLOC(mapImg1); RASTERON_DEALLOC(mapImg2);
    RASTERON_DEALLOC(symetricImg1); RASTERON_DEALLOC(symetricImg2);

    return mandalaImg;
}