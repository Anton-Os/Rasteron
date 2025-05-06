static unsigned spirals(double x, double y){
    double centerAngle = atan((y - 0.5) / (x - 0.5));
    double centerDist = sqrt(pow(x - 0.5, 2) + pow(y - 0.5, 2));

    double factor = (centerAngle * segs) - floor(centerAngle * segs) * tan(centerDist * (segs + dist));
    return fuse_colors(color_level(color1, sin(centerAngle * segs)), color_level(color2, cos(centerDist * dist)), factor);
}

Rasteron_Image* spiralsImgOp(double s, double d){
    segs = s;
    dist = d;

    return mapImgOp((ImageSize){ 1024, 1024 }, spirals);
}

static unsigned radialMix1(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return diff_colors(diff_colors(c1, c4), c3 - c2); } // starting
static unsigned radialMix2(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return diff_colors(diff_colors(c2, c3), c4 + c1); } // reverse
static unsigned radialMix3(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return diff_colors(diff_colors(c3, c1), c2 * c4); } // odds and evens
static unsigned radialMix4(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return diff_colors(diff_colors(c4, c2), pow(c1, 1.0 / (double)(c3 & 0xFF))); } // evens and odds
static unsigned radialMix5(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return diff_colors(c3 - c2, diff_colors(c1, c4)); } // starting alt
static unsigned radialMix6(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return diff_colors(c4 + c1, diff_colors(c2, c3)); } // reverse alt
static unsigned radialMix7(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return diff_colors(c2 * c4, diff_colors(c3, c1)); } // odds and evens alt
static unsigned radialMix8(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return diff_colors(pow(c1, 1.0 / (double)(c3 & 0xFF)), diff_colors(c4, c2)); } // evens and odds alt
static unsigned radialMix9(unsigned c1, unsigned c2, unsigned c3, unsigned c4){ return (c1 + c2 > c3 + c4)? blend_colors(c1, c2, 0.5) : fuse_colors(c3, c4, 0.5); } // inbetween

Rasteron_Image* radialImgOp(unsigned colors[4], mixCallback4 mix_callback){
    Rasteron_Image* radialImgs[4] = {
        gradientImgOp((ImageSize){ 1024, 1024 }, SIDE_Radial, colors[0], colors[1]),
        gradientImgOp((ImageSize){ 1024 / 2, 1024 / 2 }, SIDE_Radial, colors[1], colors[2]),
        gradientImgOp((ImageSize){ 1024 / 4, 1024 / 4 }, SIDE_Radial, colors[2], colors[3]),
        gradientImgOp((ImageSize){ 1024 / 6, 1024 / 6 }, SIDE_Radial, colors[3], colors[0])
    };

    Rasteron_Image* radialImg = mixingExtImgOp(radialImgs[0], radialImgs[1], radialImgs[2], radialImgs[3], mix_callback);

    for(unsigned i = 0; i < 4; i++) RASTERON_DEALLOC(radialImgs[i]);

    return radialImg;
}

static unsigned mandalaMix1(unsigned c1, unsigned c2){ return c1 + c2; }
static unsigned mandalaMix2(unsigned c1, unsigned c2){ return c1 * c2; }
static unsigned mandalaMix3(unsigned c1, unsigned c2){ return root_colors(c1, c2); }
static unsigned mandalaMix4(unsigned c1, unsigned c2){ return (c1 * 2) - (c2 / 2); }
static unsigned mandalaMix5(unsigned c1, unsigned c2){ return (c1 > c2)? color_invert(c1) : color_invert(c2); }
static unsigned mandalaMix6(unsigned c1, unsigned c2){ return (c1 * color_invert(c2) > color_invert(c1) * c2)? c1 : c2; }
static unsigned mandalaMix7(unsigned c1, unsigned c2){ return (fuse_colors(c1, c2, 0.5F) > blend_colors(c1, c2, 0.5F))? c1 : c2; }

static unsigned restoreMix(unsigned c1, unsigned c2){
    static unsigned lastColor;
    unsigned newColor = (c1 == 0xFF000000 || c1 == NO_COLOR || c1 == lastColor)? c2 : c1;
    lastColor = c1;
    return newColor;
}

float mandalaInterp1(unsigned i, float coords[2], float a, float d){ return tan((coords[0] + coords[1]) / (i * RADIAL_INVOKE)) * pow(a, d); }
float mandalaInterp2(unsigned i, float coords[2], float a, float d){ return tan((coords[0] + coords[1]) / (i * RADIAL_INVOKE)) * pow(d, a); }
float mandalaInterp3(unsigned i, float coords[2], float a, float d){ return sin((coords[0] - coords[1]) / (i * RADIAL_INVOKE)) * pow(a, d); }
float mandalaInterp4(unsigned i, float coords[2], float a, float d){ return cos((coords[0] - coords[1]) / (i * RADIAL_INVOKE)) * pow(d, a); }
float mandalaInterp5(unsigned i, float coords[2], float a, float d){ return tan((coords[0] * coords[1]) / (i * RADIAL_INVOKE)) * pow(d, a); }
float mandalaInterp6(unsigned i, float coords[2], float a, float d){ return sin((coords[0] / coords[1]) / (i * RADIAL_INVOKE)) * pow(a, d); }
float mandalaInterp7(unsigned i, float coords[2], float a, float d){ return cos((pow(coords[0], coords[1])) / (i * RADIAL_INVOKE)) * pow(d, a); }

float (*interpolate)(unsigned, float[2], float, float) = &mandalaInterp7;

static unsigned mandalaMap(double x, double y){
    static unsigned invocation = 0;

    float coords[2] = { x, y };
    double centerAngle = atan((y - 0.5) / (x - 0.5));
    double centerDist = sqrt(pow(x - 0.5, 2) + pow(y - 0.5, 2));

    unsigned mandalaColor = blend_colors(
        fuse_colors(color1, color2, sin(y / centerAngle) + cos(x / centerDist)), 
        diff_colors(color_invert(color1), color_invert(color2)),
        interpolate(invocation, coords, centerAngle, centerDist)
    );

    invocation++;
    return mandalaColor;
}

Rasteron_Image* mandalaImgOp(coordCallback coord_callback, mixCallback mix_callback, float (*interp_callback)(unsigned, float[2], float, float)){
    if(interp_callback != NULL) interpolate = interp_callback;

    Rasteron_Image* mapImg1 = mapImgOp((ImageSize){ 1024, 1024 }, coord_callback);
    Rasteron_Image* mapImg2 = mapImgOp((ImageSize){ 1024, 1024 }, coord_callback);

    Rasteron_Image* symetricImg1 = mixingImgOp(mapImg1, mapImg2, mix_callback);
    Rasteron_Image* symetricImg2 = flipImgOp(symetricImg1, FLIP_Clock);

    Rasteron_Image* mandalaImg = mixingImgOp(symetricImg1, symetricImg2, restoreMix); // TODO: Find a way to simply copy

    RASTERON_DEALLOC(mapImg1); RASTERON_DEALLOC(mapImg2);
    RASTERON_DEALLOC(symetricImg1); RASTERON_DEALLOC(symetricImg2);

    return mandalaImg;
}