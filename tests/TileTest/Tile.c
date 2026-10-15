#include "TileTest.h"

extern double xArg;
extern double yArg;

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

// Image Operations

Rasteron_Image* mosaicImgOp(ImageSize size, const ColorPointTable* colorPointTable, fieldCallback3 callback) {
    Rasteron_Image* mosaicImage = RASTERON_ALLOC("field", size.height, size.width);

    unsigned* colorPoints = malloc(colorPointTable->pointCount * sizeof(unsigned));
    for (unsigned t = 0; t < colorPointTable->pointCount; t++)
        *(colorPoints + t) = pixPoint_offset((PixelPoint){ colorPointTable->points[t].x + sin(xArg * t), colorPointTable->points[t].y - sin(yArg * t) }, mosaicImage);
        // *(colorPoints + t) = pixPoint_offset((PixelPoint){ pow(colorPointTable->points[t].x + sin(yArg * t), (double)mode), pow(colorPointTable->points[t].y - sin(yArg * t), (double)mode) }, mosaicImage);

    unsigned pixColors[3] = { NO_COLOR, NO_COLOR, NO_COLOR };
    double pixDistances[3] = { 1.0, 1.0, 1.0 };
    PixelPoint pixPoints[3] = {{ 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 }};

    for (unsigned p = 0; p < mosaicImage->width * mosaicImage->height; p++) {
        double x = (1.0 / (double)size.width) * (p % size.width) - xArg;
        double y = (1.0 / (double)size.height) * (p / size.width) - yArg;

        pixDistances[0] = 1.0; pixDistances[1] = 1.0; pixDistances[2] = 1.0; // reset
        for (unsigned t = 0; t < colorPointTable->pointCount; t++) {
            double dist = pix_dist(p, *(colorPoints + t), mosaicImage->width) * (1.0 / (double)(mosaicImage->width)); // distance multiplied by pixel size
            dist += sin(t / colorPointTable->pointCount);
            if (dist < pixDistances[0]) {
                for(unsigned d = 0; d < 3; d++){
                    pixDistances[d] = dist;
                    pixPoints[d] = (PixelPoint){
                        x - colorPointTable->points[t].x,
                        y - colorPointTable->points[t].y
                    };
                }
                pixColors[0] = colorPointTable->points[t].color;
                pixColors[1] = blend_colors(colorPointTable->points[t].color, 0xFF000000, 0.5); // darken color
                pixColors[2] = blend_colors(colorPointTable->points[t].color, 0xFFFFFFFF, 0.5); // lighten color
            }
        }

        *(mosaicImage->data + p) = callback(pixColors, pixDistances, pixPoints);
    }

    free(colorPoints);
    return mosaicImage;
}

Rasteron_Image* truschetImgOp(ref_image_t truschetImg, unsigned short wDiv, unsigned short hDiv){
    // Rasteron_Image* truchetTile = RASTERON_ALLOC("truschet_tile", 1300 / hDiv, 1300 / wDiv);
    Rasteron_Image* truschetTile = (truschetImg == NULL)? mapImgOp((ImageSize){ 1300 / hDiv, 1300 / wDiv }, sharpTruschetTile) : copyImgOp(truschetImg);
    Rasteron_Image* truschetTile2 = flipImgOp(truschetTile, FLIP_Upside);
    Rasteron_Image* truschetTile3 = flipImgOp(truschetTile, FLIP_Clock);
    Rasteron_Image* truschetTile4 = flipImgOp(truschetTile, FLIP_Counter);

    Rasteron_Image* finalImg = RASTERON_ALLOC("truschet", 1300, 1300);
    for(unsigned p = 0; p < 1300 * 1300; p++){
        double x = (1.0 / (double)1300) * (p % 1300);
		double y = (1.0 / (double)1300) * (p / 1300);

        unsigned c = x * wDiv;
        unsigned r = y * hDiv;

        Rasteron_Image** targetTileImg;
        if(c % 2 == 0) targetTileImg = (r % 2 == 0)? &truschetTile : &truschetTile2;
        else targetTileImg = (r % 2 == 0)? &truschetTile3 : &truschetTile4;

        *(finalImg->data + p) = pixPoint_color((PixelPoint){ (x * wDiv) - floor(x * wDiv), (y * hDiv) - floor(y * hDiv) }, *targetTileImg);
    }

    RASTERON_DEALLOC(truschetTile);
    RASTERON_DEALLOC(truschetTile2);
    RASTERON_DEALLOC(truschetTile3);
    RASTERON_DEALLOC(truschetTile4);

    return finalImg; 
}
