#include "Rasteron.h"

// Globals

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
static unsigned short pressCount = 0;

typedef unsigned (*brushCallback)(unsigned, double[3], double, double, double);

// Line Draw Callback Functions

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
static double bulge(double x, double y, double x1, double y1, double x2, double y2){
    return fabs(((y2 - y1) * x) - ((x2 - x1) * y) + (x2 * y1) - (y2 * x1)) / sqrt(pow(y2 - y1, 2.0) - sin(y * 10) + pow(x2 - x1, 2.0) + sin(x * 10));
}
static double cutLine(double x, double y, double x1, double y1, double x2, double y2){
    x *= (1.0 - (y * 2)) * (1.0 + (x * 2));
    y *= (1.0 + (x * 2)) * (1.0 - (y * 2));
    return fabs(((y2 - y1) * x) - ((x2 - x1) * y) + (x2 * y1) - (y2 * x1)) / sqrt(pow(y2 - y1, 2.0) + pow(x2 - x1, 2.0));
}
static double waves(double x, double y, double x1, double y1, double x2, double y2){
    x = y * tan(x1 * x2 * 100);
    y = x / tan(y1 * y2 * 100);
    return fabs(((y2 - y1) * x) - ((x2 - x1) * y) + (x2 * y1) - (y2 * x1)) / sqrt(pow(y2 - y1, 2.0) + pow(x2 - x1, 2.0));
}

static double (*lineEq)(double, double, double, double, double, double) = &basicLine; // default line equation

// Wave, Dot, and Field Draw Callback Functions

static unsigned waveDraw(double x, double y){ return (sin(x * 10.0) > tan(y * 10.0))? xColor : yColor; }

static unsigned dotDraw1(unsigned color, double distance, PixelPoint pixPoint){ return (distance > dotSize * DOT_RADIUS)? NO_COLOR : color_invert(color); }
static unsigned dotDraw2(unsigned color, double distance, PixelPoint pixPoint){ return (distance > dotSize * DOT_RADIUS || distance < dotSize * (DOT_RADIUS / 3.0))? NO_COLOR : color_invert(color); }
static unsigned dotDraw3(unsigned color, double distance, PixelPoint pixPoint){ return (distance > dotSize * DOT_RADIUS || distance > (dotSize * DOT_RADIUS) + pixPoint.x)? NO_COLOR : color_invert(color); }
static unsigned dotDraw4(unsigned color, double distance, PixelPoint pixPoint){ return (distance > dotSize * DOT_RADIUS || distance > (dotSize * DOT_RADIUS) - pixPoint.y)? NO_COLOR : color_invert(color); }
static unsigned dotDraw5(unsigned color, double distance, PixelPoint pixPoint){ return (distance > dotSize * DOT_RADIUS || (fabs(pixPoint.x - sin(distance / 10)) * (dotSize / DOT_RADIUS) > dotSize * DOT_RADIUS))? NO_COLOR : color_invert(color); }
static unsigned dotDraw6(unsigned color, double distance, PixelPoint pixPoint){ return (distance > dotSize * DOT_RADIUS || (fabs(pixPoint.y * cos(distance * 10)) * (dotSize / DOT_RADIUS) > dotSize * DOT_RADIUS))? NO_COLOR : color_invert(color); }
static unsigned dotDraw7(unsigned color, double distance, PixelPoint pixPoint){ return (distance > dotSize * DOT_RADIUS || ((fabs(pixPoint.x) * DOT_RADIUS + fabs(pixPoint.y) * DOT_RADIUS) * (dotSize / DOT_RADIUS) > dotSize * DOT_RADIUS))? NO_COLOR : color_invert(color); }
static unsigned dotDraw8(unsigned color, double distance, PixelPoint pixPoint){ return (distance > (dotSize * DOT_RADIUS) * sin(dotSize / pixPoint.x * DOT_RADIUS) * cos(pixPoint.y / distance))? NO_COLOR : color_invert(color); }
static unsigned dotDraw9(unsigned color, double distance, PixelPoint pixPoint){ return (distance > (dotSize * DOT_RADIUS) * sin(dotSize / pixPoint.y * DOT_RADIUS) * cos(pixPoint.x / distance))? NO_COLOR : color_invert(color); }
static unsigned dotDraw0(unsigned color, double distance, PixelPoint pixPoint){ return (distance > dotSize * DOT_RADIUS * pow((double)rand() / (double)RAND_MAX, 0.25))? NO_COLOR : color_invert(color); }

