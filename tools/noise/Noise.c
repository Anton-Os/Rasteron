#include "NoiseTool.h"

#define NOISE_TAU 6.28318530717958647692

// --------------------------------   TexTest Callbacks    -------------------------------- //

float levelsNoiseMod(float value){ return (value < 0.25)? 0.0f : (value > 0.75)? 1.0f : 0.5f; }
float rangeNoiseMod(float value){ return (float)fabs((value - 0.5F) * 1.5F); }
float powerNoiseMod(float value){ return (float)pow(value, _noise.power); }
float trigNoiseMod(float value){ return (float)fabs(sin(value * 1.57079632 * _noise.power)); }
float quiltNoiseMod(float value){
	static float m = 0.0;
	float newMod = (m != 0.0f && value / m > 1.0)? value + (m * 0.5f) : value - (m * 0.5f);
	m = value;
	return newMod;
}

noiseCallback noiseModifier(int modifier){
    switch(modifier){
        case NMOD_Levels: return levelsNoiseMod;
        case NMOD_Range: return rangeNoiseMod;
        case NMOD_Power: return powerNoiseMod;
        case NMOD_Trig: return trigNoiseMod;
        case NMOD_Quilt: return quiltNoiseMod;
        default: return NULL;
    }
}

// --------------------------------   TexTest Perturb Noise    -------------------------------- //

Rasteron_Image* purturbNoiseImgOp(ImageSize size, ColorGrid grid, noiseCallback callback, double xOff, double yOff){
    if(xOff == 0.0 && yOff == 0.0) return noiseExtImgOp(size, grid, callback);

    Rasteron_Image* noiseImg = RASTERON_ALLOC("purturb_noise", size.height, size.width);

    const unsigned xCellPoints = grid.xCells + 1;
    const unsigned yCellPoints = grid.yCells + 1;

    Rasteron_Image* gridImg = RASTERON_ALLOC("grid", yCellPoints, xCellPoints);
    for (unsigned p = 0; p < gridImg->width * gridImg->height; p++) {
        double noiseVal = (double)rand() / (double)RAND_MAX;
        *(gridImg->data + p) = blend_colors(grid.color1, grid.color2, noiseVal);
    }

    unsigned* topLeft = gridImg->data; unsigned* topRight = gridImg->data + 1;
    unsigned* botLeft = gridImg->data + gridImg->width; unsigned* botRight = gridImg->data + gridImg->width + 1;
    const unsigned xSwitch = noiseImg->width / grid.xCells;
    const unsigned ySwitch = noiseImg->height / grid.yCells;

    for(unsigned p = 0; p < noiseImg->width * noiseImg->height; p++){
        unsigned xOffset = p % noiseImg->width;
        unsigned yOffset = p / noiseImg->width;

        if(xOffset == 0){
            unsigned row = ((yOffset / ySwitch < grid.yCells)? yOffset / ySwitch : grid.yCells - 1) * gridImg->width;
            topLeft = gridImg->data + row;
            topRight = gridImg->data + 1 + row;
            botLeft = gridImg->data + gridImg->width + row;
            botRight = gridImg->data + gridImg->width + 1 + row;
        }
        else if (xOffset % xSwitch == 0 && xOffset / xSwitch < grid.xCells) { topLeft++; topRight++; botLeft++; botRight++; }

        double xFrac = (double)(xOffset % xSwitch) / (double)xSwitch;
        double yFrac = (double)(yOffset % ySwitch) / (double)ySwitch;
        if(callback != NULL){ xFrac = callback((float)xFrac); yFrac = callback((float)yFrac); }
        xFrac += (((double)rand() / (double)RAND_MAX) - 0.5F) * xOff * 2.0; // random perturb X offset
        yFrac += (((double)rand() / (double)RAND_MAX) - 0.5F) * yOff * 2.0; // random perturb Y offset

        *(noiseImg->data + p) = blend_colors(blend_colors(*topLeft, *topRight, xFrac), blend_colors(*botLeft, *botRight, xFrac), yFrac);
    }

    RASTERON_DEALLOC(gridImg);
    return noiseImg;
}

// --------------------------------   Lattice Noise    -------------------------------- //

static int _perm[512];
static double _gradX[256], _gradY[256];

