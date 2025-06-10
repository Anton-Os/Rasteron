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

static double (*lineEq)(double, double, double, double, double, double) = &basicLine;

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
    return blend_colors(colors[0], colors[1], distances[0] * FIELD_PRODUCT);
}
static unsigned fieldDraw3(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]){ 
    return blend_colors(colors[0], colors[1], distances[0] * FIELD_PRODUCT);
}
static unsigned fieldDraw4(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]){ 
    return blend_colors(colors[0], colors[1], distances[0] * FIELD_PRODUCT);
}
static unsigned fieldDraw5(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]){ 
    return blend_colors(colors[0], colors[1], distances[0] * FIELD_PRODUCT);
}
static unsigned fieldDraw6(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]){ 
    return blend_colors(colors[0], colors[1], distances[0] * FIELD_PRODUCT);
}
static unsigned fieldDraw7(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]){ 
    return blend_colors(colors[0], colors[1], distances[0] * FIELD_PRODUCT);
}
static unsigned fieldDraw8(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]){ 
    return blend_colors(colors[0], colors[1], distances[0] * FIELD_PRODUCT);
}
static unsigned fieldDraw0(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]){ 
    return blend_colors(colors[0], colors[1], distances[0] * FIELD_PRODUCT);
}

typedef unsigned (*brushCallback)(unsigned[3], double[3], double, double, double);

Rasteron_Image* brushDrawImgOp(ImageSize size, brushCallback callback){
    Rasteron_Image* brushDrawImg = RASTERON_ALLOC("brush", size.height, size.width);

    for(unsigned p = 0; p < brushDrawImg->width * brushDrawImg->height; p++){
		double x = (1.0 / (double)size.width) * (p % size.width) * (0.5 * _dimens[0]);
		double y = (1.0 / (double)size.height) * (p / size.width) * (0.5 * _dimens[1]);

        ColorPoint colorPoints[COLOR_POINTS]; // TODO: Add color points
        for(unsigned short c = 0; c < COLOR_POINTS; c++)
            colorPoints[c] = colorPointTable.points[colorPointTable.pointCount - 1 - c];

        double x1 = colorPoints[0].x - x; double y1 = colorPoints[0].y - y;
        double x2 = colorPoints[1].x - x; double y2 = colorPoints[1].y - y;

        double xDiff = (x - x2) / (x1 - x2);
        double yDiff = (y - y2) / (y1 - y2);
        // double slope = (y1 - y2) / (x2 - x1);
        // double cross = ((x - x1) * (x2 - x1)) - ((y - y1) * (y2 - y1));

        double distances[3] = {
            sqrt(pow(x2 - x1, 2.0) + pow(y2 - y1, 2.0)),
            sqrt(pow(x - x1, 2.0) + pow(y - y1, 2.0)),
            sqrt(pow(x - x2, 2.0) + pow(y - y2, 2.0))
        };

        double lineDist = lineEq(x, y, x1, y1, x2, y2);

        if(lineDist < DOT_RADIUS && distances[1] < distances[0] && distances[1] < distances[0])
		    *(brushDrawImg->data + p) = callback(colorPoints[0].color, distances, xDiff, yDiff, lineDist);
        else *(brushDrawImg->data + p) = colorPoints[0].color;
	}

    return brushDrawImg;
}

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