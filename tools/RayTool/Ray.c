#include "RayTool.h"

Rasteron_Image* raycastImgOp(ImageSize size, float* points, unsigned pointCount, double dist) {
    Rasteron_Image* raycastImg = RASTERON_ALLOC("raycast", size.height, size.width);

    for (unsigned p = 0; p < raycastImg->width * raycastImg->height; p++) {
        double x = (1.0 / (double)1300) * (p % 1300) - 0.5;
        double y = (1.0 / (double)1300) * (p / 1300) - 0.5;

        double length = sqrt(pow(x, 2.0) + pow(y, 2.0) + pow(dist, 2.0));

        if(pointCount > 0 && points != NULL)
            for (unsigned c = 0; c < pointCount; c++) {
                unsigned color = rayFunc(
                    fabs(x / length) + *(points + (c * 3) + 0), 
                    fabs(y / length) + *(points + (c * 3) + 1),
                    fabs(dist / length) + *(points + (c * 3) + 2)
                );
                *(raycastImg->data + p) = (c == 0)? color : rayMixFunc(color, *(raycastImg->data + p));
            }
        else *(raycastImg->data + p) = rayFunc(fabs(x / length), fabs(y / length), fabs(dist / length));
    }

    return raycastImg;
}

Rasteron_Image* rayblendImgOp(ImageSize size, float* points, unsigned pointCount, double dist){ 
    Rasteron_Image* raycastImg = RASTERON_ALLOC("raycast", size.height, size.width);

    for (unsigned p = 0; p < raycastImg->width * raycastImg->height; p++) {
        double x = (1.0 / (double)1300) * (p % 1300) - 0.5;
        double y = (1.0 / (double)1300) * (p / 1300) - 0.5;

        double length = sqrt(pow(x, 2.0) + pow(y, 2.0) + pow(dist, 2.0));
        
        if(pointCount > 0 && points != NULL)
            for (unsigned c = 0; c < pointCount; c++) {
                unsigned color = rayFunc(
                  fabs(x / length) + *(points + (c * 3) + 0), 
                  fabs(y / length) + *(points + (c * 3) + 1),
                  fabs(dist / length) + *(points + (c * 3) + 2)
                );
                color = mixFunc(color, color);
                *(raycastImg->data + p) = (c == 0)? color : rayMixFunc(color, *(raycastImg->data + p));
            }
        else *(raycastImg->data + p) = rayFunc(fabs(x / length), fabs(y / length), fabs(dist / length));
    }

    return raycastImg;
}