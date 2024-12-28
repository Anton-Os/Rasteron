#include "_Catalouge.h"

#define RADIAL_SEGS 30.0
#define RADIAL_DIST 30.0

#include "_Demo.h"

double segs = RADIAL_SEGS;
double dist = RADIAL_DIST;
unsigned color1 = 0xFF111188;
unsigned color2 = 0xFF88EEEE;

static char keysave = '0';


unsigned radialMix(unsigned c1, unsigned c2, unsigned c3, unsigned c4){
    return colors_diff(colors_diff(c1 , c4), c3 - c2);
}

Rasteron_Image* radialImgOp(unsigned colors[4]){
    Rasteron_Image* radialImgs[4] = {
        gradientImgOp((ImageSize){ 1024, 1024 }, SIDE_Radial, colors[0], colors[1]),
        gradientImgOp((ImageSize){ 1024 / 2, 1024 / 2 }, SIDE_Radial, colors[1], colors[2]),
        gradientImgOp((ImageSize){ 1024 / 4, 1024 / 4 }, SIDE_Radial, colors[2], colors[3]),
        gradientImgOp((ImageSize){ 1024 / 6, 1024 / 6 }, SIDE_Radial, colors[3], colors[0])
    };

    Rasteron_Image* radialImg = mixingExtImgOp(radialImgs[0], radialImgs[1], radialImgs[2], radialImgs[3], radialMix);

    for(unsigned i = 0; i < 4; i++) RASTERON_DEALLOC(radialImgs[i]);

    return radialImg;
}

static unsigned spirals(double x, double y){
    double centerAngle = atan((y - 0.5) / (x - 0.5));
    double centerDist = sqrt(pow(x - 0.5, 2) + pow(y - 0.5, 2));

    double factor = (centerAngle * segs) - floor(centerAngle * segs) * tan(centerDist * dist);
    return colors_fuse(color_level(color1, sin(centerAngle * dist)), color_level(color2, cos(centerDist * dist)), factor);
}

Rasteron_Image* spiralsImgOp(double s, double d){
    segs = s;
    dist = d;

    return mapImgOp((ImageSize){ 1024, 1024 }, spirals);
}

static unsigned mandalaMap(double x, double y){
    static unsigned invocation = 0;

    double centerAngle = atan((y - 0.5) / (x - 0.5));
    double centerDist = sqrt(pow(x - 0.5, 2) + pow(y - 0.5, 2));

    // x *= pow((x - 0.5), 1.0 / 3.0);
    // y *= pow((y - 0.5), 1.0 / 3.0);

    unsigned mandalaColor = colors_blend(
        colors_fuse(color1, color2, sin(y / centerAngle) + cos(x / centerDist)), 
        colors_diff(color_invert(color1), color_invert(color2)),
        tan((x + y) / invocation) * pow(centerAngle, centerDist)
    );

    invocation++;
    return mandalaColor;
}

static unsigned mandalaMix(unsigned c1, unsigned c2){ return c1 + c2; }
static unsigned restoreMix(unsigned c1, unsigned c2){ return (c1 == 0xFF000000 || c1 == NO_COLOR)? c2 : c1; }

Rasteron_Image* mandalaImgOp(coordCallback coord_callback, mixCallback mix_callback){
    Rasteron_Image* mapImg1 = mapImgOp((ImageSize){ 1024, 1024 }, coord_callback);
    Rasteron_Image* mapImg2 = mapImgOp((ImageSize){ 1024, 1024 }, coord_callback);

    Rasteron_Image* symetricImg1 = mixingImgOp(mapImg1, mapImg2, mix_callback);
    Rasteron_Image* symetricImg2 = flipImgOp(symetricImg1, FLIP_Clock);

    Rasteron_Image* mandalaImg = mixingImgOp(symetricImg1, symetricImg2, restoreMix);

    RASTERON_DEALLOC(mapImg1); RASTERON_DEALLOC(mapImg2);
    RASTERON_DEALLOC(symetricImg1); RASTERON_DEALLOC(symetricImg2);

    return mandalaImg;
}

void _onKeyEvent(char key){
    if(isalpha(key)) RASTERON_DEALLOC(_outputImg);

    switch(tolower(key)){
        case 'q': case 'w': case 'e': case 'r': case 't': case 'y': case 'u': case 'i': case 'o': case 'p':
            unsigned radialColors[4] = { RAND_COLOR(), RAND_COLOR(), RAND_COLOR(), RAND_COLOR() };
            _outputImg = radialImgOp(radialColors);
            break;
        case 'a': case 's': case 'd': case 'f': case 'g': case 'h': case 'j': case 'k': case 'l':
            color1 = RAND_COLOR(); color2 = RAND_COLOR();
            _outputImg = spiralsImgOp(((float)rand() / (float)RAND_MAX) * RADIAL_SEGS, ((float)rand() / (float)RAND_MAX) * RADIAL_DIST);
            break;
        case 'z': case 'x': case 'c': case 'v': case 'b': case 'n': case 'm':
            color1 = RAND_COLOR(); color2 = RAND_COLOR();
            _outputImg = mandalaImgOp(mandalaMap, mandalaMix);
            break;
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
