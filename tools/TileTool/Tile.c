#include "TileTool.h"

extern double xArg;
extern double yArg;

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
