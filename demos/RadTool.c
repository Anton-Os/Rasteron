#include "_Catalouge.h"

#define RADIAL_SEGS 10.0
#define RADIAL_DIST 10.0
#define RADIAL_INVOKE 1024

#include "_Demo.h"

double segs = RADIAL_SEGS;
double dist = RADIAL_DIST;
unsigned color1 = 0xFF111188;
unsigned color2 = 0xFF88EEEE;

static char keysave = '0';

static unsigned spirals(double x, double y){
    double centerAngle = atan((y - 0.5) / (x - 0.5));
    double centerDist = sqrt(pow(x - 0.5, 2) + pow(y - 0.5, 2));

    double factor = (centerAngle * segs) - floor(centerAngle * segs) * tan(centerDist * (segs + dist));
    return colors_fuse(color_level(color1, sin(centerAngle * segs)), color_level(color2, cos(centerDist * dist)), factor);
}

Rasteron_Image* spiralsImgOp(double s, double d){
    segs = s;
    dist = d;

    return mapImgOp((ImageSize){ 1024, 1024 }, spirals);
}

unsigned radialMix1(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return colors_diff(colors_diff(c1, c4), c3 - c2); } // starting
unsigned radialMix2(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return colors_diff(colors_diff(c2, c3), c4 + c1); } // reverse
unsigned radialMix3(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return colors_diff(colors_diff(c3, c1), c2 * c4); } // odds and evens
unsigned radialMix4(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return colors_diff(colors_diff(c4, c2), pow(c1, 1.0 / (double)(c3 & 0xFF))); } // evens and odds
unsigned radialMix5(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return colors_diff(c3 - c2, colors_diff(c1, c4)); } // starting alt
unsigned radialMix6(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return colors_diff(c4 + c1, colors_diff(c2, c3)); } // reverse alt
unsigned radialMix7(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return colors_diff(c2 * c4, colors_diff(c3, c1)); } // odds and evens alt
unsigned radialMix8(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return colors_diff(pow(c1, 1.0 / (double)(c3 & 0xFF)), colors_diff(c4, c2)); } // evens and odds alt
unsigned radialMix9(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return (c1 + c2 > c3 + c4)? colors_blend(c1, c2, 0.5) : colors_fuse(c3, c4, 0.5); } // inbetween

Rasteron_Image* radialImgOp(unsigned colors[4], mixCallback4 mix_callback){
    Rasteron_Image* radialImgs[4] = {
        gradientImgOp((ImageSize){ 1024, 1024 }, SIDE_Radial, colors[0], colors[1]),
        gradientImgOp((ImageSize){ 1024 / 2, 1024 / 2 }, SIDE_Radial, colors[1], colors[2]),
        gradientImgOp((ImageSize){ 1024 / 4, 1024 / 4 }, SIDE_Radial, colors[2], colors[3]),
        gradientImgOp((ImageSize){ 1024 / 6, 1024 / 6 }, SIDE_Radial, colors[3], colors[0])
    };

    Rasteron_Image* radialImg = mixingExtImgOp(radialImgs[0], radialImgs[1], radialImgs[2], radialImgs[3], mix_callback);

    for(unsigned i = 0; i < 4; i++) RASTERON_DEALLOC(radialImgs[i]);

    return radialImg;
}

static unsigned mandalaMix1(unsigned c1, unsigned c2){ return c1 + c2; }
static unsigned mandalaMix2(unsigned c1, unsigned c2){ return c1 * c2; }
static unsigned mandalaMix3(unsigned c1, unsigned c2){ return colors_powroot(c1, c2); }
static unsigned mandalaMix4(unsigned c1, unsigned c2){ return (c1 * 2) - (c2 / 2); }
static unsigned mandalaMix5(unsigned c1, unsigned c2){ return (c1 > c2)? color_invert(c1) : color_invert(c2); }
static unsigned mandalaMix6(unsigned c1, unsigned c2){ return (c1 * color_invert(c2) > color_invert(c1) * c2)? c1 : c2; }
static unsigned mandalaMix7(unsigned c1, unsigned c2){ return (colors_fuse(c1, c2, 0.5F) > colors_blend(c1, c2, 0.5F))? c1 : c2; }

static unsigned restoreMix(unsigned c1, unsigned c2){
    static unsigned lastColor;
    unsigned newColor = (c1 == 0xFF000000 || c1 == NO_COLOR || c1 == lastColor)? c2 : c1;
    lastColor = c1;
    return newColor;
}

