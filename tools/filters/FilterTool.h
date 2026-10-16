#include "../Tool.h"

#define FILTER_DEFAULT_IMAGE "Starcase.bmp" // inside the assets directory

// --------------------------------   Filter State    -------------------------------- //

enum FILTER_Channel { FCHANNEL_All = 0, FCHANNEL_Red = 1, FCHANNEL_Green = 2, FCHANNEL_Blue = 3 };
enum FILTER_Swap { FSWAP_None = 0, FSWAP_RG = 1, FSWAP_RB = 2, FSWAP_GB = 3 };
enum FILTER_Fade { FFADE_None = 0, FFADE_White = 1, FFADE_Black = 2 };

typedef struct {
    char source[TOOL_TEXT_MAX]; // path of the reference image
    // geometry
    int cropSide; // index into _toolSideNames
    double crop; // fraction kept, 1 keeps everything
    int flip; // index into _toolFlipNames
    int mirror;
    int resize[2]; // width height, 0 0 keeps the size
    double corners; // rounded corner cut
    // tone
    double darkness; // blend toward black
    double lightness; // blend toward white
    double saturation; // 0 grey, 1 unchanged, 2 vivid
    double contrast; // 1 unchanged
    double gamma; // 1 unchanged
    double hue; // rotation in degrees
    unsigned tint;
    double tintAmount;
    double threshold; // 0 off, otherwise black and white cut by brightness
    // channels
    int invert;
    int grey;
    int channel;
    int swap;
    int levels; // posterize levels, 0 off
    int smooth; // antialias passes
    int fade; // transparency from white or black
} Filter_State;

extern Filter_State _filter;

// --------------------------------   Filter Operations (see Filter.c)    -------------------------------- //

Rasteron_Image* tuneImgOp(ref_image_t refImage, const Filter_State* state); // per pixel tone adjustments
Rasteron_Image* filterChainImgOp(ref_image_t refImage, const Filter_State* state); // full chain in fixed order
int filterIsNeutral(const Filter_State* state); // TRUE when the chain leaves the image unchanged
