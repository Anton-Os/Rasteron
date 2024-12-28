#include "_Catalouge.h"

#define CANVAS_COLOR 0xFF111111
#define COLOR_POINTS 4
#define OSCILATION 30.0
#define DOT_RADIUS 0.05
#define FIELD_PRODUCT 5.0
#define STROKE_MOD 5.0

static unsigned dotSize = 1.0;
static unsigned isCoords = CANVAS_COLOR;
static unsigned xColor = 0xFFFFFF00;
static unsigned yColor = 0xFFFF00FF;
static unsigned short mode = '1';
static unsigned short pressCount = 0;

#include "_Demo.h"

PixelPointTable pixelPointTable;
ColorPointTable colorPointTable;

/* static double sinMod(double val){ return sin(val * STROKE_MOD); }
static double cosMod(double val){ return sin(val * STROKE_MOD); }
static double tanMod(double val){ return sin(val * STROKE_MOD); }
static double sinModX(double val){ return val * sin(val * STROKE_MOD); }
static double cosModX(double val){ return val * cos(val * STROKE_MOD); }
static double tanModX(double val){ return val * tan(val * STROKE_MOD); }
static double sinModD(double val){ return val / sin(val * STROKE_MOD); }
static double cosModD(double val){ return val / cos(val * STROKE_MOD); }
static double tanModD(double val){ return val / tan(val * STROKE_MOD); }

static double (*xMod)(double) = &sinMod;
static double (*yMod)(double) = &cosMod; */

static double basicLine(double x, double y, double x1, double y1, double x2, double y2){
    return fabs(((y2 - y1) * x) - ((x2 - x1) * y) + (x2 * y1) - (y2 * x1)) / sqrt(pow(y2 - y1, 2.0) + pow(x2 - x1, 2.0));
}

static double wobbleLine(double x, double y, double x1, double y1, double x2, double y2){
    x *= 1.0 + sin(x); y *= 1.0 + sin(y);
    return fabs(((y2 - y1) * x) - ((x2 - x1) * y) + (x2 * y1) - (y2 * x1)) / sqrt(pow(y2 - y1, 2.0) + pow(x2 - x1, 2.0));
}

static double powerLine(double x, double y, double x1, double y1, double x2, double y2){
    x += pow(x, 1.0 / sqrt(pow(x - x1, 2.0) + pow(y - y1, 2.0)));
    y += pow(y, 1.0 / sqrt(pow(x - x1, 2.0) + pow(y - y2, 2.0)));
    return fabs(((y2 - y1) * x) - ((x2 - x1) * y) + (x2 * y1) - (y2 * x1)) / sqrt(pow(y2 - y1, 2.0) + pow(x2 - x1, 2.0));
}

static double squigLine(double x, double y, double x1, double y1, double x2, double y2){
    double dist = sqrt(pow(x2 - x1, 2.0) + pow(y2 - y1, 2.0));
    x *= sqrt(pow(x - x1, 2.0) + pow(y - y1, 2.0)) / dist;
    y *= sqrt(pow(x - x2, 2.0) + pow(y - y2, 2.0)) / dist;
    return fabs(((y2 - y1) * x) - ((x2 - x1) * y) + (x2 * y1) - (y2 * x1)) / sqrt(pow(y2 - y1, 2.0) + pow(x2 - x1, 2.0));
}

static double (*lineEq)(double, double, double, double, double, double) = &basicLine;

unsigned waveDraw(double x, double y){ return (sin(x * 10.0) > tan(y * 10.0))? xColor : yColor; }

static unsigned dotDraw(unsigned color, double distance, PixelPoint pixPoint){
    ColorPoint lastPoint = colorPointTable.points[colorPointTable.pointCount - 1];

    // if(pixPoint.x == lastPoint.x && pixPoint.y == lastPoint.y)
    switch(mode){
        case '1': return (distance > dotSize * DOT_RADIUS)? NO_COLOR : color_invert(color);
        case '2': return (distance > dotSize * DOT_RADIUS || distance < dotSize * (DOT_RADIUS / 3.0))? NO_COLOR : color_invert(color);
        case '3': return (distance > dotSize * DOT_RADIUS || distance > (dotSize * DOT_RADIUS) + pixPoint.x)? NO_COLOR : color_invert(color);
        case '4': return (distance > dotSize * DOT_RADIUS || distance > (dotSize * DOT_RADIUS) - pixPoint.y)? NO_COLOR : color_invert(color);
        case '5': return (distance > dotSize * DOT_RADIUS || (fabs(pixPoint.x - sin(distance / 10)) * (dotSize / DOT_RADIUS) > dotSize * DOT_RADIUS))? NO_COLOR : color_invert(color);
        case '6': return (distance > dotSize * DOT_RADIUS || (fabs(pixPoint.y * cos(distance * 10)) * (dotSize / DOT_RADIUS) > dotSize * DOT_RADIUS))? NO_COLOR : color_invert(color);
        case '7': return (distance > dotSize * DOT_RADIUS || ((fabs(pixPoint.x) * DOT_RADIUS + fabs(pixPoint.y) * DOT_RADIUS) * (dotSize / DOT_RADIUS) > dotSize * DOT_RADIUS))? NO_COLOR : color_invert(color);
        case '8': return (distance > (dotSize * DOT_RADIUS) * sin(dotSize / pixPoint.x * DOT_RADIUS) * cos(pixPoint.y / distance))? NO_COLOR : color_invert(color);
        case '9': return (distance > (dotSize * DOT_RADIUS) * sin(dotSize / pixPoint.y * DOT_RADIUS) * cos(pixPoint.x / distance))? NO_COLOR : color_invert(color);
        default: return (distance > dotSize * DOT_RADIUS * pow((double)rand() / (double)RAND_MAX, 0.25))? NO_COLOR : color_invert(color);
    }
    // else return NO_COLOR;
}

