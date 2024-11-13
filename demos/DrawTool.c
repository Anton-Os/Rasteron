#include "catalouge.h"

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
static unsigned short drawIters = 1;

#include "Util_Runner.h"

PixelPointTable pixelPointTable;
ColorPointTable colorPointTable;

static double sinMod(double val){ return sin(val * drawIters); }
static double cosMod(double val){ return sin(val * drawIters); }
static double tanMod(double val){ return sin(val * drawIters); }
static double sinModX(double val){ return val * sin(val * drawIters); }
static double cosModX(double val){ return val * cos(val * drawIters); }
static double tanModX(double val){ return val * tan(val * drawIters); }
static double sinModD(double val){ return val / sin(val * drawIters); }
static double cosModD(double val){ return val / cos(val * drawIters); }
static double tanModD(double val){ return val / tan(val * drawIters); }

unsigned wavePaint(double x, double y){ return (sin(x * xFactor) > tan(y * yFactor))? xColor : yColor; } 

unsigned strokeDraw(double x, double y){
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
	double slope = (y1 - y2) / (x2 - x1);

	double dist = sqrt(pow(x2 - x1, 2.0) + pow(y2 - y1, 2.0));
    double dist1 = sqrt(pow(x - x1, 2.0) + pow(y - y1, 2.0));
    double dist2 = sqrt(pow(x - x2, 2.0) + pow(y - y2, 2.0));
	double cross = ((x - x1) * (x2 - x1)) - ((y - y1) * (y2 - y1));

	switch(mode){
		case 'q': return (((x > x1 && x < x2) || (x < x1 && x > x2)) && ((y > y1 && y < y2) || (y < y1 && y > y2)))? colors_blend(0xFF00FF00, canvasColor, (pow(fabs(xDiff - yDiff), 0.1))) : canvasColor;
		case 'w': return (xDiff > yDiff)? colorPoints[0].color : colorPoints[1].color;
		case 'e': return (x1 * sin(y1 * OSCILATION) < x2 * -y2)? colorPoints[0].color : colorPoints[1].color;
		case 'r': return (x1 * y1 < cos(x2 * OSCILATION) * y2)? colorPoints[0].color : colorPoints[1].color;
		case 't': return (tan(x1 * OSCILATION) * y1 < x2 * tan(-y2 * OSCILATION))? colorPoints[0].color : colorPoints[1].color;
		case 'y': return (xDiff / yDiff + x - y < yDiff / xDiff - x + y)? colorPoints[0].color : colorPoints[1].color;
		case 'u': return (fabs(xDiff - yDiff) < (0.25 * (x2 / y1 * y2 / x1)))? colorPoints[0].color : colorPoints[1].color;
		case 'i': return (xDiff / x1 / x2 / x3 / x4 > yDiff / y1 / y2 / y3 / y4)? colorPoints[0].color : colorPoints[1].color;
		case 'o': return (pow(x1 * x2 * x3 * x4, 1.0) > pow(y1 * y2 * y3 * y4, 1.0))? colorPoints[0].color : colorPoints[1].color;
		default: return wavePaint(x, y);
	}
}

static unsigned dotDraw(unsigned color, double distance, PixelPoint pixPoint){
    switch(mode){
		case '1': return (distance > dotSize * DOT_RADIUS)? canvasColor : color;
		case '2': return (distance > dotSize * DOT_RADIUS || distance < dotSize * (DOT_RADIUS / 3.0))? canvasColor : color;
        case '3': return (distance > dotSize * DOT_RADIUS || distance > (dotSize * DOT_RADIUS) + pixPoint.x)? canvasColor : color;
		case '4': return (distance > dotSize * DOT_RADIUS || distance > (dotSize * DOT_RADIUS) - pixPoint.y)? canvasColor : color;
		case '5': return (distance > dotSize * DOT_RADIUS || (fabs(pixPoint.x - sin(distance / 10)) * (dotSize / DOT_RADIUS) > dotSize * DOT_RADIUS))? canvasColor : color;
		case '6': return (distance > dotSize * DOT_RADIUS || (fabs(pixPoint.y * cos(distance * 10)) * (dotSize / DOT_RADIUS) > dotSize * DOT_RADIUS))? canvasColor : color;
		case '7': return (distance > dotSize * DOT_RADIUS || ((fabs(pixPoint.x) * DOT_RADIUS + fabs(pixPoint.y) * DOT_RADIUS) * (dotSize / DOT_RADIUS) > dotSize * DOT_RADIUS))? canvasColor : color;
		case '8': return (distance > (dotSize * DOT_RADIUS) * sin(dotSize / pixPoint.x * DOT_RADIUS) * cos(pixPoint.y / distance))? canvasColor : color;
		case '9': return (distance > (dotSize * DOT_RADIUS) * sin(dotSize / pixPoint.y * DOT_RADIUS) * cos(pixPoint.x / distance))? canvasColor : color;
		default: return (distance > dotSize * DOT_RADIUS * pow((double)rand() / (double)RAND_MAX, 0.25))? canvasColor : color;
    }
}

