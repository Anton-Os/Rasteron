#include "_Catalouge.h"

#define RADIAL_SEGS 30.0
#define RADIAL_DIST 30.0

#include "Util_Runner.h"

double segs = RADIAL_SEGS;
double dist = RADIAL_DIST;
unsigned color1 = 0xFF111188;
unsigned color2 = 0xFF88EEEE;

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

unsigned radialMix(unsigned c1, unsigned c2, unsigned c3, unsigned c4){
    return colors_diff(colors_diff(c1 , c4), c3 - c2);
}

Rasteron_Image* radialImgOp(){
    Rasteron_Image* radialImgs[4] = {
        gradientImgOp((ImageSize){ 1024, 1024 }, SIDE_Radial, RAND_COLOR(), RAND_COLOR()),
        gradientImgOp((ImageSize){ 1024 / 2, 1024 / 2 }, SIDE_Radial, RAND_COLOR(), RAND_COLOR()),
        gradientImgOp((ImageSize){ 1024 / 4, 1024 / 4 }, SIDE_Radial, RAND_COLOR(), RAND_COLOR()),
        gradientImgOp((ImageSize){ 1024 / 6, 1024 / 6 }, SIDE_Radial, RAND_COLOR(), RAND_COLOR())
    };

    Rasteron_Image* radialImg = mixingExtImgOp(radialImgs[0], radialImgs[1], radialImgs[2], radialImgs[3], radialMix);

    for(unsigned i = 0; i < 4; i++) RASTERON_DEALLOC(radialImgs[i]);

    return radialImg;
}

void _onKeyEvent(char key){
    switch(tolower(key)){
        case 'q': segs = 10.0; dist = 10.0; break;
        case 'w': segs = 20.0; dist = 20.0; break;
        case 'e': segs = 30.0; dist = 30.0; break;
        default: segs = 0.0; dist = 0.0; break;
    }

    if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    if(segs != 0.0 && dist != 0.0) _outputImg = spiralsImgOp(segs, dist);
    else _outputImg = radialImgOp();
}
void _onPressEvent(double x, double y){ }
void _onTickEvent(unsigned secs){}

int main(int argc, char** argv) {
	srand(time(NULL));
	if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    _outputImg = radialImgOp(RADIAL_SEGS, RADIAL_DIST); // hypnosisImgOp(0, 0xFF00FF00, 0xFF0000FF);

	_run(argc, argv, NULL); // system specific initialization and continuous loop

    RASTERON_DEALLOC(_outputImg); // cleanup
    return 0;
}