void latticeSeed(int gradients){
    for(int i = 0; i < 256; i++) _perm[i] = i;
    for(int i = 255; i > 0; i--){ // shuffle
        int j = rand() % (i + 1);
        int t = _perm[i]; _perm[i] = _perm[j]; _perm[j] = t;
    }
    for(int i = 0; i < 256; i++) _perm[256 + i] = _perm[i];

    if(gradients < 2) gradients = 2;
    double turn = (double)rand() / RAND_MAX; // rotates the gradient set
    for(int i = 0; i < 256; i++){
        double angle = NOISE_TAU * ((double)(i % gradients) + turn) / (double)gradients;
        _gradX[i] = cos(angle);
        _gradY[i] = sin(angle);
    }
}

static int wrapIndex(int v, int period){
    if(period <= 0) return v & 255;
    v %= period;
    if(v < 0) v += period;
    return v & 255;
}

static double fade(double t){ return t * t * t * (t * (t * 6.0 - 15.0) + 10.0); }

static double gradDot(int hash, double x, double y){ return _gradX[hash & 255] * x + _gradY[hash & 255] * y; }

double perlinNoise(double x, double y, int xPeriod, int yPeriod){
    int xi = (int)floor(x), yi = (int)floor(y);
    double fx = x - xi, fy = y - yi;
    int x0 = wrapIndex(xi, xPeriod), x1 = wrapIndex(xi + 1, xPeriod);
    int y0 = wrapIndex(yi, yPeriod), y1 = wrapIndex(yi + 1, yPeriod);

    double n00 = gradDot(_perm[_perm[x0] + y0], fx, fy);
    double n10 = gradDot(_perm[_perm[x1] + y0], fx - 1.0, fy);
    double n01 = gradDot(_perm[_perm[x0] + y1], fx, fy - 1.0);
    double n11 = gradDot(_perm[_perm[x1] + y1], fx - 1.0, fy - 1.0);

    double u = fade(fx), v = fade(fy);
    double nx0 = n00 + u * (n10 - n00);
    double nx1 = n01 + u * (n11 - n01);
    double value = (nx0 + v * (nx1 - nx0)) * 1.41421356;
    return (value > 1.0)? 1.0 : (value < -1.0)? -1.0 : value;
}

double fbmNoise(double x, double y){ // x and y are in lattice units
    double sum = 0.0, amp = 1.0, norm = 0.0, freq = 1.0;
    for(int o = 0; o < _noise.octaves; o++){
        double f = _noise.wrap? floor(freq + 0.5) : freq; // whole frequencies keep wrapping seamless
        int xPeriod = _noise.wrap? (int)(_noise.cells[0] * f) : 0;
        int yPeriod = _noise.wrap? (int)(_noise.cells[1] * f) : 0;
        sum += amp * perlinNoise(x * f + o * 17.31, y * f + o * 9.17, xPeriod, yPeriod);
        norm += amp;
        amp *= _noise.persistence;
        freq *= _noise.lacunarity;
    }
    return (norm > 0.0)? sum / norm : 0.0;
}

static void featurePoint(int cx, int cy, double* px, double* py){ // jittered point inside a lattice cell
    int h = _perm[(_perm[cx & 255] + (cy & 255)) & 511];
    int h2 = _perm[(h + 101) & 511];
    *px = cx + 0.5 + (((double)h / 255.0) - 0.5) * _noise.jitter;
    *py = cy + 0.5 + (((double)h2 / 255.0) - 0.5) * _noise.jitter;
}

double worleyNoise(double x, double y, double* secondDist){
    int cx = (int)floor(x), cy = (int)floor(y);
    double best = 10.0, second = 10.0;
    for(int oy = -1; oy <= 1; oy++)
        for(int ox = -1; ox <= 1; ox++){
            int nx = cx + ox, ny = cy + oy;
            int hx = _noise.wrap? wrapIndex(nx, _noise.cells[0]) : nx;
            int hy = _noise.wrap? wrapIndex(ny, _noise.cells[1]) : ny;
            double px, py;
            featurePoint(hx, hy, &px, &py);
            px += nx - hx; py += ny - hy; // back into unwrapped space
            double d = sqrt((px - x) * (px - x) + (py - y) * (py - y));
            if(d < best){ second = best; best = d; }
            else if(d < second) second = d;
        }
    if(secondDist != NULL) *secondDist = second;
    return best;
}