static unsigned fieldCompute(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]){
	switch(mode){
		case 'a': return colors_blend(xColor, yColor, distances[0] * FIELD_PRODUCT);
		case 's': return (distances[1] - distances[0] > 0.01)? xColor : yColor; // colors_blend(xColor, yColor, distances[1] * FIELD_PRODUCT);
		case 'd': return (distances[2] - distances[1] > 0.01)? xColor : yColor; // colors_blend(xColor, yColor, distances[2] * FIELD_PRODUCT);
		case 'f': return colors_blend(xColor, yColor, sin(distances[0] * FIELD_PRODUCT * 10.0));
		case 'g': return colors_blend(xColor, yColor, cos(distances[0] * FIELD_PRODUCT * 10.0));
		case 'h': return colors_blend(xColor, yColor, tan(distances[0] * FIELD_PRODUCT * 10.0));
		case 'j': return (distances[2] > distances[0] + distances[1])? xColor : yColor;
        case 'k': return (pixPoints[0].x / pixPoints[1].y > pow(pixPoints[1].x, pixPoints[0].y))? xColor : yColor;
		default: return (((distances[2] + distances[1] + distances[0]) / 3) > distances[1])? xColor : yColor;
	}
}

Rasteron_Image* drawImgOp(Rasteron_Image* targetImg, PixelPointTable* pointsTable, unsigned short count, double (*xModCallback)(double), double (*yModCallback)(double)){
	drawIters = count;

	unsigned color1 = _swatch.colors[count % 8];
	unsigned color2 = _swatch.colors[(count + 1) % 8];
	unsigned color3 = _swatch.colors[(count - 1) % 8];

    Rasteron_Image* drawImg = (targetImg != NULL)? copyImgOp(targetImg) : checkeredImgOp((ImageSize){ 1024, 1024 }, (ColorGrid){ _dimens[0], _dimens[1], color2 });

	for(unsigned p = 0; p < 1024 * 1024; p++){
		double x = (1.0 / (double)1024) * (p % 1024); // (1.0 / (double)1024) * (p % 1024);
        double y = (1.0 / (double)1024) * (p / 1024); // (1.0 / (double)1024) * (p / 1024)
        if(xModCallback != NULL) x = xModCallback(x);
		if(yModCallback != NULL) y = yModCallback(y);

        double xOffLo = 9999.99, yOffLo = 9999.99; // lowest value
        double xOffHi = 0.0, yOffHi = 0.0; // highest value
		double xAccum = 0.0; double yAccum = 0.0; // accumulated value

        // for(unsigned o = pointsTable->pointCount; o > (count < pointsTable->pointCount - 1)? count : pointsTable->pointCount - 1; o--){
        for(unsigned o = pointsTable->pointCount; o > count; o--){
            double xOff = x - pointsTable->points[o - 1].x;
            double yOff = y - pointsTable->points[o - 1].y;
			double angle = atan(((yOff + 1.0) * 0.5) / ((xOff + 1.0) * 0.5));
			double distance = sqrt(pow(xOff, 2) + pow(yOff, 2));

			if(o == 0 || fabs(xOff) < xOffLo) xOffLo = xOff; // minimum x value
			if(o == 0 || fabs(xOff) > xOffHi) xOffHi = xOff; // maximum x value
			if(o == 0 || fabs(yOff) < yOffLo) yOffLo = yOff; // minimum y value
			if(o == 0 || fabs(yOff) > yOffHi) yOffHi = yOff; // maximum y value
            xAccum += fabs(xOff); yAccum += fabs(yOff);

			/* switch(mode){
				case 'z': *(drawImg->data + p) = colors_blend(colors_blend(color1, color_invert(color1), fabs(xAccum)), colors_blend(color2, color_invert(color2), fabs(yAccum)), fabs(xAccum / yAccum)); break;
				case 'x': if(fabs(xOffLo) < fabs(yOffLo)) *(drawImg->data + p) = color1; else if(fabs(xOffHi) > fabs(yOffHi)) *(drawImg->data + p) = color2; break;
				case 'c': *(drawImg->data + p) = colors_blend(color1, color2, (xAccum / yOffHi) * (xOffLo / yAccum)); break;
				case 'v': *(drawImg->data + p) = colors_blend(color1, color2, (xOffLo * yOffLo) / (xOffHi * yOffHi)); break;
				case 'b': *(drawImg->data + p) = colors_fuse(color1, color2, (xOffHi / yOffLo) * sin(pow(xOffLo / yOffHi, 2) + pow(yOffLo / xOffHi, 2))); break;
				case 'n': *(drawImg->data + p) = (xOff / yOff > 0.5)? colors_blend(*(drawImg->data + p), color1, xOffHi) : colors_blend(*(drawImg->data + p), color2, yOffHi); break;
                case 'm': *(drawImg->data + p) = (color1 / color2) * (xOffLo * xAccum) / (yOff * yAccum); break;
				default: *(drawImg->data + p) = *(drawImg->data + p);
			} */

			switch(mode){
                case 'z': if(o == pointsTable->pointCount) *(drawImg->data + p) = colors_blend(color1, color2, angle); break;
				case 'x': *(drawImg->data + p) = colors_blend(*(drawImg->data + p), color2, pow(angle, count)); break;
				case 'c': *(drawImg->data + p) = colors_blend(*(drawImg->data + p), color3, angle / distance); break;
				case 'v': *(drawImg->data + p) = colors_blend(*(drawImg->data + p), color1, (distance - angle) / (angle + distance)); break;
				case 'b': *(drawImg->data + p) = (angle > distance)? color1 : color2; break;
				case 'n': *(drawImg->data + p) = colors_fuse(color1, color3, sin(angle) + cos(distance)); break;
				case 'm': *(drawImg->data + p) = colors_fuse(colors_blend(color1, color2, angle), colors_blend(color1, color3, distance), 1.0 / (xAccum + yAccum)); break;
			}
        }
	}

	return drawImg;
}

