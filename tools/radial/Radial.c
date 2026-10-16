#include "RadialTool.h"

#define RADIAL_TAU 6.28318530717958647692

static double _aspect = 1.0; // width over height of the image being generated

// --------------------------------   Periodic Modes    -------------------------------- //

static double frac(double v){ return v - floor(v); }

double radialWave(int wave, double t){
    switch(wave){
        case WAVE_Sin: return 0.5 + 0.5 * sin(t);
        case WAVE_Cos: return 0.5 + 0.5 * cos(t);
        case WAVE_Tan: return 0.5 + 0.5 * tanh(tan(t * 0.5));
        case WAVE_Saw: return frac(t / RADIAL_TAU);
        case WAVE_Triangle: return 1.0 - fabs(2.0 * frac(t / RADIAL_TAU) - 1.0);
        case WAVE_Square: return (frac(t / RADIAL_TAU) < 0.5)? 1.0 : 0.0;
        case WAVE_Pulse: return (frac(t / RADIAL_TAU) < 0.15)? 1.0 : 0.0;
        case WAVE_Arc: return fabs(sin(t * 0.5));
        default: return 0.5;
    }
}

static unsigned periodicMap(double x, double y){
    const Radial_State* s = &_radial;
    double dx = (x - 0.5 - s->center[0]) * _aspect;
    double dy = (y - 0.5 - s->center[1]);
    double r = sqrt(dx * dx + dy * dy) / s->zoom; // 0 at vanishing point
    double theta = atan2(dy, dx); // -pi to pi
    double sym = (double)s->symmetry;

    // depth grows without bound toward the center so rings and spokes recede into it
    double depth = (s->recede > 0.0)? 1.0 / pow(r + 0.002, s->recede) : r * 8.0;
    double radial = s->frequency * depth;
    double value;

    switch(s->mode){
    case RADIAL_Rings:
        value = radialWave(s->wave, radial + s->twist * sin(sym * theta) + s->phase);
        break;
    case RADIAL_Spiral:
        value = radialWave(s->wave, radial * (1.0 + s->twist * 0.1) + sym * theta + s->phase);
        break;
    case RADIAL_Star:
        value = radialWave(s->wave, sym * theta + s->twist * depth + s->phase) * radialWave(s->wave, radial);
        value = sqrt(value);
        break;
    case RADIAL_Tunnel: {
        double a = radialWave(WAVE_Square, sym * theta + s->twist * depth);
        double b = radialWave(WAVE_Square, radial + s->phase);
        double shade = radialWave(s->wave, radial * 0.25 + s->phase);
        value = ((a > 0.5) != (b > 0.5))? 0.15 + 0.85 * shade : 0.85 * (1.0 - shade);
    } break;
    case RADIAL_Petals: {
        double petal = fabs(cos(sym * theta * 0.5));
        value = radialWave(s->wave, radial - petal * (2.0 + s->twist) * RADIAL_TAU * 0.25 + s->phase);
    } break;
    case RADIAL_Vortex: {
        double warped = theta + s->twist * 0.05 * depth;
        value = 0.5 * radialWave(s->wave, sym * warped + s->phase) + 0.5 * radialWave(s->wave, radial - s->phase);
    } break;
    default: value = 0.5; break;
    }

    unsigned color = blend_colors(s->colors[0], s->colors[1], value);

    if(s->fog > 0.0){ // center fades toward the distance color
        double haze = s->fog * exp(-r * 6.0 / (s->fog + 0.05));
        color = blend_colors(color, s->colors[2], (haze > 1.0)? 1.0 : haze);
    }
    return color | 0xFF000000;
}

Rasteron_Image* periodicImgOp(ImageSize size){
    _aspect = (double)size.width / (double)size.height;
    return mapImgOp(size, periodicMap);
}

// --------------------------------   HypnoTest Modes    -------------------------------- //

static double segs = RADIAL_SEGS;
static double dist = RADIAL_DIST;

// Spirals

unsigned spirals(double x, double y){
    double centerAngle = atan((y - 0.5 - _radial.center[1]) / (x - 0.5 - _radial.center[0])) + _radial.phase;
    double centerDist = sqrt(pow(x - 0.5 - _radial.center[0], 2) + pow(y - 0.5 - _radial.center[1], 2)) / _radial.zoom;

    double factor = (centerAngle * segs) - floor(centerAngle * segs) * tan(centerDist * (segs + dist));
    return fuse_colors(color_level(_radial.colors[0], sin(centerAngle * segs)), color_level(_radial.colors[1], cos(centerDist * dist)), factor) | 0xFF000000;
}

Rasteron_Image* spiralsImgOp(ImageSize size, double s, double d){
    segs = s;
    dist = d;
    return mapImgOp(size, spirals);
}

// Swirls

static double swirl = 10.0;
static unsigned swirlColor1 = 0xFF000000;
static unsigned swirlColor2 = 0xFFFFFFFF;