unsigned strokeDraw(double x, double y){
    x *= 0.5 * _dimens[0];
    y *= 0.5 * _dimens[1];

    ColorPoint colorPoints[COLOR_POINTS]; // TODO: Add color points
    for(unsigned short c = 0; c < COLOR_POINTS; c++)
        colorPoints[c] = colorPointTable.points[colorPointTable.pointCount - 1 - c];

    double x1 = colorPoints[0].x - x; double y1 = colorPoints[0].y - y;
    double x2 = colorPoints[1].x - x; double y2 = colorPoints[1].y - y;

    double xDiff = (x - x2) / (x1 - x2);
    double yDiff = (y - y2) / (y1 - y2);
    double slope = (y1 - y2) / (x2 - x1);

    double dist = sqrt(pow(x2 - x1, 2.0) + pow(y2 - y1, 2.0));
    double dist1 = sqrt(pow(x - x1, 2.0) + pow(y - y1, 2.0));
    double dist2 = sqrt(pow(x - x2, 2.0) + pow(y - y2, 2.0));
    double cross = ((x - x1) * (x2 - x1)) - ((y - y1) * (y2 - y1));

    double lineDist = lineEq(x, y, x1, y1, x2, y2);

    if(lineDist < DOT_RADIUS && dist2 < dist && dist1 < dist)
        switch(mode){
            case 'w': return (lineDist * (dist / (dist1 * dist2)) < 0.5)? colorPoints[0].color : NO_COLOR; // SAVE THIS
            case 'e': return (lineDist * (dist / (dist1 * dist2)) < cos(lineDist * 5.0 * 10))? colorPoints[0].color : NO_COLOR; // SAVE THIS!
            case 'r': return (lineDist / (fabs(dist1 + dist2 + cross) / FIELD_PRODUCT) > 0.1)? colorPoints[0].color : NO_COLOR;
            case 't': return (pow(lineDist, (fabs(dist1 - dist2) * FIELD_PRODUCT)) < fabs(sin(lineDist * 10.0)))? colorPoints[0].color : NO_COLOR;
            case 'y': return (lineDist * (dist / (dist1 * dist2)) < pow(xDiff / yDiff, yDiff * xDiff))? colorPoints[0].color : NO_COLOR;
            case 'u': return (lineDist * (dist / (dist1 * dist2)) < (lineDist * fabs(x / y)) * 30.0)? colorPoints[0].color : NO_COLOR;
            case 'i': return (lineDist - sin((dist1 + dist2) * 5.0 * 10) < DOT_RADIUS / 5)? colorPoints[0].color : NO_COLOR;
            case 'o': return (lineDist * cos((dist1 + dist2) * 5.0 * 10) < DOT_RADIUS / 5)? colorPoints[0].color : NO_COLOR;
            case 'p': return (pow(lineDist, tan((dist1 / dist2) * 5.0 * 10)) < DOT_RADIUS)? colorPoints[0].color : NO_COLOR;
            // case 'i': return (lineDist < xMod(dist1) + yMod(dist2))? colorPoints[0].color : NO_COLOR; // TODO: Replace with draw algorithm
            // case 'o': return (lineDist * xMod(dist1) < lineDist * yMod(dist2))? colorPoints[0].color : NO_COLOR; // TODO: Replace with draw algorithm
            // case 'p': return (pow(lineDist, xMod(x - x1)) < pow(lineDist, yMod(y - y2)))? colorPoints[0].color : NO_COLOR; // TODO: Replace with draw algorithm
            default: return colorPoints[0].color;
        }
    else return NO_COLOR; // colors_blend(_swatch.colors[SWATCH_Light], _swatch.colors[SWATCH_Dark], dist1 + dist2);
}