float mandalaInterp1(unsigned i, float coords[2], float a, float d){ return tan((coords[0] + coords[1]) / (i * RADIAL_INVOKE)) * pow(a, d); }
float mandalaInterp2(unsigned i, float coords[2], float a, float d){ return tan((coords[0] + coords[1]) / (i * RADIAL_INVOKE)) * pow(d, a); }
float mandalaInterp3(unsigned i, float coords[2], float a, float d){ return sin((coords[0] - coords[1]) / (i * RADIAL_INVOKE)) * pow(a, d); }
float mandalaInterp4(unsigned i, float coords[2], float a, float d){ return cos((coords[0] - coords[1]) / (i * RADIAL_INVOKE)) * pow(d, a); }
float mandalaInterp5(unsigned i, float coords[2], float a, float d){ return tan((coords[0] * coords[1]) / (i * RADIAL_INVOKE)) * pow(d, a); }
float mandalaInterp6(unsigned i, float coords[2], float a, float d){ return sin((coords[0] / coords[1]) / (i * RADIAL_INVOKE)) * pow(a, d); }
float mandalaInterp7(unsigned i, float coords[2], float a, float d){ return cos((pow(coords[0], coords[1])) / (i * RADIAL_INVOKE)) * pow(d, a); }

float (*interpolate)(unsigned, float[2], float, float) = &mandalaInterp7;

static unsigned mandalaMap(double x, double y){
    static unsigned invocation = 0;

    float coords[2] = { x, y };
    double centerAngle = atan((y - 0.5) / (x - 0.5));
    double centerDist = sqrt(pow(x - 0.5, 2) + pow(y - 0.5, 2));

    unsigned mandalaColor = colors_blend(
        colors_fuse(color1, color2, sin(y / centerAngle) + cos(x / centerDist)), 
        colors_diff(color_invert(color1), color_invert(color2)),
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

void _onKeyEvent(char key){
    if(isalpha(key)) RASTERON_DEALLOC(_outputImg);

    color1 = RAND_COLOR(); color2 = RAND_COLOR();
    unsigned radialColors[4] = { color1, color2, color_invert(color1), color_invert(color2) };
    switch(tolower(key)){
        case 'q': _outputImg = spiralsImgOp(0.1F * RADIAL_SEGS, 0.1F * RADIAL_DIST); break;
        case 'w': _outputImg = spiralsImgOp(0.1F * RADIAL_SEGS, 0.5F * RADIAL_DIST); break;
        case 'e': _outputImg = spiralsImgOp(0.5F * RADIAL_SEGS, 0.1F * RADIAL_DIST); break;
        case 'r': _outputImg = spiralsImgOp(1.0F * RADIAL_SEGS, 1.0F * RADIAL_DIST); break;
        case 't': _outputImg = spiralsImgOp(2.0F * RADIAL_SEGS, 2.0F * RADIAL_DIST); break;
        case 'y': _outputImg = spiralsImgOp(3.0F * RADIAL_SEGS, 3.0F * RADIAL_DIST); break;
        case 'u': _outputImg = spiralsImgOp(5.0F * RADIAL_SEGS, 5.0F * RADIAL_DIST); break;
        case 'i': _outputImg = spiralsImgOp(10.0F * RADIAL_SEGS, 10.0F * RADIAL_DIST); break;
        case 'o': _outputImg = spiralsImgOp(10.0F * RADIAL_SEGS, 100.0F * RADIAL_DIST); break;
        case 'p': _outputImg = spiralsImgOp(100.0F * RADIAL_SEGS, 10.0F * RADIAL_DIST); break;
        case 'a': _outputImg = radialImgOp(radialColors, radialMix1); break;
        case 's': _outputImg = radialImgOp(radialColors, radialMix2); break;
        case 'd': _outputImg = radialImgOp(radialColors, radialMix3); break;
        case 'f': _outputImg = radialImgOp(radialColors, radialMix4); break;
        case 'g': _outputImg = radialImgOp(radialColors, radialMix5); break;
        case 'h': _outputImg = radialImgOp(radialColors, radialMix6); break;
        case 'j': _outputImg = radialImgOp(radialColors, radialMix7); break;
        case 'k': _outputImg = radialImgOp(radialColors, radialMix8); break;
        case 'l': _outputImg = radialImgOp(radialColors, radialMix9); break;
        case 'z': _outputImg = mandalaImgOp(mandalaMap, mandalaMix1, mandalaInterp1); break;
        case 'x': _outputImg = mandalaImgOp(mandalaMap, mandalaMix2, mandalaInterp2); break;
        case 'c': _outputImg = mandalaImgOp(mandalaMap, mandalaMix3, mandalaInterp3); break;
        case 'v': _outputImg = mandalaImgOp(mandalaMap, mandalaMix4, mandalaInterp4); break;
        case 'b': _outputImg = mandalaImgOp(mandalaMap, mandalaMix5, mandalaInterp5); break;
        case 'n': _outputImg = mandalaImgOp(mandalaMap, mandalaMix6, mandalaInterp6); break;
        case 'm': _outputImg = mandalaImgOp(mandalaMap, mandalaMix7, mandalaInterp7); break;
        default: segs = 0.0; dist = 0.0; break;
    }
}
void _onPressEvent(double x, double y){ }
void _onTickEvent(unsigned secs){}

int main(int argc, char** argv) {
	srand(time(NULL));
	if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    _outputImg = hypnosisImgOp(0, 0xFF00FF00, 0xFF0000FF);

    _run(argc, argv); // system specific initialization and continuous loop

    RASTERON_DEALLOC(_outputImg); // cleanup
    return 0;
}