unsigned swirly(double x, double y) {
    double centerAngle = atan((y - 0.5 - _radial.center[1]) / (x - 0.5 - _radial.center[0])) + _radial.phase;
    double centerDist = sqrt(pow(x - 0.5 - _radial.center[0], 2) + pow(y - 0.5 - _radial.center[1], 2)) / _radial.zoom;

    if (sin(centerDist * swirl) - sin(floor(centerDist * swirl)) > cos(centerAngle * swirl) - cos(floor(centerAngle * swirl)))
        return blend_colors(swirlColor1, swirlColor2, centerAngle);
    else return blend_colors(swirlColor1, swirlColor2, centerDist);
}

Rasteron_Image* swirlyImgOp(ImageSize size, double swirlFactor, unsigned color1, unsigned color2) {
    swirl = swirlFactor;
    swirlColor1 = color1;
    swirlColor2 = color2;
    return mapImgOp(size, swirly);
}

// Radial

Rasteron_Image* radialImgOp(ImageSize size, unsigned colors[3], mixCallback3 mix_callback){
    Rasteron_Image* radialImgs[3] = {
        gradientImgOp(size, SIDE_Radial, colors[0] + colors[2], colors[1]),
        gradientImgOp((ImageSize){ size.height / 2, size.width / 2 }, SIDE_Radial, colors[1] * colors[0], colors[2]),
        gradientImgOp((ImageSize){ size.height / 3, size.width / 3 }, SIDE_Radial, colors[2] - colors[1], colors[0]),
    };

    d = RAND_COLOR(); // for callback functions
    Rasteron_Image* radialImg = mixExtImgOp(radialImgs[0], radialImgs[1], radialImgs[2], mix_callback);

    for(unsigned i = 0; i < 3; i++) RASTERON_DEALLOC(radialImgs[i]);
    return radialImg;
}

// Hypnosis

Rasteron_Image* hypnosisImgOp(ImageSize size, unsigned color1, unsigned color2, unsigned short iters, mixCallback callback) {
    Rasteron_Image* gradientImgs[5] = {
        gradientImgOp(size, SIDE_Left, color1, color2),
        gradientImgOp(size, SIDE_Right, color1, color2),
        gradientImgOp(size, SIDE_Top, color1, color2),
        gradientImgOp(size, SIDE_Bottom, color1, color2),
        gradientImgOp(size, SIDE_Radial, color1, color2),
    };

    Rasteron_Image* mixImg1 = mixImgOp(gradientImgs[0], gradientImgs[1], callback);
    Rasteron_Image* mixImg2 = mixImgOp(gradientImgs[2], gradientImgs[0], callback);
    Rasteron_Image* mixImg3 = mixImgOp(gradientImgs[1], gradientImgs[3], callback);
    Rasteron_Image* mixImgs[3] = { mixImg1, mixImg2, mixImg3 };

    Rasteron_Image* hypnosisImg = mixImgOp(mixImgs[iters % 3], gradientImgs[4], callback);
    if (iters > 3) {
        unsigned m = 0;
        for (unsigned i = 0; i < iters / 3; i++) {
            Rasteron_Image* stagingImg = mixImgOp(mixImgs[m % 3], hypnosisImg, callback);
            RASTERON_DEALLOC(hypnosisImg);
            hypnosisImg = stagingImg;
            m++;
        }
    }

    for (unsigned g = 0; g < 5; g++) RASTERON_DEALLOC(gradientImgs[g]);
    RASTERON_DEALLOC(mixImg1); RASTERON_DEALLOC(mixImg2); RASTERON_DEALLOC(mixImg3);

    return hypnosisImg;
}

// Mandala

static mandalaInterpCallback interpolate = &mandalaInterp7;
static unsigned invocation = 0;

unsigned mandalaMap(double x, double y){
    float coords[2] = { (float)x, (float)y };
    double centerAngle = atan((y - 0.5 - _radial.center[1]) / (x - 0.5 - _radial.center[0])) + _radial.phase;
    double centerDist = sqrt(pow(x - 0.5 - _radial.center[0], 2) + pow(y - 0.5 - _radial.center[1], 2)) / _radial.zoom;
    unsigned color1 = _radial.colors[0], color2 = _radial.colors[1];

    unsigned mandalaColor = blend_colors(
        fuse_colors(color1, color2, sin(y / centerAngle) + cos(x / centerDist)),
        diff_colors(color_invert(color1), color_invert(color2)),
        interpolate(invocation, coords, (float)centerAngle, (float)centerDist)
    );

    invocation++;
    return mandalaColor;
}

Rasteron_Image* mandalaImgOp(ImageSize size, coordCallback coord_callback, mixCallback mix_callback, mandalaInterpCallback interp_callback){
    if(interp_callback != NULL) interpolate = interp_callback;
    invocation = 0; // same output for the same parameters

    ImageSize square = { size.height, size.height }; // clockwise flip below needs matching sides
    Rasteron_Image* mapImg1 = mapImgOp(square, coord_callback);
    Rasteron_Image* mapImg2 = mapImgOp(square, coord_callback);

    Rasteron_Image* symetricImg1 = mixImgOp(mapImg1, mapImg2, mix_callback);
    Rasteron_Image* symetricImg2 = flipImgOp(symetricImg1, FLIP_Clock);

    Rasteron_Image* mandalaImg = mixImgOp(symetricImg1, symetricImg2, restoreMix);

    RASTERON_DEALLOC(mapImg1); RASTERON_DEALLOC(mapImg2);
    RASTERON_DEALLOC(symetricImg1); RASTERON_DEALLOC(symetricImg2);

    if(size.width != size.height){
        Rasteron_Image* sizedImg = resizeImgOp(size, mandalaImg);
        RASTERON_DEALLOC(mandalaImg);
        mandalaImg = sizedImg;
    }
    return mandalaImg;
}