static unsigned fieldDraw1(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]){ 
    return blend_colors(colors[0], colors[1], distances[0] * FIELD_PRODUCT);
}
static unsigned fieldDraw2(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]){ 
    return (distances[1] - distances[0] > 0.01)? blend_colors(colors[0], colors[1], (distances[1] - distances[0]) * 10.0) : colors[1];
}
static unsigned fieldDraw3(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]){ 
    return (distances[2] - distances[1] > 0.01)? blend_colors(colors[0], colors[1], (distances[2] - distances[1]) * 10.0) : colors[1]; // blend_colors(colors[0], colors[1], distances[2] * FIELD_PRODUCT);
}
static unsigned fieldDraw4(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]){ 
    return blend_colors(colors[0], colors[1], sin(pow(fabs(pixPoints[0].x - pixPoints[1].y), distances[0]) * FIELD_PRODUCT * 5.0));
}
static unsigned fieldDraw5(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]){ 
    return blend_colors(colors[0], colors[1], cos(pow(fabs(pixPoints[0].y + pixPoints[1].x), distances[0]) * FIELD_PRODUCT * 5.0));
}
static unsigned fieldDraw6(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]){ 
    return blend_colors(colors[0], colors[1], tan(pow(distances[2], fabs(pixPoints[0].x * pixPoints[1].y)) * FIELD_PRODUCT * 10.0));
}
static unsigned fieldDraw7(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]){ 
    return (distances[2] * 0.5 > distances[0] + distances[1])? colors[0] : colors[1];
}
static unsigned fieldDraw8(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]){ 
    return (pixPoints[0].x / pixPoints[1].y > pow(pixPoints[1].x, fabs(pixPoints[0].y)))? colors[0] : colors[1];
}
static unsigned fieldDraw0(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]){ 
    return (((distances[2] + distances[1] + distances[0]) / 3.0) > distances[1])? colors[0] : colors[1];
}

// Brush Draw Callback Functions

static unsigned brushDraw1(unsigned color, double distances[3], double xDiff, double yDiff, double lineDist){ 
    return (lineDist * (distances[0] / (distances[1] * distances[2])) < 0.5)? color : NO_COLOR;
}
static unsigned brushDraw2(unsigned color, double distances[3], double xDiff, double yDiff, double lineDist){ 
    return (lineDist * (distances[0] / (distances[0] * distances[1])) < cos(lineDist * 5.0 * 10))? color : NO_COLOR;
}
static unsigned brushDraw3(unsigned color, double distances[3], double xDiff, double yDiff, double lineDist){ 
    return (lineDist / (fabs(distances[0] + distances[1] + (xDiff / yDiff)) / FIELD_PRODUCT) > 0.1)? color : NO_COLOR;
}
static unsigned brushDraw4(unsigned color, double distances[3], double xDiff, double yDiff, double lineDist){ 
    return (pow(lineDist, (fabs(distances[0] - distances[1]) * FIELD_PRODUCT)) < fabs(sin(lineDist * 10.0)))? color : NO_COLOR;
}
static unsigned brushDraw5(unsigned color, double distances[3], double xDiff, double yDiff, double lineDist){ 
    return (lineDist * (distances[0] / (distances[0] * distances[1])) < pow(xDiff / yDiff, yDiff * xDiff))? color : NO_COLOR;
}
static unsigned brushDraw6(unsigned color, double distances[3], double xDiff, double yDiff, double lineDist){ 
    return (lineDist * (distances[0] / (distances[0] * distances[1])) < (lineDist * fabs(xDiff / yDiff)) * 30.0)? color : NO_COLOR;
}
static unsigned brushDraw7(unsigned color, double distances[3], double xDiff, double yDiff, double lineDist){ 
    return (lineDist - sin((distances[0] + distances[1]) * 5.0 * 10) < DOT_RADIUS / 5)? color : NO_COLOR;
}
static unsigned brushDraw8(unsigned color, double distances[3], double xDiff, double yDiff, double lineDist){ 
    return (lineDist * cos((distances[0] + distances[1]) * 5.0 * 10) < DOT_RADIUS / 5)? color : NO_COLOR;
}
static unsigned brushDraw9(unsigned color, double distances[3], double xDiff, double yDiff, double lineDist){ 
    return (pow(lineDist, tan((distances[0] / distances[1]) * 5.0 * 10)) < DOT_RADIUS)? color : NO_COLOR;
}
static unsigned brushDraw0(unsigned color, double distances[3], double xDiff, double yDiff, double lineDist){ return color; }

// Image Functions

Rasteron_Image* brushDrawImgOp(ImageSize size, brushCallback callback);