void _onKeyEvent(char key){
    static double (*xMod)(double) = &sinMod;
    static double (*yMod)(double) = &cosMod;
	mode = key;


    if(colorPointTable.pointCount > COLOR_POINTS && isalpha(key))
        switch(key){
            case 'q': case 'w': case 'e': case 'r': case 't': case 'y': case 'u': case 'i': case 'o': case 'p':
                _outputImg = mapImgOp((ImageSize){1024, 1024}, strokeDraw);
            break;
            case 'a': case 's': case 'd': case 'f': case 'g': case 'h': case 'j': case 'k': case 'l':
                _outputImg = fieldExtImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, fieldCompute);
            break;
            case 'z': case 'x': case 'c': case 'v': case 'b': case 'n': case 'm':
                _outputImg = drawImgOp(_savedImg, &pixelPointTable, pixelPointTable.pointCount - 4, xMod, yMod);
            break;
        }
    else if(key == '<'){
        if(xMod == NULL) xMod = &sinMod;
        else if(xMod == &sinMod) xMod = &sinModX;
        else if(xMod == &sinModX) xMod = &sinModD;
        else xMod = NULL;
    } else if(key == '>'){
        if(yMod == NULL) yMod = &cosMod;
        else if(yMod == &cosMod) yMod = &cosModX;
        else if(yMod == &cosModX) yMod = &cosModD;
        else yMod = NULL;
    }

	else _outputImg = fieldImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, dotDraw);
}

void _onPressEvent(double x, double y){ 
	xFactor = x * OSCILATION; 
	yFactor = y * OSCILATION;
	xColor = 0xFF00FF00 | ((unsigned)(x * 256) * 0x10001); 
	yColor = 0xFFFF0000 | ((unsigned)(y * 256) * 0x101); 

	pixelPointToTable(&pixelPointTable, x, y);
	colorPointToTable(&colorPointTable, (colorPointTable.pointCount % 2 == 0)? xColor : yColor, x, y);
}

void _onTickEvent(unsigned secs){}

int main(int argc, char** argv){
	pixelPointTable.pointCount = 0;
	colorPointTable.pointCount = 0;

	if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    _outputImg = mapImgOp((ImageSize){1024, 1024}, wavePaint); // global canvas for drawing

	_run(argc, argv, NULL); // system specific initialization and continuous loop

    RASTERON_DEALLOC(_outputImg); // cleanup
    return 0;
}