// --------------------------------   Materials    -------------------------------- //

static double clamp01(double v){ return (v < 0.0)? 0.0 : (v > 1.0)? 1.0 : v; }
static double smooth01(double e0, double e1, double v){ double t = clamp01((v - e0) / (e1 - e0)); return t * t * (3.0 - 2.0 * t); }
static double frac(double v){ return v - floor(v); }
static double unit(double n){ return clamp01(n * 0.5 + 0.5); } // [-1, 1] to [0, 1]
static double hash01(double x, double y){ return frac(sin(x * 127.1 + y * 311.7) * 43758.5453); }

static unsigned ramp(double t){ // low to high color
    return blend_colors(_noise.colors[0], _noise.colors[1], clamp01(t));
}

static unsigned accent(unsigned color, double t){ return blend_colors(color, _noise.colors[2], clamp01(t)); }

static unsigned materialColor(double u, double v){ // u, v in lattice units
    double n, m, d1, d2;
    double cx = _noise.cells[0], cy = _noise.cells[1];

    switch(_noise.material){
    case MATERIAL_Grass: { // tall thin blades with drier patches
        n = fbmNoise(u * 6.0, v * 0.6);
        double blades = frac(u * 9.0 + n * 2.0);
        double blade = smooth01(0.0, 0.5, blades) * (1.0 - smooth01(0.5, 1.0, blades));
        m = unit(fbmNoise(u * 0.5 + 40.0, v * 0.5));
        return accent(ramp(0.25 + 0.75 * blade * unit(n)), smooth01(0.55, 0.85, m) * 0.7);
    }
    case MATERIAL_Ooze: { // warped blobs with a glossy rim
        double wx = fbmNoise(u + 5.2, v + 1.3), wy = fbmNoise(u + 1.7, v + 9.2);
        n = unit(fbmNoise(u + wx * 2.0, v + wy * 2.0));
        double body = smooth01(0.42, 0.58, n);
        double rim = 1.0 - smooth01(0.0, 0.04, fabs(n - 0.6));
        return accent(ramp(body), rim * 0.8);
    }
    case MATERIAL_Sand: { // dune ripples with scattered grains
        n = fbmNoise(u * 0.7, v * 0.7);
        double ripple = 0.5 + 0.5 * sin((v + n * 1.5) * 10.0 + u * 0.8);
        double grain = hash01(floor(u * 160.0 / cx), floor(v * 160.0 / cy));
        unsigned color = ramp(0.55 * ripple + 0.3 * unit(n) + 0.15 * grain);
        return (grain > 0.985)? _noise.colors[2] : color;
    }
    case MATERIAL_Metal: { // brushed streaks with a sheen band
        n = fbmNoise(u * 0.4, v * 25.0);
        double scratch = (hash01(floor(v * 300.0 / cy), 3.0) > 0.97)? 0.25 : 0.0;
        double sheen = 0.5 + 0.5 * cos((v / cy - 0.35) * 5.0);
        return accent(ramp(0.35 + 0.35 * unit(n) + 0.3 * sheen * sheen), scratch + 0.4 * pow(sheen, 12.0));
    }
    case MATERIAL_Fabric: { // over-under weave of threads
        double threads = 1.0; // threads per lattice cell
        double tu = u * threads, tv = v * threads;
        int iu = (int)floor(tu), iv = (int)floor(tv);
        double across = fabs(sin(frac(tv) * 3.14159265)); // horizontal thread profile
        double down = fabs(sin(frac(tu) * 3.14159265)); // vertical thread profile
        int over = ((iu + iv) & 1) == 0;
        double fuzz = 0.15 * unit(fbmNoise(u * 8.0, v * 8.0));
        double shade = over? across : down;
        unsigned threadColor = over? _noise.colors[1] : _noise.colors[2];
        return blend_colors(blend_colors(_noise.colors[0], threadColor, 0.35 + 0.65 * shade), _noise.colors[0], fuzz);
    }
    case MATERIAL_Wood: { // growth rings drifting across planks with fine grain
        n = fbmNoise(u * 0.35, v * 1.5);
        double ring = frac(v * 2.5 + n * 0.9 + 0.12 * sin(u * 0.7));
        double late = smooth01(0.7, 0.95, ring); // darker late wood at each ring edge
        double grain = unit(fbmNoise(u * 0.25 + 50.0, v * 30.0));
        return accent(ramp(0.35 + 0.45 * (1.0 - late) + 0.2 * grain), late * 0.75);
    }
    case MATERIAL_Marble: { // turbulent veins
        n = fbmNoise(u, v);
        double vein = pow(fabs(sin((u + v * 0.5) * 1.3 + n * 6.0)), 0.25);
        double fine = 1.0 - smooth01(0.0, 0.05, fabs(fbmNoise(u * 2.0 + 7.0, v * 2.0)));
        return accent(ramp(vein), fine * 0.6);
    }
    case MATERIAL_Stone: { // cracked cells
        d1 = worleyNoise(u, v, &d2);
        double crack = smooth01(0.0, 0.08, d2 - d1);
        n = unit(fbmNoise(u * 2.0, v * 2.0));
        return accent(blend_colors(_noise.colors[2], ramp(0.3 + 0.7 * n * (1.0 - d1 * 0.5)), crack), 0.0);
    }
    case MATERIAL_Water: { // caustic ridges
        double wx = fbmNoise(u * 0.5 + 3.0, v * 0.5);
        n = 1.0 - fabs(fbmNoise(u + wx, v + wx));
        double caustic = pow(n, 6.0);
        return accent(ramp(0.3 + 0.5 * unit(wx)), caustic);
    }
    case MATERIAL_Lava: { // dark crust with glowing ridges
        double wx = fbmNoise(u + 2.0, v + 8.0);
        n = 1.0 - fabs(fbmNoise(u + wx * 0.8, v - wx * 0.8));
        double glow = smooth01(0.75, 0.98, n);
        return accent(ramp(unit(wx) * 0.4 + glow * 0.6), pow(glow, 3.0));
    }
    case MATERIAL_Clouds: { // soft coverage
        n = unit(fbmNoise(u, v));
        return accent(blend_colors(_noise.colors[0], _noise.colors[1], smooth01(0.45, 0.75, n)), smooth01(0.8, 1.0, n) * 0.6);
    }
    case MATERIAL_Rust: { // blotches with pitting
        n = unit(fbmNoise(u, v));
        double blotch = smooth01(0.45, 0.6, n);
        double pit = (hash01(floor(u * 200.0 / cx), floor(v * 200.0 / cy)) > 0.97)? 0.6 : 0.0;
        return blend_colors(ramp(blotch * 0.8 + 0.2 * unit(fbmNoise(u * 4.0, v * 4.0))), _noise.colors[2], pit * blotch);
    }
    default: return 0;
    }
}

