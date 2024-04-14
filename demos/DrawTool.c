#include "Experimental.h"

#define OSCILATION 30.0

static double xFactor = 10.0; 
static double yFactor = 10.0;
static unsigned color1 = 0xFFFFFF00; 
static unsigned color2 = 0xFFFF00FF;

unsigned startPaint(double x, double y){ return (sin(x * xFactor) > tan(y * yFactor))? color1 : color2; } 

#include "Util_Demo.h"

PixelPointTable pixelPointTable;

void _onKeyEvent(char key){ printf("Key entered: %c", key); }
void _onPressEvent(double x, double y){ 
	pixelPointToTable(&pixelPointTable, x, y);
	printf("Cursor data is (%f, %f)", x, y);

	xFactor = x * OSCILATION; 
	yFactor = y * OSCILATION;
	color1 = 0xFF006600 | ((unsigned)(x * 256) * 0x10001); 
	color2 = 0xFF660000 | ((unsigned)(y * 256) * 0x101); 

	if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
	_outputImg = mapImgOp((ImageSize){1024, 1024}, startPaint);

	// TODO: Perform advanced drawing oparations
}
void _onTickEvent(unsigned secs){}

int main(int argc, char** argv){
	if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    _outputImg = mapImgOp((ImageSize){1024, 1024}, startPaint); // global canvas for drawing

	_run(); // system specific initialization and continuous loop

    RASTERON_DEALLOC(_outputImg); // cleanup
    return 0;
}