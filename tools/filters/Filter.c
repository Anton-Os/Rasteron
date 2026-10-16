#include "FilterTool.h"

// --------------------------------   Tone    -------------------------------- //

static double clamp255(double v){ return (v < 0.0)? 0.0 : (v > 255.0)? 255.0 : v; }

static const Filter_State* _tune = NULL; // parameters for tuneColor
static double _hueMatrix[9];

static void buildHueMatrix(double degrees){ // luminance preserving hue rotation
    double a = degrees * 3.14159265358979 / 180.0;
    double c = cos(a), s = sin(a);
    double m[9] = {
        0.213 + c * 0.787 - s * 0.213, 0.715 - c * 0.715 - s * 0.715, 0.072 - c * 0.072 + s * 0.928,
        0.213 - c * 0.213 + s * 0.143, 0.715 + c * 0.285 + s * 0.140, 0.072 - c * 0.072 - s * 0.283,
        0.213 - c * 0.213 - s * 0.787, 0.715 - c * 0.715 + s * 0.715, 0.072 + c * 0.928 + s * 0.072
    };
    memcpy(_hueMatrix, m, sizeof(m));
}

static unsigned tuneColor(unsigned color){
    const Filter_State* s = _tune;
    unsigned alpha = color & ALPHA_CHANNEL;
    double r = (color & RED_CHANNEL) >> 16, g = (color & GREEN_CHANNEL) >> 8, b = color & BLUE_CHANNEL;

    if(s->hue != 0.0){
        double nr = _hueMatrix[0] * r + _hueMatrix[1] * g + _hueMatrix[2] * b;
        double ng = _hueMatrix[3] * r + _hueMatrix[4] * g + _hueMatrix[5] * b;
        double nb = _hueMatrix[6] * r + _hueMatrix[7] * g + _hueMatrix[8] * b;
        r = clamp255(nr); g = clamp255(ng); b = clamp255(nb);
    }
    if(s->saturation != 1.0){
        double lum = 0.299 * r + 0.587 * g + 0.114 * b;
        r = clamp255(lum + (r - lum) * s->saturation);
        g = clamp255(lum + (g - lum) * s->saturation);
        b = clamp255(lum + (b - lum) * s->saturation);
    }
    if(s->contrast != 1.0){
        r = clamp255((r - 128.0) * s->contrast + 128.0);
        g = clamp255((g - 128.0) * s->contrast + 128.0);
        b = clamp255((b - 128.0) * s->contrast + 128.0);
    }
    if(s->gamma != 1.0){
        double inv = 1.0 / s->gamma;
        r = 255.0 * pow(r / 255.0, inv); g = 255.0 * pow(g / 255.0, inv); b = 255.0 * pow(b / 255.0, inv);
    }
    if(s->darkness > 0.0){ r *= 1.0 - s->darkness; g *= 1.0 - s->darkness; b *= 1.0 - s->darkness; }
    if(s->lightness > 0.0){ r += (255.0 - r) * s->lightness; g += (255.0 - g) * s->lightness; b += (255.0 - b) * s->lightness; }
    if(s->tintAmount > 0.0){
        double tr = (s->tint & RED_CHANNEL) >> 16, tg = (s->tint & GREEN_CHANNEL) >> 8, tb = s->tint & BLUE_CHANNEL;
        r += (tr - r) * s->tintAmount; g += (tg - g) * s->tintAmount; b += (tb - b) * s->tintAmount;
    }
    if(s->threshold > 0.0){
        double lum = (0.299 * r + 0.587 * g + 0.114 * b) / 255.0;
        r = g = b = (lum >= s->threshold)? 255.0 : 0.0;
    }
    if(s->invert){ r = 255.0 - r; g = 255.0 - g; b = 255.0 - b; }

    return alpha | ((unsigned)clamp255(r) << 16) | ((unsigned)clamp255(g) << 8) | (unsigned)clamp255(b);
}

Rasteron_Image* tuneImgOp(ref_image_t refImage, const Filter_State* state){
    _tune = state;
    buildHueMatrix(state->hue);

    Rasteron_Image* tuneImg = RASTERON_ALLOC("tune", refImage->height, refImage->width);
    for(unsigned p = 0; p < refImage->width * refImage->height; p++)
        *(tuneImg->data + p) = tuneColor(*(refImage->data + p));
    return tuneImg;
}

static int toneIsNeutral(const Filter_State* s){
    return s->hue == 0.0 && s->saturation == 1.0 && s->contrast == 1.0 && s->gamma == 1.0 && s->darkness == 0.0
        && s->lightness == 0.0 && s->tintAmount == 0.0 && s->threshold == 0.0 && !s->invert;
}

int filterIsNeutral(const Filter_State* s){
    return toneIsNeutral(s) && s->crop >= 1.0 && s->flip == 0 && !s->mirror && s->resize[0] == 0 && s->resize[1] == 0
        && s->corners == 0.0 && !s->grey && s->channel == FCHANNEL_All && s->swap == FSWAP_None && s->levels == 0
        && s->smooth == 0 && s->fade == FFADE_None;
}

// --------------------------------   Chain    -------------------------------- //


static Rasteron_Image* replace(Rasteron_Image* oldImg, Rasteron_Image* newImg){
    if(newImg != oldImg) RASTERON_DEALLOC(oldImg);
    return newImg;
}

Rasteron_Image* filterChainImgOp(ref_image_t refImage, const Filter_State* s){
    Rasteron_Image* image = copyImgOp(refImage);

    // geometry
    if(s->flip > 0) image = replace(image, flipImgOp(image, (enum FLIP_Type)(s->flip - 1)));
    if(s->mirror) image = replace(image, mirrorImgOp(image));
    if(s->crop < 1.0 && s->cropSide > 0) image = replace(image, cropImgOp(image, (enum SIDE_Type)(s->cropSide - 1), s->crop)); // sides as seen after flipping
    if(s->resize[0] > 0 || s->resize[1] > 0){
        unsigned width = (s->resize[0] > 0)? (unsigned)s->resize[0] : image->width;
        unsigned height = (s->resize[1] > 0)? (unsigned)s->resize[1] : image->height;
        image = replace(image, resizeImgOp((ImageSize){ height, width }, image));
    }

    // channels
    if(s->swap == FSWAP_RG) image = replace(image, colorSwitchImgOp(image, CHANNEL_Red, CHANNEL_Green));
    else if(s->swap == FSWAP_RB) image = replace(image, colorSwitchImgOp(image, CHANNEL_Red, CHANNEL_Blue));
    else if(s->swap == FSWAP_GB) image = replace(image, colorSwitchImgOp(image, CHANNEL_Green, CHANNEL_Blue));
    if(s->grey) image = replace(image, greyImgOp(image));
    if(s->channel != FCHANNEL_All) image = replace(image, filterImgOp(image, (CHANNEL_Type)(s->channel - 1)));

    // tone
    if(!toneIsNeutral(s)) image = replace(image, tuneImgOp(image, s));
    if(s->levels > 0) image = replace(image, splitImgOp(image, (unsigned short)s->levels));
    if(s->smooth > 0) image = replace(image, antialiasImgOp(image, (unsigned short)s->smooth));

    // alpha
    if(s->corners > 0.0) image = replace(image, cornerImgOp(image, s->corners, s->corners, s->corners, s->corners));
    if(s->fade == FFADE_White) image = replace(image, fadeImgOp_white(image));
    else if(s->fade == FFADE_Black) image = replace(image, fadeImgOp_black(image));

    return image;
}