Rasteron_Image* latticeImgOp(ImageSize size){
    Rasteron_Image* noiseImg = RASTERON_ALLOC("lattice_noise", size.height, size.width);
    latticeSeed(_noise.gradients);
    noiseCallback modifier = noiseModifier(_noise.modifier);

    for(unsigned p = 0; p < noiseImg->width * noiseImg->height; p++){
        double x = (double)(p % size.width) / (double)size.width;
        double y = (double)(p / size.width) / (double)size.height;
        double u = x * _noise.cells[0] * _noise.stretch[0] + _noise.offset[0];
        double v = y * _noise.cells[1] * _noise.stretch[1] + _noise.offset[1];

        if(_noise.warp > 0.0){ // domain warp before sampling
            double wu = fbmNoise(u + 31.4, v + 12.7), wv = fbmNoise(u + 4.2, v + 27.1);
            u += wu * _noise.warp; v += wv * _noise.warp;
        }

        unsigned color;
        if(_noise.material != MATERIAL_None) color = materialColor(u, v);
        else {
            double value;
            if(_noise.type == NOISE_Worley){
                double second;
                double first = worleyNoise(u, v, &second);
                value = clamp01(first * 0.8);
            }
            else value = unit(fbmNoise(u, v));
            if(modifier != NULL) value = clamp01(modifier((float)value));
            color = ramp(value);
        }
        *(noiseImg->data + p) = color | 0xFF000000;
    }
    return noiseImg;
}
