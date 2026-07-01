#include "TileTool.h"

// Tiling Functions

unsigned eqTiling(unsigned colors[3], double dists[3], PixelPoint pxPts[3]){
    return blend_colors(colors[0], blend_colors(colors[1], colors[2], fabs(dists[0] - dists[2])), fabs(dists[0] - dists[1]));
}
unsigned softTiling(unsigned colors[3], double dists[3], PixelPoint pxPts[3]){
    return blend_colors(colors[0], color_invert(colors[0]), fabs(pxPts[0].x / pxPts[0].y));
}
unsigned hardTiling(unsigned colors[3], double dists[3], PixelPoint pxPts[3]){
    return (dists[0] / dists[1] > pxPts[0].x / pxPts[1].y)? colors[0] : colors[1];
}
unsigned dotTiling1(unsigned colors[3], double dists[3], PixelPoint pxPts[3]){
    return (dists[0] < (mode * 0.05))? colors[0] : colors[1];
}
unsigned dotTiling2(unsigned colors[3], double dists[3], PixelPoint pxPts[3]){
    return (dists[0] / dists[1] * dists[2] < (mode * 0.05))? colors[0] : colors[1];
}
unsigned dotTiling3(unsigned colors[3], double dists[3], PixelPoint pxPts[3]){
    return (dists[0] + dists[1] - dists[2] < (mode * 0.05))? colors[0] : colors[1];
}
unsigned stripeTiling1(unsigned colors[3], double dists[3], PixelPoint pxPts[3]){
    return ((dists[0] * 10) - floor(dists[0] * 10) < 0.5) ? colors[0] : colors[1];
    // return (abs(pxPts[0].x - pxPts[0].y) > CROSSTILE_THRESH) ? colors[0] : colors[1];
    // return blend_colors(colors[0] + colors[1], colors[2] - colors[1], atan(pxPts[0].x / pxPts[0].y));
}
unsigned stripeTiling2(unsigned colors[3], double dists[3], PixelPoint pxPts[3]){
    return (ceil(dists[1] * 10) - (dists[1] * 10) < 0.5) ? colors[1] : colors[0];
    // return (abs(pxPts[1].x - pxPts[0].y) > CROSSTILE_THRESH) ? colors[1] : colors[2];
    // return blend_colors(colors[0], blend_colors(colors[1], colors[2], sin(dists[0] / dists[1])), cos(dists[2] / dists[1]));
}
unsigned stripeTiling3(unsigned colors[3], double dists[3], PixelPoint pxPts[3]) {
    return (ceil(dists[2] * 10) - (dists[2] * 10) > ((dists[0] / dists[1]) * 10) - floor((dists[0] / dists[1]) * 10)) ? colors[2] : colors[1];
    // return (pow(dists[0], pxPts[2].x + pxPts[2].y) > CROSSTILE_THRESH) ? colors[0] : colors[2];
}
unsigned breakTiling(unsigned colors[3], double dists[3], PixelPoint pxPts[3]) {
    double xDiff = (pxPts[0].x * 10) - floor(pxPts[0].x * 10);
    double yDiff = (pxPts[0].y * 10) - floor(pxPts[0].y * 10);
    return (xDiff * yDiff * 10 - floor(xDiff * yDiff * 10) < 0.5) ? colors[0] : colors[1];
    // return (pxPts[0].x / pxPts[1].y > dists[0] / atan(((pxPts[0].y + 1.0) * 0.5) / ((pxPts[0].x + 1.0) * 0.5)))? root_colors(colors[2], colors[1]) : root_colors(colors[1], colors[2]);
} 
unsigned shineTiling(unsigned colors[3], double dists[3], PixelPoint pxPts[3]){
    return fuse_colors(colors[0], colors[1], sin(pxPts[0].x * (1.0 / dists[0]) + pxPts[0].y * (1.0 / dists[0])));
}
unsigned lumenTiling(unsigned colors[3], double dists[3], PixelPoint pxPts[3]){
    if(dists[0] > xArg){
        if(pxPts[0].x / pxPts[1].y > yArg) return blend_colors(colors[1], colors[2], fabs(tan(pow(dists[0], dists[1] + dists[2]))));
        else return blend_colors(colors[0], colors[1], fabs(tan(dists[1] - dists[2]) * 10.0));
    }
    else return blend_colors(colors[0], colors[1], fabs(tan((dists[0] - dists[1]) * 10.0)));
}
unsigned flashTiling(unsigned colors[3], double dists[3], PixelPoint pxPts[3]){
    double angle = atan(pxPts[0].y / pxPts[1].x);
    if(angle > (mode * 0.5)) return blend_colors(colors[0], colors[1], angle - (dists[0] / xArg));
    else fuse_colors(colors[0], colors[1], angle + (dists[0] / fabs(yArg)));
}
unsigned amorphTiling(unsigned colors[3], double dists[3], PixelPoint pxPts[3]){
    unsigned s = (pxPts[0].x / pxPts[0].y) / (dists[0] / dists[1] / dists[2]);
    if(s < 0.0) return colors[0];
    else if(s > 1.0) return colors[1];
    else return colors[2];
}
unsigned focalTiling(unsigned colors[3], double dists[3], PixelPoint pxPts[3]){
    double xRel = (dists[0] / pxPts[0].x) * xArg + (mode * 0.5);
    double yRel = (dists[0] / pxPts[0].y) * yArg + (mode * 0.5);
    return fuse_colors(colors[0], colors[1], fabs(xRel - yRel));
}
unsigned linedTiling1(unsigned colors[3], double dists[3], PixelPoint pxPts[3]){
    unsigned color = colors[0];
    double dist = dists[0] + dists[1] + dists[2];
    if((dist * 5.0) - floor(dist * 5.0) > pow(xArg, 1.0 - yArg) + sin((mode * 0.5) * dist)) color = colors[1];
    else if(ceil(dist * 5.0) - (dist * 5.0) > pow(yArg, 1.0 - xArg) + sin((mode * 0.5) * dist)) color = colors[2];
    return color;
}
unsigned linedTiling2(unsigned colors[3], double dists[3], PixelPoint pxPts[3]){
    double stepDistance = dists[0] * 10.0;
    double subtDistance = stepDistance - floor(stepDistance);
    if(subtDistance < xArg) return colors[0];
    else if(subtDistance > 1.0 - fabs(yArg)) return colors[1];
    else return colors[2];
}
unsigned complexTiling1(unsigned colors[3], double dists[3], PixelPoint pxPts[3]){
    if(pow(dists[0], pxPts[0].x + pxPts[0].y) > xArg)
        return color_level(colors[0], pow((dists[0] * xArg) / (dists[1] * fabs(yArg)), 1.0 + (mode * 0.5)));
    else if(pxPts[1].x - pxPts[1].y < dists[1] + (1.0 / yArg))
        return fuse_colors(colors[2], colors[1], atan((pxPts[0].y * xArg) / (pxPts[1].x * yArg)));
    else return blend_colors(colors[0], colors[2], tan(dists[2] * 10.0)); // atan((pxPts[0].y * xArg) / (pxPts[1].x * yArg)));
}
unsigned complexTiling2(unsigned colors[3], double dists[3], PixelPoint pxPts[3]) {
    if (pow(dists[1], pxPts[1].x + pxPts[1].y) > xArg)
        return color_level(colors[0], pow((dists[0] * xArg) / (dists[1] * fabs(yArg)), 1.0 + (mode * 0.5)));
    else if (pxPts[2].x - pxPts[2].y < dists[0] + (1.0 / yArg))
        return fuse_colors(colors[2], colors[1], atan((pxPts[0].y * xArg) / (pxPts[1].x * yArg)));
    else return blend_colors(colors[0], colors[2], tan(dists[1] * 10.0)); // atan((pxPts[0].y * xArg) / (pxPts[1].x * yArg)));
}

// Truschet Functions

unsigned sharpTruschetTile(double x, double y){
    return blend_colors(0xFFFF00FF, 0xFF00FFFF, asin(pow(x, y)) + acos(pow(y, x)));
}