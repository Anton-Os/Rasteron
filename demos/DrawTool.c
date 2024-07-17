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
static unsigned short mode = 0;

#include "Util_Demo.h"

PixelPointTable pixelPointTable;
ColorPointTable colorPointTable;

Rasteron_Image* canvasImg;

unsigned wavePaint(double x, double y){ return (sin(x * xFactor) > tan(y * yFactor))? xColor : yColor; } 

unsigned strokePaint(double x, double y){
	x *= 0.5 * _dimens[0]; 
	y *= 0.5 * _dimens[1];

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
		case 6: return (fabs(xDiff - yDiff) < (0.25 * (x2 / y1 * y2 / x1)/*(xDiff / yDiff)*/))? colorPoints[0].color : colorPoints[1].color;
			// if(((x > x1 && x < x2) || (x < x1 && x > x2)) && ((y > y1 && y < y2) || (y < y1 && y > y2))) return colors_blend(0xFF00FF00, canvasColor, (pow(fabs(xDiff - yDiff), 0.1)));
			// else return canvasColor;
		case 7: return (xDiff / x1 / x2 / x3 / x4 > yDiff / y1 / y2 / y3 / y4)? colorPoints[0].color : colorPoints[1].color;
		case 8: return (pow(x1 * x2 * x3 * x4, 1.0) > pow(y1 * y2 * y3 * y4, 1.0))? colorPoints[0].color : colorPoints[1].color;
		default: return wavePaint(x, y);
	}
}

static unsigned dotSplash(unsigned color, double distance, PixelPoint pixPoint){
    return (distance > dotSize * DOT_RADIUS * pow((double)rand() / (double)RAND_MAX, 0.25))? canvasColor : color;
}

static unsigned fieldCompute(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]){
	switch(mode){
		case 10: return colors_blend(xColor, yColor, distances[0] * FIELD_PRODUCT);
		case 11: return colors_blend(xColor, yColor, distances[1] * FIELD_PRODUCT);
		case 12: return colors_blend(xColor, yColor, distances[2] * FIELD_PRODUCT);
		case 13: return colors_blend(xColor, yColor, sin(distances[0] * FIELD_PRODUCT * 10.0));
		case 14: return colors_blend(xColor, yColor, cos(distances[0] * FIELD_PRODUCT * 10.0));
		case 15: return colors_blend(xColor, yColor, tan(distances[0] * FIELD_PRODUCT * 10.0));
		case 16: return (distances[2] > distances[0] + distances[1])? xColor : yColor;
		default: return (((distances[2] + distances[1] + distances[0]) / 3) > distances[1])? xColor : yColor;
	}
}

Rasteron_Image* drawImgOp(/* TODO: Add parameters */){
	static unsigned swatchOffset = SWATCH_Light;
	unsigned color1 = _swatch.colors[swatchOffset % 8];
	unsigned color2 = _swatch.colors[(swatchOffset + 1) % 8];

	Rasteron_Image* drawImg = checkeredImgOp((ImageSize){ 1024, 1024 }, (ColorGrid){ _dimens[0], _dimens[1], color2 });

	for(unsigned p = 0; p < 1024 * 1024; p++){
		double x = (1.0 / (double)1024) * (p % 1024);
		double y = (1.0 / (double)1024) * (p / 1024);

		double xOffLo = 0.0; double yOffLo = 0.0; // lowest value
		double xOffHi = 0.0; double yOffHi = 0.0; // highest value
		double xAccum = 0.0; double yAccum = 0.0; // accumulated value

		for(unsigned o = 0; o < pixelPointTable.pointCount; o++){
			double xOff = x - pixelPointTable.points[o].x;
			double yOff = y - pixelPointTable.points[o].y;

			if(o == 0 || fabs(xOff) < xOffLo) xOffLo = xOff; // minimum x value
			if(o == 0 || fabs(xOff) > xOffHi) xOffHi = xOff; // maximum x value
			if(o == 0 || fabs(yOff) < yOffLo) yOffLo = yOff; // minimum y value
			if(o == 0 || fabs(yOff) > yOffHi) yOffHi = yOff; // maximum y value
			xAccum += xOff; yAccum += yOff;

			switch(mode){
			case 17: if(sin(xOff * 10.0) > cos(yOff * 10.0)) *(drawImg->data + p) += 0xF; break;
			// case 18: if(xOff < 0.1) *(drawImg->data + p) += 0xF; else if(yOff < 0.1) *(drawImg->data + p) -= 0xF; break;
			case 18: *(drawImg->data + p) = colors_blend(colors_blend(color1, color_invert(color1), fabs(xAccum)), colors_blend(color2, color_invert(color2), fabs(yAccum)), fabs(xAccum / yAccum));
			case 19: if(fabs(xOffLo) < fabs(yOffLo)) *(drawImg->data + p) = color1; else if(fabs(xOffHi) > fabs(yOffHi)) *(drawImg->data + p) = color2; break;
			case 20: *(drawImg->data + p) = colors_blend(color1, color2, xOff / yOff); break;
			case 21: *(drawImg->data + p) = colors_blend(color1, color2, (xOffLo * yOffLo) / (xOffHi * yOffHi)); break;
			case 22: *(drawImg->data + p) = colors_fuse(color1, color2, (x / y) * sin(pow(xOffLo / yOffHi, 2) + pow(yOffLo / xOffHi, 2))); break;
			case 23: *(drawImg->data + p) = (xOff / yOff > 0.5)? colors_blend(*(drawImg->data + p), color1, xOffHi) : colors_blend(*(drawImg->data + p), color2, yOffHi);
			default: *(drawImg->data + p) = *(drawImg->data + p);
			}
		}
	}

	swatchOffset++;

	return drawImg;
}

static void update(){
	if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
	if(colorPointTable.pointCount > COLOR_POINTS){
		if(mode > 0 && mode < 10) _outputImg = mapImgOp((ImageSize){1024, 1024}, strokePaint);
		else if(mode >= 10 && mode < 17) _outputImg = fieldExtImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, fieldCompute);
		else if(mode < 24) _outputImg = drawImgOp();
	}
	else _outputImg = fieldImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, dotSplash);
}

void _onKeyEvent(char key){
	// if(key - '0' >= 0 && key - '0' < 10) coordSpace = key - '0'
	switch(key){
		case 'q': mode = 0; break; case 'w': mode = 1; break; case 'e': mode = 2; break;
		case 'r': mode = 3; break; case 't': mode = 4; break; case 'y': mode = 5; break;
		case 'u': mode = 6; break; case 'i': mode = 7; break; case 'o': mode = 8; break;
		case 'p': mode = 9; break; 
		case 'a': mode = 10; break; case 's': mode = 11; break; case 'd': mode = 12; break; 
		case 'f': mode = 13; break; case 'g': mode = 14; break; case 'h': mode = 13; break; 
		case 'j': mode = 14; break; case 'k': mode = 15; break; case 'l': mode = 16; break; 
		case 'z': mode = 17; break; case 'x': mode = 18; break; case 'c': mode = 19; break;
		case 'v': mode = 20; break; case 'b': mode = 21; break; case 'n': mode = 22; break;
		case 'm': mode = 23;
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