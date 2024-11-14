#include "_Catalouge.h"

#define RADIAL_SEGS 30.0
#define RADIAL_DIST 30.0

#include "Util_Runner.h"

static unsigned spirals(double x, double y){
    double centerAngle = atan((y - 0.5) / (x - 0.5));
    double centerDist = sqrt(pow(x - 0.5, 2) + pow(y - 0.5, 2));

    // return colors_fuse(0xFF111111, 0xFFEEEEEE, ((centerAngle * RADIAL_SEGS) - floor(centerAngle * RADIAL_SEGS) * (centerDist * RADIAL_DIST)));
    return colors_fuse(color_level(0xFF111111, sin(centerAngle * RADIAL_DIST)), color_level(0xFFEEEEEE, cos(centerDist * RADIAL_DIST)), ((centerAngle * RADIAL_SEGS) - floor(centerAngle * RADIAL_SEGS) * tan(centerDist * RADIAL_DIST)));
}

Rasteron_Image* radialImgOp(double r){
    Rasteron_Image* radialImgs[4] = {
        gradientImgOp((ImageSize){ 1024, 1024 }, SIDE_Radial, 0xFF33AAFF, 0xFF991166),
        gradientImgOp((ImageSize){ 1024, 1024 }, SIDE_Radial, 0xFF33AAFF, 0xFF991166),
        gradientImgOp((ImageSize){ 1024, 1024 }, SIDE_Radial, 0xFF33AAFF, 0xFF991166),
        gradientImgOp((ImageSize){ 1024, 1024 }, SIDE_Radial, 0xFF33AAFF, 0xFF991166)
    };
    
    // TODO: Create a custom radial image

    for(unsigned i = 0; i < 4; i++) RASTERON_DEALLOC(radialImgs[i]);

    return mapImgOp((ImageSize){ 1024, 1024 }, spirals);
}

void _onKeyEvent(char key){ }
void _onPressEvent(double x, double y){ }
void _onTickEvent(unsigned secs){}

int main(int argc, char** argv) {
	srand(time(NULL));
	if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    _outputImg = radialImgOp(10000.0); // hypnosisImgOp(0, 0xFF00FF00, 0xFF0000FF);

	_run(argc, argv, NULL); // system specific initialization and continuous loop

    RASTERON_DEALLOC(_outputImg); // cleanup
    return 0;
}
