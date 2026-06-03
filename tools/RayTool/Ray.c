unsigned rayFunc1(double x, double y, double z) {
    unsigned color = blend_colors(rayColor1, rayColor2, pow(x + y + z, x * y * z) * 0.5);
    return vecMixFunc((mode < 0)? rayColor1 : color, color);
}
unsigned rayFunc2(double x, double y, double z) {
    unsigned color = blend_colors(rayColor1, rayColor2, pow(x * y * z, x + y + z) * 0.5);
    return vecMixFunc((mode < 0)? rayColor1 : color, color);
}
unsigned rayFunc3(double x, double y, double z) {
    unsigned color = blend_colors(rayColor1, rayColor2, pow(x - (y + z), x + y + z) * 0.5);
    return vecMixFunc((mode < 0)? rayColor1 : color, color);
}
unsigned rayFunc4(double x, double y, double z) {
    unsigned color = blend_colors(rayColor1, rayColor2, pow(x + (y - z), -x + y + z) * 0.5);
    return vecMixFunc((mode < 0)? rayColor1 : color, color);
}
unsigned rayFunc5(double x, double y, double z) {
    unsigned color = blend_colors(rayColor1, rayColor2, pow((x / y) * z, x * (y / z)) * 0.5);
    return vecMixFunc((mode < 0)? rayColor1 : color, color);
}
unsigned rayFunc6(double x, double y, double z) {
    unsigned color = blend_colors(rayColor1, rayColor2, pow(x / (y + z), y / (x - z)) * 0.5);
    return vecMixFunc((mode < 0)? rayColor1 : color, color);
}
unsigned rayFunc7(double x, double y, double z) {
    unsigned color = blend_colors(rayColor1, rayColor2, pow(sin((x + y + z) * 10), cos(x * y * z * 10)) * 0.5);
    return vecMixFunc((mode < 0)? rayColor1 : color, color);
}
unsigned rayFunc8(double x, double y, double z) {
    unsigned color = blend_colors(rayColor1, rayColor2, pow(cos((x + y + z) * 10), sin(x * y * z * 10)) * 0.5);
    return vecMixFunc((mode < 0)? rayColor1 : color, color);
}
unsigned rayFunc9(double x, double y, double z) {
    unsigned color = root_colors(rayColor1, rayColor2, (x * y) / z);
    return vecMixFunc((mode < 0)? rayColor1 : color, color);
}

unsigned rayFunc10(double x, double y, double z) {
    unsigned color = root_colors(rayColor1, rayColor2, (x - y) + z);
    return vecMixFunc((mode < 0)? rayColor1 : color, color);
}

static unsigned invertMix1(unsigned color1, unsigned color2) { return (0xFFFFFFFF - ((color1 > color2)? color_invert(color1 - color2) : color_invert(color2 - color1))) | 0xFF000000; }

static unsigned invertMix2(unsigned color1, unsigned color2) { return (0xFFFFFFFF - ((color1 > color2)? color_invert(color1 + color2) : color_invert(color2 - color1))) | 0xFF000000; }

static unsigned invertMix3(unsigned color1, unsigned color2) { return (0xFFFFFFFF - ((color1 > color2)? color_invert(color1 + color2) : color_invert(color2 * color1))) | 0xFF000000; }

// Image Functions

Rasteron_Image* raycastImgOp(ImageSize size, float* points, unsigned pointCount, double dist){ 
    Rasteron_Image* raycastImg = RASTERON_ALLOC("raycast", size.height, size.width);

    for (unsigned p = 0; p < raycastImg->width * raycastImg->height; p++) {
        double x = (1.0 / (double)1024) * (p % 1024) - 0.5;
        double y = (1.0 / (double)1024) * (p / 1024) - 0.5;

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