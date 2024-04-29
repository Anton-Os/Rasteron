#include "Experimental.h"

#define OSCILATION 30.0
#define DOT_RADIUS 0.075
#define FIELD_PRODUCT 5.0

static double xFactor = 10.0; 
static double yFactor = 10.0;
static unsigned dotSize = 1.0;
static unsigned canvasColor = 0xFF333333;
static unsigned xColor = 0xFFFFFF00; 
static unsigned yColor = 0xFFFF00FF;
static unsigned short mode = 0;

unsigned startPaint(double x, double y){ return (sin(x * xFactor) > tan(y * yFactor))? xColor : yColor; } 

static unsigned dotSplash(unsigned color, double distance){
    return (distance > dotSize * DOT_RADIUS * pow((double)rand() / (double)RAND_MAX, 0.25))? canvasColor : color;
}

/* static unsigned dotTest(unsigned colors[3], double distances[3]){
	return (distances[0] > dotSize * DOT_RADIUS * pow((double)rand() / (double)RAND_MAX, 0.25))? canvasColor : colors[0];
} */

static unsigned fieldTest(unsigned colors[3], double distances[3]){
	switch(mode){
		case 1: return colors[1];
		case 2: return colors[2];
		case 3: return colors_blend(xColor, yColor, distances[0] * FIELD_PRODUCT);
		case 4: return colors_blend(xColor, yColor, distances[1] * FIELD_PRODUCT);
		case 5: return colors_blend(xColor, yColor, distances[2] * FIELD_PRODUCT);
		case 6: return colors_blend(xColor, yColor, sin(distances[0] * FIELD_PRODUCT * 10.0));
		case 7: return colors_blend(xColor, yColor, cos(distances[0] * FIELD_PRODUCT * 10.0));
		case 8: return colors_blend(xColor, yColor, tan(distances[0] * FIELD_PRODUCT * 10.0));
		// case 6: return (distances[2] > distances[0] + distances[1])? xColor : yColor;
		// case 7: return (((distances[2] + distances[1] + distances[0]) / 3) > distances)? xColor : yColor;
		default: return colors[0];
	}
}

#include "Util_Demo.h"

PixelPointTable pixelPointTable;
ColorPointTable colorPointTable;

void _onKeyEvent(char key){ 
	/* switch(key){
		case 'q': mode = 0; break; case 'w': mode = 1; break; case 'e': mode = 2;
		case 'r': mode = 3; break; case 't': mode = 4; break; case 'y': mode = 5;
		case 'u': mode = 4; break; case 'i': mode = 5; break; case 'o': mode = 6;
		case 'p': mode = 7; break; case 'a': mode = 8; break; case 's': mode = 9;
		// default: mode = -1;
	} */
	if(isspace(key)) mode = (mode + 1) % 10;

	if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
	if(mode >= 0 && mode < 9) _outputImg = fieldExtImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, fieldTest);
	else _outputImg = fieldImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, dotSplash);
}

void _onPressEvent(double x, double y){ 
	pixelPointToTable(&pixelPointTable, x, y);
	colorPointToTable(&colorPointTable, RAND_COLOR(), x, y);

	xFactor = x * OSCILATION; 
	yFactor = y * OSCILATION;
	xColor = 0xFF006600 | ((unsigned)(x * 256) * 0x10001); 
	yColor = 0xFF660000 | ((unsigned)(y * 256) * 0x101); 

	if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
	// _outputImg = mapImgOp((ImageSize){1024, 1024}, startPaint);
	// _outputImg = vornoiImgOp((ImageSize){ 1024, 1024 }, &colorPointTable);
	// _outputImg = fieldImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, dotSplash);
	_outputImg = fieldExtImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, fieldTest);

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