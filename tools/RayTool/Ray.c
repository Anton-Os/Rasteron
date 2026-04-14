static unsigned vectorFunc1(double x, double y, double z) {
    unsigned color = blend_colors(rayColor1, rayColor2, pow(x + y + z, x * y * z) * 0.5);
    return vecMixFunc(color, color);
}

static unsigned vectorFunc2(double x, double y, double z) {
    unsigned color = blend_colors(rayColor1, rayColor2, pow(sin(x + y + z), cos(x * y * z)) * 0.5);
    return vecMixFunc(color, color);
}

static unsigned vectorFunc3(double x, double y, double z) {
    unsigned color = blend_colors(rayColor1, rayColor2, pow(x / y / z, x / y / z) * 0.5);
    return vecMixFunc(color, color);
}

Rasteron_Image* raycastImgOp(ImageSize size, float* points, unsigned pointCount, double dist){ 
    Rasteron_Image* raycastImg = RASTERON_ALLOC("raycast", size.height, size.width);

    for (unsigned p = 0; p < raycastImg->width * raycastImg->height; p++) {
        double x = (1.0 / (double)1024) * (p % 1024) - 0.5;
        double y = (1.0 / (double)1024) * (p / 1024) - 0.5;

        double length = sqrt(pow(x, 2.0) + pow(y, 2.0) + pow(dist, 2.0));
        
        if(pointCount > 0 && points != NULL)
            for (unsigned c = 0; c < pointCount; c++) {
                unsigned color = vectorFunc(
                  fabs(x / length) + *(points + (c * 3) + 0), 
                  fabs(y / length) + *(points + (c * 3) + 1),
                  fabs(dist / length) + *(points + (c * 3) + 2)
                );
                *(raycastImg->data + p) = (c == 0)? color : rayMixFunc(color, *(raycastImg->data + p));
            }
        else *(raycastImg->data + p) = vectorFunc(fabs(x / length), fabs(y / length), fabs(dist / length));
    }

    return raycastImg;
}