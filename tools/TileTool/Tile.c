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
    unsigned targetColor = blend_colors(colors[0], colors[2], tan(distances[2] * 10.0)); // atan((pixPoint[0].y * xArg) / (pixPoint[1].x * yArg)));

    if(pow(distances[0], pixPoint[0].x + pixPoint[0].y) > xArg)
        targetColor = color_level(targetColor, pow((distances[0] * xArg) / (distances[1] * fabs(yArg)), 1.0 + (mode * 0.5)));
    else if(pixPoint[1].x - pixPoint[1].y < distances[1] + (1.0 / yArg))
        targetColor = fuse_colors(targetColor, colors[1], atan((pixPoint[0].y * xArg) / (pixPoint[1].x * yArg)));

    return targetColor;
}
static unsigned complexTiling2(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]) {
    unsigned targetColor = blend_colors(colors[0], colors[2], tan(distances[1] * 10.0)); // atan((pixPoint[0].y * xArg) / (pixPoint[1].x * yArg)));

    if (pow(distances[1], pixPoint[1].x + pixPoint[1].y) > xArg)
        targetColor = color_level(targetColor, pow((distances[0] * xArg) / (distances[1] * fabs(yArg)), 1.0 + (mode * 0.5)));
    else if (pixPoint[2].x - pixPoint[2].y < distances[0] + (1.0 / yArg))
        targetColor = fuse_colors(targetColor, colors[1], atan((pixPoint[0].y * xArg) / (pixPoint[1].x * yArg)));

    return targetColor;
}


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

static unsigned sharpTruschetTile(double x, double y){
    return blend_colors(0xFFFF00FF, 0xFF00FFFF, asin(pow(x, y)) + acos(pow(y, x)));
}

Rasteron_Image* truschetImgOp(ref_image_t truschetImg, unsigned short wDiv, unsigned short hDiv){
    // Rasteron_Image* truchetTile = RASTERON_ALLOC("truschet_tile", 1024 / hDiv, 1024 / wDiv);
    Rasteron_Image* truschetTile = (truschetImg == NULL)? mapImgOp((ImageSize){ 1024 / hDiv, 1024 / wDiv }, sharpTruschetTile) : copyImgOp(truschetImg);
    Rasteron_Image* truschetTile2 = flipImgOp(truschetTile, FLIP_Upside);
    Rasteron_Image* truschetTile3 = flipImgOp(truschetTile, FLIP_Clock);
    Rasteron_Image* truschetTile4 = flipImgOp(truschetTile, FLIP_Counter);

    Rasteron_Image* finalImg = RASTERON_ALLOC("truschet", 1024, 1024);
    for(unsigned p = 0; p < 1024 * 1024; p++){
        double x = (1.0 / (double)1024) * (p % 1024);
		double y = (1.0 / (double)1024) * (p / 1024);

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
