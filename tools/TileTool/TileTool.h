#include "Rasteron.h"

extern double xArg;
extern double yArg;
extern int mode;

// Tiling Functions

static unsigned eqTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    return blend_colors(colors[0], blend_colors(colors[1], colors[2], fabs(distances[0] - distances[2])), fabs(distances[0] - distances[1]));
}
static unsigned softTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    return blend_colors(colors[0], color_invert(colors[0]), fabs(pixPoint[0].x / pixPoint[0].y));
}
static unsigned hardTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    return (distances[0] / distances[1] > pixPoint[0].x / pixPoint[1].y)? colors[0] : colors[1];
}
static unsigned dotTiling1(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    return (distances[0] < (mode * 0.05))? colors[0] : colors[1];
}
static unsigned dotTiling2(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    return (distances[0] / distances[1] * distances[2] < (mode * 0.05))? colors[0] : colors[1];
}
static unsigned dotTiling3(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    return (distances[0] + distances[1] - distances[2] < (mode * 0.05))? colors[0] : colors[1];
}
static unsigned stripeTiling1(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    return ((distances[0] * 10) - floor(distances[0] * 10) < 0.5) ? colors[0] : colors[1];
    // return (abs(pixPoint[0].x - pixPoint[0].y) > CROSSTILE_THRESH) ? colors[0] : colors[1];
    // return blend_colors(colors[0] + colors[1], colors[2] - colors[1], atan(pixPoint[0].x / pixPoint[0].y));
}
static unsigned stripeTiling2(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    return (ceil(distances[1] * 10) - (distances[1] * 10) < 0.5) ? colors[1] : colors[0];
    // return (abs(pixPoint[1].x - pixPoint[0].y) > CROSSTILE_THRESH) ? colors[1] : colors[2];
    // return blend_colors(colors[0], blend_colors(colors[1], colors[2], sin(distances[0] / distances[1])), cos(distances[2] / distances[1]));
}
static unsigned stripeTiling3(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]) {
    return (ceil(distances[2] * 10) - (distances[2] * 10) > ((distances[0] / distances[1]) * 10) - floor((distances[0] / distances[1]) * 10)) ? colors[2] : colors[1];
    // return (pow(distances[0], pixPoint[2].x + pixPoint[2].y) > CROSSTILE_THRESH) ? colors[0] : colors[2];
}
static unsigned breakTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]) {
    double xDiff = (pixPoint[0].x * 10) - floor(pixPoint[0].x * 10);
    double yDiff = (pixPoint[0].y * 10) - floor(pixPoint[0].y * 10);
    return (xDiff * yDiff * 10 - floor(xDiff * yDiff * 10) < 0.5) ? colors[0] : colors[1];
    // return (pixPoint[0].x / pixPoint[1].y > distances[0] / atan(((pixPoint[0].y + 1.0) * 0.5) / ((pixPoint[0].x + 1.0) * 0.5)))? root_colors(colors[2], colors[1]) : root_colors(colors[1], colors[2]);
} 
static unsigned shineTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    return fuse_colors(colors[0], colors[1], sin(pixPoint[0].x * (1.0 / distances[0]) + pixPoint[0].y * (1.0 / distances[0])));
}
static unsigned lumenTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    if(distances[0] > xArg){
        if(pixPoint[0].x / pixPoint[1].y > yArg) return blend_colors(colors[1], colors[2], fabs(tan(pow(distances[0], distances[1] + distances[2]))));
        else return blend_colors(colors[0], colors[1], fabs(tan(distances[1] - distances[2]) * 10.0));
    }
    else return blend_colors(colors[0], colors[1], fabs(tan((distances[0] - distances[1]) * 10.0)));
}
static unsigned flashTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    double angle = atan(pixPoint[0].y / pixPoint[1].x);
    if(angle > (mode * 0.5)) return blend_colors(colors[0], colors[1], angle - (distances[0] / xArg));
    else fuse_colors(colors[0], colors[1], angle + (distances[0] / fabs(yArg)));
}
static unsigned amorphTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    unsigned s = (pixPoint[0].x / pixPoint[0].y) / (distances[0] / distances[1] / distances[2]);
    if(s < 0.0) return colors[0];
    else if(s > 1.0) return colors[1];
    else return colors[2];
}
static unsigned focalTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    double xRel = (distances[0] / pixPoint[0].x) * xArg + (mode * 0.5);
    double yRel = (distances[0] / pixPoint[0].y) * yArg + (mode * 0.5);
    return fuse_colors(colors[0], colors[1], fabs(xRel - yRel));
}
static unsigned linedTiling1(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    unsigned color = colors[0];
    double dist = distances[0] + distances[1] + distances[2];
    if((dist * 5.0) - floor(dist * 5.0) > pow(xArg, 1.0 - yArg) + sin((mode * 0.5) * dist)) color = colors[1];
    else if(ceil(dist * 5.0) - (dist * 5.0) > pow(yArg, 1.0 - xArg) + sin((mode * 0.5) * dist)) color = colors[2];
    return color;
}
static unsigned linedTiling2(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    double stepDistance = distances[0] * 10.0;
    double subtDistance = stepDistance - floor(stepDistance);
    if(subtDistance < xArg) return colors[0];
    else if(subtDistance > 1.0 - fabs(yArg)) return colors[1];
    else return colors[2];
}
static unsigned complexTiling1(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    if(pow(distances[0], pixPoint[0].x + pixPoint[0].y) > xArg)
        return color_level(colors[0], pow((distances[0] * xArg) / (distances[1] * fabs(yArg)), 1.0 + (mode * 0.5)));
    else if(pixPoint[1].x - pixPoint[1].y < distances[1] + (1.0 / yArg))
        return fuse_colors(colors[2], colors[1], atan((pixPoint[0].y * xArg) / (pixPoint[1].x * yArg)));
    else return blend_colors(colors[0], colors[2], tan(distances[2] * 10.0)); // atan((pixPoint[0].y * xArg) / (pixPoint[1].x * yArg)));
}
static unsigned complexTiling2(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]) {
    if (pow(distances[1], pixPoint[1].x + pixPoint[1].y) > xArg)
        return color_level(colors[0], pow((distances[0] * xArg) / (distances[1] * fabs(yArg)), 1.0 + (mode * 0.5)));
    else if (pixPoint[2].x - pixPoint[2].y < distances[0] + (1.0 / yArg))
        return fuse_colors(colors[2], colors[1], atan((pixPoint[0].y * xArg) / (pixPoint[1].x * yArg)));
    else return blend_colors(colors[0], colors[2], tan(distances[1] * 10.0)); // atan((pixPoint[0].y * xArg) / (pixPoint[1].x * yArg)));
}

// Truschet Functionsw

static unsigned sharpTruschetTile(double x, double y){
    return blend_colors(0xFFFF00FF, 0xFF00FFFF, asin(pow(x, y)) + acos(pow(y, x)));
}

// Image Functions

Rasteron_Image* mosaicImgOp(ImageSize size, const ColorPointTable* colorPointTable, fieldCallback3 callback);
Rasteron_Image* truschetImgOp(ref_image_t truschetImg, unsigned short wDiv, unsigned short hDiv);