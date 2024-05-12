#include "Experimental.h"

#define CANVAS_COLOR 0xFF111111
#define COLOR_POINTS 4
#define OSCILATION 30.0
#define DOT_RADIUS 0.075
#define FIELD_PRODUCT 5.0

static double xFactor = 10.0; 
static double yFactor = 10.0;
static unsigned dotSize = 1.0;
static unsigned isCoords = CANVAS_COLOR;
static unsigned xColor = 0xFFFFFF00; 
static unsigned yColor = 0xFFFF00FF;
static unsigned canvasColor = CANVAS_COLOR;
static short coordSpace = 1;
static unsigned short mode = 0;

#include "Util_Demo.h"

PixelPointTable pixelPointTable;
ColorPointTable colorPointTable;

Rasteron_Image* canvasImg;

unsigned wavePaint(double x, double y){ return (sin(x * xFactor) > tan(y * yFactor))? xColor : yColor; } 

unsigned strokePaint(double x, double y){
	x *= 0.5 * coordSpace; 
	y *= 0.5 * coordSpace;

	ColorPoint colorPoints[COLOR_POINTS]; // TODO: Add color points
	for(unsigned short c = 0; c < COLOR_POINTS; c++)
		colorPoints[c] = colorPointTable.points[colorPointTable.pointCount - 1 - c];

	double x1 = colorPoints[0].x - x; double y1 = colorPoints[0].y - y;
	double x2 = colorPoints[1].x - x; double y2 = colorPoints[1].y - y;
	double x3 = colorPoints[2].x - x; double y3 = colorPoints[2].y - y;
	double x4 = colorPoints[3].x - x; double y4 = colorPoints[3].y - y;

	double xDiff = (x - x2) / (x1 - x2);
	double yDiff = (y - y2) / (y1 - y2);

	switch(mode){
		case 1: return (xDiff > yDiff)? colorPoints[0].color : colorPoints[1].color;
			// if(((x > x1 && x < x2) || (x < x1 && x > x2)) && ((y > y1 && y < y2) || (y < y1 && y > y2))) return (xDiff > yDiff)? colorPoints[0].color : colorPoints[1].color;
			// else return canvasColor;
		case 2: return (x1 * sin(y1 * OSCILATION) < x2 * -y2)? colorPoints[0].color : colorPoints[1].color;
		case 3: return (x1 * y1 < cos(x2 * OSCILATION) * y2)? colorPoints[0].color : colorPoints[1].color;
		case 4: return (tan(x1 * OSCILATION) * y1 < x2 * tan(-y2 * OSCILATION))? colorPoints[0].color : colorPoints[1].color;
		case 5: return (xDiff / yDiff + x - y < yDiff / xDiff - x + y)? colorPoints[0].color : colorPoints[1].color;
		case 6: 
			if(((x > x1 && x < x2) || (x < x1 && x > x2)) && ((y > y1 && y < y2) || (y < y1 && y > y2))) return colors_blend(0xFF00FF00, canvasColor, (pow(fabs(xDiff - yDiff), 0.1)));
			else return canvasColor;
		case 7: return (xDiff / x1 / x2 / x3 / x4 > yDiff / y1 / y2 / y3 / y4)? colorPoints[0].color : colorPoints[1].color;
		case 8: return (pow(x1 * x2 * x3 * x4, 1.0) > pow(y1 * y2 * y3 * y4, 1.0))? colorPoints[0].color : colorPoints[1].color;
		default: return colors_blend(colorPoints[0].color, colorPoints[1].color, 0.5);
	}
}

static unsigned dotSplash(unsigned color, double distance){
    return (distance > dotSize * DOT_RADIUS * pow((double)rand() / (double)RAND_MAX, 0.25))? canvasColor : color;
}

static unsigned fieldCompute(unsigned colors[3], double distances[3]){
	switch(mode){
		case 9: return colors_blend(xColor, yColor, distances[0] * FIELD_PRODUCT);
		case 10: return colors_blend(xColor, yColor, distances[1] * FIELD_PRODUCT);
		case 11: return colors_blend(xColor, yColor, distances[2] * FIELD_PRODUCT);
		case 12: return colors_blend(xColor, yColor, sin(distances[0] * FIELD_PRODUCT * 10.0));
		case 13: return colors_blend(xColor, yColor, cos(distances[0] * FIELD_PRODUCT * 10.0));
		case 14: return colors_blend(xColor, yColor, tan(distances[0] * FIELD_PRODUCT * 10.0));
		case 15: return (distances[2] > distances[0] + distances[1])? xColor : yColor;
		case 16: return (((distances[2] + distances[1] + distances[0]) / 3) > distances[1])? xColor : yColor;
		default: return colors[0];
	}
}

static void update(){
	if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
	if(colorPointTable.pointCount <= COLOR_POINTS || mode == 0) _outputImg = fieldImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, dotSplash);
	else if(mode > 0 && mode < 9) _outputImg = mapImgOp((ImageSize){1024, 1024}, strokePaint);
	else if(mode > 8 && mode < 17) _outputImg = fieldExtImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, fieldCompute);
	else _outputImg = mapImgOp((ImageSize){1024, 1024}, wavePaint);
}

void _onKeyEvent(char key){
	if(key - '0' >= 0 && key - '0' < 10) coordSpace = key - '0';
	switch(key){
		case 'q': mode = 0; break; case 'w': mode = 1; break; case 'e': mode = 2; break;
		case 'r': mode = 3; break; case 't': mode = 4; break; case 'y': mode = 5; break;
		case 'u': mode = 6; break; case 'i': mode = 7; break; case 'o': mode = 8; break;
		case 'p': mode = 9; break; case 'a': mode = 10; break; case 's': mode = 11; break;
		case 'd': mode = 12; break; case 'f': mode = 13; break; case 'g': mode = 14; break;
		case 'h': mode = 13; break; case 'j': mode = 14; break; case 'k': mode = 15; break;
		case 'l': mode = 16; break; case 'm': mode = 17; break;
		// default: mode = -1;
	}
	if(isspace(key)) mode = (mode + 1) % 18;

	update();
}

void _onPressEvent(double x, double y){ 
	xFactor = x * OSCILATION; 
	yFactor = y * OSCILATION;
	xColor = 0xFF006600 | ((unsigned)(x * 256) * 0x10001); 
	yColor = 0xFF660000 | ((unsigned)(y * 256) * 0x101); 

	pixelPointToTable(&pixelPointTable, x, y);
	colorPointToTable(&colorPointTable, (colorPointTable.pointCount % 2 == 0)? xColor : yColor, x, y);

	update();
}

void _onTickEvent(unsigned secs){}

int main(int argc, char** argv){
	canvasImg = solidImgOp((ImageSize){ 1024, 1024 }, canvasColor);

	pixelPointTable.pointCount = 0;
	colorPointTable.pointCount = 0;

	if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    _outputImg = mapImgOp((ImageSize){1024, 1024}, wavePaint); // global canvas for drawing

	_run(argc, argv, NULL); // system specific initialization and continuous loop

    RASTERON_DEALLOC(_outputImg); // cleanup
    return 0;
}