static unsigned fieldDraw(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]){
    unsigned c1 = colors[0];
    unsigned c2 = colors[1];

    switch(mode){
        case 'a': return colors_blend(c1, c2, distances[0] * FIELD_PRODUCT);
        case 's': return (distances[1] - distances[0] > 0.01)? colors_blend(c1, c2, (distances[1] - distances[0]) * 10.0) : c2; // colors_blend(c1, c2, distances[1] * FIELD_PRODUCT);
        case 'd': return (distances[2] - distances[1] > 0.01)? colors_blend(c1, c2, (distances[2] - distances[1]) * 10.0) : c2; // colors_blend(c1, c2, distances[2] * FIELD_PRODUCT);
        case 'f': return colors_blend(c1, c2, sin(pow(fabs(pixPoints[0].x - pixPoints[1].y), distances[0]) * FIELD_PRODUCT * 5.0));
        case 'g': return colors_blend(c1, c2, cos(pow(fabs(pixPoints[0].y + pixPoints[1].x), distances[0]) * FIELD_PRODUCT * 5.0));
        case 'h': return colors_blend(c1, c2, tan(pow(distances[2], fabs(pixPoints[0].x * pixPoints[1].y)) * FIELD_PRODUCT * 100.0));
        case 'j': return (distances[2] * 0.5 > distances[0] + distances[1])? c1 : c2;
        case 'k': return (pixPoints[0].x / pixPoints[1].y > pow(pixPoints[1].x, fabs(pixPoints[0].y)))? c1 : c2;
        default: return (((distances[2] + distances[1] + distances[0]) / 3.0) > distances[1])? c1 : c2;
    }
}

void setup(char input){ // double (*xMod)(double), double (*yMod)(double)){
    if(!isdigit(input))
        switch(input){
            case 'q': case 'w': case 'e': case 'r': case 't': case 'y': case 'u': case 'i': case 'o': case 'p':
            case 'z': case 'x': case 'c': case 'v': case 'b': case 'n': case 'm':
                Rasteron_Image* tempImg = mapImgOp((ImageSize){1024, 1024}, strokeDraw);
                for(unsigned p = 0; p < tempImg->width * tempImg->height; p++)
                    if(*(tempImg->data + p) != NO_COLOR) *(_outputImg->data + p) = *(tempImg->data + p);
                RASTERON_DEALLOC(tempImg);
            break;
            case 'a': case 's': case 'd': case 'f': case 'g': case 'h': case 'j': case 'k': case 'l':
                RASTERON_DEALLOC(_outputImg);
                _outputImg = fieldExtImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, fieldDraw);
            break;
        }
    else {
        Rasteron_Image* tempImg = fieldImgOp((ImageSize){1024, 1024}, &colorPointTable, dotDraw);
        for(unsigned p = 0; p < tempImg->width * tempImg->height; p++)
            if(*(tempImg->data + p) != NO_COLOR) *(_outputImg->data + p) = *(tempImg->data + p);
        RASTERON_DEALLOC(tempImg);
    }
}

void _onKeyEvent(char key){
    if(colorPointTable.pointCount > COLOR_POINTS && key != 13) mode = key;

    /* switch(key){
        case 'z': xMod = &sinMod; yMod = &cosMod; break;
        case 'x': xMod = &sinModX; yMod = &cosModD; break;
        case 'c': xMod = &cosModD; yMod = &sinModD; break;
        case 'v': xMod = &cosModX; yMod = &sinModX; break;
        case 'b': xMod = &tanMod; yMod = &sinModX; break;
        case 'n': xMod = &tanMod; yMod = &cosModD; break;
        case 'm': xMod = &tanModX; yMod = &tanModD; break;
    } */
    switch(key){
        case 'z': lineEq = &basicLine; break;
        case 'x': lineEq = &wobbleLine; break;
        case 'c': lineEq = &powerLine; break;
        case 'v': lineEq = &squigLine; break;
    }
    setup(mode); // xMod, yMod);
}

void _onPressEvent(double x, double y){
    xColor = RAND_COLOR(); // | ((unsigned)(x * 256) * 0x10001);
    yColor = color_invert(xColor); // | ((unsigned)(y * 256) * 0x101);

    pixelPointToTable(&pixelPointTable, x, y);
    // colorPointToTable(&colorPointTable, (colorPointTable.pointCount % 2 == 0)? xColor : yColor, x, y);
    colorPointToTable(&colorPointTable, color_level((colorPointTable.pointCount % 2 == 0)? xColor : yColor, (x + y) / 2.0), x, y);

    setup(mode);
    pressCount++;
}

void _onTickEvent(unsigned secs){}

int main(int argc, char** argv){
    pixelPointTable.pointCount = 0;
    colorPointTable.pointCount = 0;

    if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    _outputImg = solidImgOp((ImageSize){1024, 1024}, 0xFF333333); // mapImgOp((ImageSize){1024, 1024}, waveDraw); // global canvas for drawing

    _run(argc, argv); // system specific initialization and continuous loop

    RASTERON_DEALLOC(_outputImg); // cleanup
    return 0;
}
