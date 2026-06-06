#include "DrawTool.h"

extern unsigned _dimens[2];

extern PixelPointTable pixelPointTable;
extern ColorPointTable colorPointTable;

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