// --------------------------------   Post Processing    -------------------------------- //

Rasteron_Image* spinImgOp(ref_image_t refImage, double angle, double xCenter, double yCenter){
    Rasteron_Image* spinImg = RASTERON_ALLOC("spin", refImage->height, refImage->width);
    double cosA = cos(-angle), sinA = sin(-angle);
    double cx = (0.5 + xCenter) * refImage->width, cy = (0.5 + yCenter) * refImage->height;

    for(unsigned p = 0; p < refImage->width * refImage->height; p++){
        double x = (double)(p % refImage->width) - cx;
        double y = (double)(p / refImage->width) - cy;
        long srcX = (long)(cx + x * cosA - y * sinA);
        long srcY = (long)(cy + x * sinA + y * cosA);
        srcX = ((srcX % (long)refImage->width) + refImage->width) % refImage->width; // wraps around edges
        srcY = ((srcY % (long)refImage->height) + refImage->height) % refImage->height;
        *(spinImg->data + p) = *(refImage->data + srcY * refImage->width + srcX);
    }
    return spinImg;
}

Rasteron_Image* radialEffectImgOp(ref_image_t refImage, int effect, double strength, double xCenter, double yCenter){
    switch(effect){
    case EFFECT_Invert: {
        Rasteron_Image* invertImg = copyImgOp(refImage);
        for(unsigned p = 0; p < invertImg->width * invertImg->height; p++)
            *(invertImg->data + p) = blend_colors(*(invertImg->data + p), color_invert(*(invertImg->data + p)), strength);
        return invertImg;
    }
    case EFFECT_Grey: {
        Rasteron_Image* greyImg = greyImgOp(refImage);
        Rasteron_Image* mixedImg = copyImgOp(refImage);
        for(unsigned p = 0; p < mixedImg->width * mixedImg->height; p++)
            *(mixedImg->data + p) = blend_colors(*(mixedImg->data + p), *(greyImg->data + p), strength);
        RASTERON_DEALLOC(greyImg);
        return mixedImg;
    }
    case EFFECT_Posterize: return splitImgOp(refImage, (unsigned short)(2 + strength * 10.0));
    case EFFECT_Smooth: return antialiasImgOp(refImage, (unsigned short)(1 + strength * 4.0));
    case EFFECT_Channel: return filterImgOp(refImage, (CHANNEL_Type)((int)(strength * 2.999)));
    case EFFECT_Vignette:
    case EFFECT_Glow: {
        Rasteron_Image* lightImg = copyImgOp(refImage);
        for(unsigned p = 0; p < lightImg->width * lightImg->height; p++){
            double x = (double)(p % lightImg->width) / lightImg->width - 0.5 - xCenter;
            double y = (double)(p / lightImg->width) / lightImg->height - 0.5 - yCenter;
            double r = sqrt(x * x + y * y) * 1.4142;
            unsigned color = *(lightImg->data + p);
            if(effect == EFFECT_Vignette) color = blend_colors(color, 0xFF000000, fmin(1.0, r * r * strength * 1.5));
            else color = blend_colors(color, 0xFFFFFFFF, fmax(0.0, (1.0 - r * 2.0)) * strength);
            *(lightImg->data + p) = color;
        }
        return lightImg;
    }
    default: return copyImgOp(refImage);
    }
}

// --------------------------------   Variant Lookup    -------------------------------- //
//
//      Kept here so the static 'd' color used by the radial mixes is the one radialImgOp sets

mixCallback3 radialMixVariant(int variant){
    static const mixCallback3 mixes[9] = { radialMix1, radialMix2, radialMix3, radialMix4, radialMix5, radialMix6, radialMix7, radialMix8, radialMix9 };
    return mixes[(variant - 1) % 9];
}

mixCallback hypnoticMixVariant(int variant){
    static const mixCallback mixes[3] = { hypnoticMix1, hypnoticMix2, hypnoticMix3 };
    return mixes[(variant - 1) % 3];
}

mixCallback mandalaMixVariant(int variant){
    static const mixCallback mixes[7] = { mandalaMix1, mandalaMix2, mandalaMix3, mandalaMix4, mandalaMix5, mandalaMix6, mandalaMix7 };
    return mixes[(variant - 1) % 7];
}

mandalaInterpCallback mandalaInterpVariant(int variant){
    static const mandalaInterpCallback interps[7] = { mandalaInterp1, mandalaInterp2, mandalaInterp3, mandalaInterp4, mandalaInterp5, mandalaInterp6, mandalaInterp7 };
    return interps[(variant - 1) % 7];
}
