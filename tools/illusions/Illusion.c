#include "IllusionTool.h"

#define I_DARK S->colors[ICOLOR_Dark]
#define I_LIGHT S->colors[ICOLOR_Light]
#define I_ACCENT S->colors[ICOLOR_Accent]
#define I_MID S->colors[ICOLOR_Mid]

static const Illusion_State* S = &_illusion; // state being rendered
static double _aspect = 1.0; // width over height of the image being generated

// --------------------------------   Illusion Catalogue    -------------------------------- //

const Illusion_Info _illusionInfo[ILLUSION_Count] = {
    { "cafewall", "the grey mortar lines are straight and parallel but look sloped", "lines up the tiles, the slope disappears",
        { 12, 9 }, 0.5, 0.5, 0.4, 0.0, { 0xFF000000, 0xFFFFFFFF, 0xFFE03030, 0xFF808080 }, TRUE },
    { "hermann", "grey blobs appear at the crossings you are not looking at", NULL,
        { 7, 7 }, 0.5, 0.5, 0.5, 0.0, { 0xFF000000, 0xFFFFFFFF, 0xFFE03030, 0xFF808080 }, TRUE },
    { "scintillating", "black dots flash inside the white discs away from where you look", "removes the discs",
        { 8, 8 }, 0.5, 0.5, 0.5, 0.0, { 0xFF000000, 0xFFFFFFFF, 0xFFE03030, 0xFF808080 }, TRUE },
    { "zollner", "the long lines are parallel but seem to lean toward and away from each other", "removes the hatch marks",
        { 26, 7 }, 0.5, 0.7, 0.4, 45.0, { 0xFF000000, 0xFFFFFFFF, 0xFFE03030, 0xFF808080 }, FALSE },
    { "hering", "the two colored lines are straight but seem to bow outward", "removes the fan of lines",
        { 20, 1 }, 0.5, 0.5, 0.4, 0.0, { 0xFF000000, 0xFFFFFFFF, 0xFFE03030, 0xFF808080 }, FALSE },
    { "poggendorff", "the slanted line continues straight behind the bar but looks shifted", "shows the line passing through the bar",
        { 1, 1 }, 0.5, 0.6, 0.4, 0.0, { 0xFF000000, 0xFFFFFFFF, 0xFFE03030, 0xFFB0B0B0 }, FALSE },
    { "mullerlyer", "the shafts have the same length, the one with outward fins looks longer", "marks both ends of the shafts",
        { 1, 2 }, 0.5, 0.5, 0.5, 0.0, { 0xFF000000, 0xFFFFFFFF, 0xFFE03030, 0xFF808080 }, FALSE },
    { "ponzo", "the two colored bars have the same length, the upper one looks longer", "removes the converging rails",
        { 1, 9 }, 0.5, 0.8, 0.5, 0.0, { 0xFF000000, 0xFFFFFFFF, 0xFFE03020, 0xFF909090 }, FALSE },
    { "ebbinghaus", "the two colored discs have the same size, the one among small discs looks larger", "removes the surrounding discs",
        { 6, 9 }, 0.5, 0.6, 0.5, 0.0, { 0xFF4070B0, 0xFFFFFFFF, 0xFFF08020, 0xFF808080 }, FALSE },
    { "jastrow", "the two arcs are identical, the lower one looks longer", NULL,
        { 1, 1 }, 0.5, 0.5, 0.5, 0.0, { 0xFF000000, 0xFFFFFFFF, 0xFFE06030, 0xFF808080 }, FALSE },
    { "kanizsa", "a bright triangle with clear edges appears although none is drawn", "outlines where the triangle is seen",
        { 3, 1 }, 0.5, 0.5, 0.4, 0.0, { 0xFF000000, 0xFFFFFFFF, 0xFFE03030, 0xFF808080 }, FALSE },
    { "white", "the grey bars are the same grey, the left ones look lighter", "removes the stripes",
        { 1, 12 }, 0.5, 0.5, 0.5, 0.0, { 0xFF000000, 0xFFFFFFFF, 0xFFE03030, 0xFF808080 }, FALSE },
    { "munker", "the colored bars are the same color on both sides but look different", "removes the stripes",
        { 1, 14 }, 0.5, 0.5, 0.5, 0.0, { 0xFF2040D0, 0xFFF0E030, 0xFFE02020, 0xFF808080 }, FALSE },
    { "contrast", "the squares are the same grey, they look darker toward the bright side", "flattens the background",
        { 5, 1 }, 0.5, 0.5, 0.5, 0.0, { 0xFF000000, 0xFFFFFFFF, 0xFFE03030, 0xFF808080 }, FALSE },
    { "chevreul", "each band is one flat grey but looks lighter next to its darker neighbor", "separates the bands",
        { 8, 1 }, 0.5, 0.5, 0.5, 0.0, { 0xFF202020, 0xFFE0E0E0, 0xFFE03030, 0xFF808080 }, FALSE },
    { "cornsweet", "both halves are the same grey away from the middle, the right half looks lighter", "covers the edge",
        { 1, 1 }, 0.5, 0.6, 0.5, 0.0, { 0xFF000000, 0xFFFFFFFF, 0xFFE03030, 0xFF808080 }, FALSE },
    { "fraser", "the twisted cords are perfect circles but look like one spiral", "draws the cords as plain circles",
        { 16, 6 }, 0.5, 0.7, 0.5, 0.0, { 0xFF000000, 0xFFFFFFFF, 0xFFE03030, 0xFF909090 }, TRUE },
    { "pinna", "fix the center and move your head closer and away, the rings turn against each other", NULL,
        { 16, 3 }, 0.5, 0.7, 0.4, 0.0, { 0xFF000000, 0xFFFFFFFF, 0xFFE03030, 0xFF808080 }, TRUE },
    { "drift", "the discs seem to rotate slowly when you look next to them", "removes the color steps that cause the drift",
        { 3, 3 }, 0.5, 0.5, 0.5, 0.0, { 0xFF000000, 0xFFFFFFFF, 0xFF2030C0, 0xFFE0D020 }, TRUE },
    { "ouchi", "the disc seems to float and slide over the background when your eyes move", "outlines the disc",
        { 7, 1 }, 0.5, 0.5, 0.5, 0.0, { 0xFF000000, 0xFFFFFFFF, 0xFFE03030, 0xFF808080 }, FALSE },
    { "bulge", "the checkerboard is flat and straight but seems to bulge toward you", "removes the small squares",
        { 14, 14 }, 0.5, 0.6, 0.5, 0.0, { 0xFF000000, 0xFFFFFFFF, 0xFFE03030, 0xFF808080 }, FALSE },
    { "moire", "two plain sets of rings overlap into bands that neither set contains", "shows one set of rings alone",
        { 10, 1 }, 0.5, 0.3, 0.4, 0.0, { 0xFF000000, 0xFFFFFFFF, 0xFFE03030, 0xFF808080 }, TRUE },
    { "lilac", "stare at the cross while it animates, the gap turns into a green dot and the others fade", "fills the gap",
        { 12, 1 }, 0.5, 0.5, 0.5, 0.0, { 0xFF000000, 0xFFFFFFFF, 0xFFE040E0, 0xFFB0B0B0 }, TRUE },
};

void illusionPreset(Illusion_State* state, int type){
    const Illusion_Info* info = &_illusionInfo[type];
    state->type = type;
    for(unsigned c = 0; c < 4; c++) state->colors[c] = info->colors[c];
    state->count[0] = info->count[0]; state->count[1] = info->count[1];
    state->size = info->size;
    state->strength = info->strength;
    state->thickness = info->thickness;
    state->angle = info->angle;
    state->zoom = 1.0;
    state->phase = 0.0;
    state->reveal = FALSE;
}

// --------------------------------   Helpers    -------------------------------- //

static double fracPart(double v){ return v - floor(v); }

static double clampUnit(double v){ return (v < 0.0)? 0.0 : (v > 1.0)? 1.0 : v; }

static int isOdd(double v){ return (int)(((long)floor(v)) & 1); }

static double lineHalf(void){ return 0.001 + (0.008 * S->thickness); } // half width of drawn lines

static unsigned mixColors(unsigned color1, unsigned color2, double t){ // opaque blend, 0 gives color1
    t = clampUnit(t);
    unsigned result = 0xFF000000;
    for(unsigned shift = 0; shift <= 16; shift += 8){
        double c1 = (double)((color1 >> shift) & 0xFF), c2 = (double)((color2 >> shift) & 0xFF);
        result |= (unsigned)(c1 + ((c2 - c1) * t) + 0.5) << shift;
    }
    return result;
}

static double segmentDist(double px, double py, double ax, double ay, double bx, double by){ // distance from a point to a segment
    double dx = bx - ax, dy = by - ay;
    double lengthSq = (dx * dx) + (dy * dy);
    double t = (lengthSq > 0.0)? clampUnit((((px - ax) * dx) + ((py - ay) * dy)) / lengthSq) : 0.0;
    return hypot(px - (ax + (t * dx)), py - (ay + (t * dy)));
}

static double edgeSide(double px, double py, double ax, double ay, double bx, double by){ return ((bx - ax) * (py - ay)) - ((by - ay) * (px - ax)); }

static int insideTriangle(double px, double py, const double x[3], const double y[3]){
    double s1 = edgeSide(px, py, x[0], y[0], x[1], y[1]), s2 = edgeSide(px, py, x[1], y[1], x[2], y[2]), s3 = edgeSide(px, py, x[2], y[2], x[0], y[0]);
    return (s1 >= 0.0 && s2 >= 0.0 && s3 >= 0.0) || (s1 <= 0.0 && s2 <= 0.0 && s3 <= 0.0);
}

static double triangleDist(double px, double py, const double x[3], const double y[3]){ // distance to the outline
    double dist = segmentDist(px, py, x[0], y[0], x[1], y[1]);
    dist = fmin(dist, segmentDist(px, py, x[1], y[1], x[2], y[2]));
    return fmin(dist, segmentDist(px, py, x[2], y[2], x[0], y[0]));
}

// --------------------------------   Grids    -------------------------------- //

static unsigned cafeWall(double u, double v){
    static const double steps[4] = { 0.0, 0.5, 1.0, 0.5 }; // rows slide right and back again
    double rowPos = (v + 0.5) * S->count[1];
    double row = floor(rowPos), inRow = rowPos - row;
    double mortar = 0.02 + (0.16 * S->thickness); // fraction of a row
    if(inRow < mortar * 0.5 || inRow > 1.0 - (mortar * 0.5)) return I_MID;

    double shift = S->reveal? 0.0 : S->strength * steps[((int)row % 4 + 4) % 4];
    double column = (((u / _aspect) + 0.5) * S->count[0]) + shift + (S->phase / ILLUSION_TAU * 2.0);
    return isOdd(column)? I_DARK : I_LIGHT;
}

static unsigned gridIllusion(double u, double v, int scintillating){ // hermann and scintillating grids
    double cellW = _aspect / S->count[0], cellH = 1.0 / S->count[1];
    double x = ((u + (_aspect * 0.5)) / cellW) + (S->phase / ILLUSION_TAU), y = (v + 0.5) / cellH;
    double dx = fabs(x - floor(x + 0.5)) * cellW, dy = fabs(y - floor(y + 0.5)) * cellH; // distance to the nearest street centers
    double street = scintillating? 0.06 + (0.2 * S->thickness) : 0.08 + (0.3 * S->thickness);
    double halfStreet = street * 0.5 * fmin(cellW, cellH);

    if(scintillating){
        if(!S->reveal && hypot(dx, dy) < halfStreet * (1.0 + (0.9 * S->size))) return I_LIGHT; // disc on the crossing
        return (dx < halfStreet || dy < halfStreet)? I_MID : I_DARK;
    }
    return (dx < halfStreet || dy < halfStreet)? I_LIGHT : I_DARK;
}

// --------------------------------   Tilt and Alignment    -------------------------------- //

static unsigned zollner(double u, double v){
    double spacing = 1.0 / S->count[1]; // distance between the long lines
    double x = u / spacing, y = v / spacing;
    double row = floor(y), dy = y - row - 0.5;
    double half = lineHalf() / spacing;

    if(!S->reveal){ // short hatch marks crossing each line, leaning the other way on every second line
        double hatchGap = (_aspect / spacing) / S->count[0];
        double tilt = (90.0 - (65.0 * S->strength)) * (ILLUSION_TAU / 360.0) * (isOdd(row)? 1.0 : -1.0);
        double reach = 0.2 + (0.28 * S->size);
        double nearest = floor(x / hatchGap);
        int span = (int)ceil(reach / hatchGap) + 1;
        for(int k = -span; k <= span; k++){
            double dx = x - ((nearest + k + 0.5) * hatchGap);
            double along = (dx * cos(tilt)) + (dy * sin(tilt)), across = (dy * cos(tilt)) - (dx * sin(tilt));
            if(fabs(along) < reach && fabs(across) < half) return I_DARK;
        }
    }
    return (fabs(dy) < half * 1.3)? I_DARK : I_LIGHT;
}

static unsigned hering(double u, double v){
    double half = lineHalf();
    double gap = 0.05 + (0.2 * S->size);
    if(fabs(fabs(u) - gap) < half * 1.6 && fabs(v) < 0.46) return I_ACCENT; // the two straight lines

    double radius = hypot(u, v);
    if(!S->reveal && radius > 0.0){ // fan of lines through the center
        double step = (ILLUSION_TAU * 0.5) / S->count[0];
        double theta = atan2(v, u);
        double nearest = floor((theta / step) + 0.5) * step;
        if(radius * fabs(sin(theta - nearest)) < half * 0.8) return I_DARK;
    }
    return I_LIGHT;
}

static unsigned poggendorff(double u, double v){
    double half = lineHalf();
    double barHalf = 0.04 + (0.14 * S->size);
    double slant = (70.0 - (45.0 * S->strength)) * (ILLUSION_TAU / 360.0); // angle between the lines and the bar
    double spacing = 0.7 / S->count[1];

    int onLine = FALSE;
    for(int l = 0; l < S->count[1]; l++){
        double offset = (l - ((S->count[1] - 1) * 0.5)) * spacing;
        if(fabs((u * cos(slant)) + ((v - offset) * sin(slant))) < half) onLine = TRUE;
    }

    if(fabs(u) < barHalf && fabs(v) < 0.44){ // bar hiding the middle of the lines
        if(fabs(u) > barHalf - (half * 1.2) || fabs(v) > 0.44 - (half * 1.2)) return I_DARK;
        if(S->reveal) return onLine? I_ACCENT : I_LIGHT;
        return I_MID;
    }
    return (onLine && fabs(u) < barHalf + 0.3)? I_DARK : I_LIGHT;
}

// --------------------------------   Size    -------------------------------- //

static unsigned mullerLyer(double u, double v){
    double half = lineHalf();
    double spacing = 0.72 / S->count[1];
    double reach = 0.12 + (0.26 * S->size); // half length of every shaft
    double fin = fmin(0.38 * reach, 0.42 * spacing);
    double finAngle = (90.0 - (90.0 * fmin(S->strength, 0.85))) * (ILLUSION_TAU / 360.0);

    for(int r = 0; r < S->count[1]; r++){
        double y = v - ((r - ((S->count[1] - 1) * 0.5)) * spacing);
        if(fabs(y) > fin + half) continue;
        if(segmentDist(u, y, -reach, 0.0, reach, 0.0) < half) return I_DARK;
        for(int end = -1; end <= 1; end += 2){ // fins point inward on even rows and outward on odd rows
            double finX = (end * reach) + (((r & 1)? end : -end) * cos(finAngle) * fin);
            if(segmentDist(u, y, end * reach, 0.0, finX, sin(finAngle) * fin) < half) return I_DARK;
            if(segmentDist(u, y, end * reach, 0.0, finX, -sin(finAngle) * fin) < half) return I_DARK;
        }
    }
    if(S->reveal && fabs(fabs(u) - reach) < half * 0.6) return I_ACCENT;
    return I_LIGHT;
}

static unsigned ponzo(double u, double v){
    double half = lineHalf();
    double bottom = 0.4, top = 0.4 * (1.0 - (0.85 * S->strength)); // half distance between the rails
    double barHalf = 0.05 + (0.14 * S->size);

    if(fabs(fabs(v) - 0.24) < half * 2.2 && fabs(u) < barHalf) return I_ACCENT; // the two equal bars
    if(S->reveal) return I_LIGHT;
    if(segmentDist(fabs(u), v, top, -0.5, bottom, 0.5) < half * 1.4) return I_DARK; // rails

    double tie = (v + 0.5) * S->count[1];
    if(fabs(tie - floor(tie) - 0.5) / S->count[1] < half * 0.9 && fabs(u) < top + ((bottom - top) * (v + 0.5))) return I_MID;
    return I_LIGHT;
}

static unsigned ebbinghaus(double u, double v){
    double radius = 0.025 + (0.03 * S->size); // both center discs
    for(int side = -1; side <= 1; side += 2){ // large discs on the left, small discs on the right
        double centerX = side * 0.25;
        if(hypot(u - centerX, v) < radius) return I_ACCENT;
        if(S->reveal) continue;

        int count = (side < 0)? S->count[0] : S->count[1];
        double surround = (side < 0)? radius * (1.0 + (1.6 * S->strength)) : radius * (1.0 - (0.6 * S->strength));
        double ring = radius + 0.015 + surround;
        if(count > 1) ring = fmax(ring, 1.08 * surround / sin(ILLUSION_TAU * 0.5 / count)); // keeps neighbors apart
        double step = ILLUSION_TAU / count;
        double nearest = floor((atan2(v, u - centerX) / step) + 0.5) * step;
        if(hypot(u - centerX - (ring * cos(nearest)), v - (ring * sin(nearest))) < surround) return I_DARK;
    }
    return I_LIGHT;
}

static unsigned jastrow(double u, double v){
    double inner = 0.5, outer = inner + 0.1 + (0.12 * S->size);
    double spread = 0.5; // half angle of each arc in radians
    for(int arc = 0; arc < 2; arc++){
        double dy = v - 0.45 - (arc * (outer - inner + 0.035)); // second arc sits right below the first
        double radius = hypot(u, dy);
        if(radius >= inner && radius <= outer && fabs(atan2(u, -dy)) < spread) return (arc == 0)? I_ACCENT : I_DARK;
    }
    return I_LIGHT;
}

// --------------------------------   Illusory Contours    -------------------------------- //

static unsigned kanizsa(double u, double v){
    double half = lineHalf();
    double reach = 0.16 + (0.24 * S->size);
    double upX[3], upY[3], downX[3], downY[3];
    for(unsigned c = 0; c < 3; c++){
        double angle = (ILLUSION_TAU / 3.0 * c) - (ILLUSION_TAU / 4.0);
        upX[c] = reach * cos(angle); upY[c] = reach * sin(angle); // triangle that is seen but not drawn
        downX[c] = -upX[c]; downY[c] = -upY[c]; // outlined triangle underneath
    }

    if(S->reveal && triangleDist(u, v, upX, upY) < half * 0.7) return I_ACCENT;
    if(insideTriangle(u, v, upX, upY)) return I_LIGHT; // covers the discs and the outline
    for(unsigned c = 0; c < 3; c++) if(hypot(u - upX[c], v - upY[c]) < reach * 0.36) return I_DARK;
    return (triangleDist(u, v, downX, downY) < half)? I_DARK : I_LIGHT;
}

// --------------------------------   Brightness and Color    -------------------------------- //

static unsigned whiteBars(double u, double v, unsigned target){ // white and munker illusions
    double stripe = floor((v + 0.5) * S->count[1]);
    int darkStripe = !isOdd(stripe);
    int inBlock = fabs(((stripe + 0.5) / S->count[1]) - 0.5) < 0.12 + (0.36 * S->size); // bars cover whole stripes
    int inLeft = inBlock && fabs(u + 0.2) < 0.11, inRight = inBlock && fabs(u - 0.2) < 0.11;

    if((inLeft && darkStripe) || (inRight && !darkStripe)) return target; // bars replace dark stripes on the left and light stripes on the right
    if(S->reveal) return I_LIGHT;
    return darkStripe? I_DARK : I_LIGHT;
}

static unsigned contrast(double u, double v){
    double across = (u / _aspect) + 0.5;
    unsigned backdrop = S->reveal? I_LIGHT : mixColors(I_DARK, I_LIGHT, across);

    if(S->count[0] <= 1) return (fabs(v) < 0.04 + (0.1 * S->size) && fabs(across - 0.5) < 0.42)? I_MID : backdrop; // one long bar
    double cell = _aspect / S->count[0];
    double x = (u + (_aspect * 0.5)) / cell;
    double squareHalf = cell * (0.1 + (0.5 * S->size));
    return (fabs((x - floor(x) - 0.5) * cell) < squareHalf && fabs(v) < squareHalf)? I_MID : backdrop;
}

static unsigned chevreul(double u, double v){
    int bands = (S->count[0] > 1)? S->count[0] : 2;
    double position = clampUnit((u / _aspect) + 0.5) * bands;
    double band = fmin(floor(position), bands - 1.0);
    if(S->reveal && (position - band < 0.06 || position - band > 0.94)) return I_ACCENT;
    return mixColors(I_DARK, I_LIGHT, band / (bands - 1.0));
}

static unsigned cornsweet(double u, double v){
    double falloff = 0.02 + (0.12 * S->size);
    if(S->reveal && fabs(u) < falloff * 2.2) return I_DARK;
    double edge = 0.45 * S->strength * exp(-fabs(u) / falloff); // only the edge differs
    return (u < 0.0)? mixColors(I_MID, I_DARK, edge) : mixColors(I_MID, I_LIGHT, edge);
}

// --------------------------------   Apparent Motion and Shape    -------------------------------- //

static unsigned fraser(double u, double v){
    double inner = 0.05;
    double radius = hypot(u, v);
    if(radius < inner) return I_MID;

    double ringPos = log(radius / inner) / log(0.5 / inner) * S->count[1]; // rings grow with distance
    double inRing = ringPos - floor(ringPos) - 0.5;
    double around = (atan2(v, u) + S->phase) / ILLUSION_TAU * S->count[0];
    double cord = 0.1 + (0.24 * S->thickness);

    if(fabs(inRing) < cord){ // circle made of slanted dark and light strands
        if(S->reveal) return I_ACCENT;
        return (fracPart(around + (S->strength * 0.6 * inRing / cord)) < 0.5)? I_DARK : I_LIGHT;
    }
    double spiral = ringPos * 0.5; // backdrop of tiles that lean the same way as the strands
    return (isOdd(around * 2.0 + spiral) != isOdd(ringPos * 2.0))? mixColors(I_MID, I_DARK, 0.3) : mixColors(I_MID, I_LIGHT, 0.3);
}

static unsigned pinna(double u, double v){
    double breathing = 1.0 + (0.1 * sin(S->phase)); // growing and shrinking stands in for moving your head
    u /= breathing; v /= breathing;

    double first = 0.13, last = 0.44;
    double gap = (S->count[1] > 1)? (last - first) / (S->count[1] - 1) : 0.0;
    double ring = (gap > 0.0)? floor(((hypot(u, v) - first) / gap) + 0.5) : 0.0;
    ring = fmax(0.0, fmin(ring, S->count[1] - 1.0));
    double ringRadius = first + (ring * gap);

    double count = fmax(3.0, floor((S->count[0] * ringRadius / first) + 0.5)); // same spacing on every ring
    double step = ILLUSION_TAU / count;
    double center = floor((atan2(v, u) / step) + 0.5) * step;
    double dx = u - (ringRadius * cos(center)), dy = v - (ringRadius * sin(center));

    double squareHalf = (ILLUSION_TAU * first / S->count[0]) * (0.35 + (0.5 * S->size)) * 0.5;
    double tilt = (isOdd(ring)? -1.0 : 1.0) * S->strength * (ILLUSION_TAU / 8.0); // neighboring rings lean opposite ways
    double frame = center + (ILLUSION_TAU / 4.0) + tilt;
    double localX = (dx * cos(frame)) + (dy * sin(frame)), localY = (dy * cos(frame)) - (dx * sin(frame));
    double reach = fmax(fabs(localX), fabs(localY));

    if(reach > squareHalf || reach < squareHalf * (0.85 - (0.3 * S->thickness))) return I_MID;
    if(fabs(localX) > fabs(localY)) return (localX > 0.0)? I_DARK : I_LIGHT; // two dark and two light edges
    return (localY > 0.0)? I_DARK : I_LIGHT;
}

static unsigned drift(double u, double v){
    double cellW = _aspect / S->count[0], cellH = 1.0 / S->count[1];
    double x = (u + (_aspect * 0.5)) / cellW, y = (v + 0.5) / cellH;
    double localX = (x - floor(x) - 0.5) * cellW, localY = (y - floor(y) - 0.5) * cellH;
    double discRadius = 0.49 * fmin(cellW, cellH);
    double radius = hypot(localX, localY);
    if(radius >= discRadius) return mixColors(I_DARK, I_LIGHT, 0.5);

    int rings = 3 + (int)((S->size * 6.0) + 0.5);
    int ring = (int)(radius / discRadius * rings);
    double cycles = 4.0 + (2.0 * ring); // color cycles around this ring
    double turn = (isOdd(x) != isOdd(y))? 1.0 : -1.0; // neighboring discs drift opposite ways
    int step = (int)(fracPart((turn * atan2(localY, localX) / ILLUSION_TAU * cycles) + (ring * 0.125) + (S->phase / ILLUSION_TAU)) * 4.0) & 3;

    if(S->reveal) return (step < 2)? I_DARK : I_LIGHT;
    switch(step){ // drift follows dark, dark color, light, light color
        case 0: return I_DARK;
        case 1: return I_ACCENT;
        case 2: return I_LIGHT;
        default: return I_MID;
    }
}

static unsigned ouchi(double u, double v){
    double radius = hypot(u, v);
    double discRadius = 0.14 + (0.24 * S->size);
    if(S->reveal && fabs(radius - discRadius) < lineHalf()) return I_ACCENT;

    double cells = S->count[0];
    if(radius < discRadius) return (isOdd(u * cells * 4.0) != isOdd(v * cells))? I_DARK : I_LIGHT; // tall checks
    return (isOdd(u * cells) != isOdd(v * cells * 4.0))? I_DARK : I_LIGHT; // wide checks
}

static unsigned bulge(double u, double v){
    double x = u * S->count[0], y = v * S->count[1];
    double cellX = floor(x), cellY = floor(y);
    int lightCell = isOdd(cellX) != isOdd(cellY);
    unsigned cell = lightCell? I_LIGHT : I_DARK, other = lightCell? I_DARK : I_LIGHT;

    double centerX = (cellX + 0.5) / S->count[0], centerY = (cellY + 0.5) / S->count[1];
    double falloff = 1.0 - (hypot(centerX, centerY) / 0.46);
    if(S->reveal || falloff <= 0.0) return cell;

    double square = (0.1 + (0.24 * S->size)) * fmin(1.0, falloff * 2.0) * (0.4 + (1.2 * S->strength)); // small squares shrink toward the rim
    double inset = 0.06;
    double inX = x - cellX, inY = y - cellY;
    int mainDiagonal = ((centerX * centerY) > 0.0) != lightCell; // corners are picked by quadrant
    for(int corner = 0; corner < 2; corner++){
        double cornerX = (corner == 0)? inset : 1.0 - inset - square;
        double cornerY = (mainDiagonal == (corner == 0))? inset : 1.0 - inset - square;
        if(inX >= cornerX && inX <= cornerX + square && inY >= cornerY && inY <= cornerY + square) return other;
    }
    return cell;
}

// --------------------------------   Interference and Afterimages    -------------------------------- //

static unsigned moire(double u, double v){
    double frequency = S->count[0] * 4.0;
    double duty = 0.2 + (0.5 * S->thickness);
    double apart = 0.01 + (0.12 * S->strength); // half distance between the two ring centers
    double offsetX = apart * cos(S->phase), offsetY = apart * sin(S->phase);

    int first = fracPart(hypot(u - offsetX, v - offsetY) * frequency) < duty;
    int second = !S->reveal && fracPart(hypot(u + offsetX, v + offsetY) * frequency) < duty;
    return (first || second)? I_DARK : I_LIGHT;
}

static unsigned lilac(double u, double v){
    double half = lineHalf();
    if((fabs(u) < half && fabs(v) < 0.022) || (fabs(v) < half && fabs(u) < 0.022)) return I_DARK; // cross to stare at

    int count = S->count[0];
    int gap = S->reveal? -1 : (int)(fracPart(S->phase / ILLUSION_TAU) * count) % count; // the missing disc moves around
    double blur = 0.03 + (0.05 * S->size);
    double step = ILLUSION_TAU / count;
    long nearest = (long)floor(((atan2(v, u) + (ILLUSION_TAU / 4.0)) / step) + 0.5);

    double amount = 0.0;
    for(long k = nearest - 1; k <= nearest + 1; k++){
        if((int)(((k % count) + count) % count) == gap) continue;
        double angle = (k * step) - (ILLUSION_TAU / 4.0);
        double dist = hypot(u - (0.3 * cos(angle)), v - (0.3 * sin(angle)));
        amount = fmax(amount, exp(-(dist * dist) / (blur * blur)));
    }
    return mixColors(I_MID, I_ACCENT, amount);
}

// --------------------------------   Image Operations    -------------------------------- //

unsigned illusionColor(double u, double v){
    switch(S->type){
        case ILLUSION_CafeWall: return cafeWall(u, v);
        case ILLUSION_Hermann: return gridIllusion(u, v, FALSE);
        case ILLUSION_Scintillating: return gridIllusion(u, v, TRUE);
        case ILLUSION_Zollner: return zollner(u, v);
        case ILLUSION_Hering: return hering(u, v);
        case ILLUSION_Poggendorff: return poggendorff(u, v);
        case ILLUSION_MullerLyer: return mullerLyer(u, v);
        case ILLUSION_Ponzo: return ponzo(u, v);
        case ILLUSION_Ebbinghaus: return ebbinghaus(u, v);
        case ILLUSION_Jastrow: return jastrow(u, v);
        case ILLUSION_Kanizsa: return kanizsa(u, v);
        case ILLUSION_White: return whiteBars(u, v, I_MID);
        case ILLUSION_Munker: return whiteBars(u, v, I_ACCENT);
        case ILLUSION_Contrast: return contrast(u, v);
        case ILLUSION_Chevreul: return chevreul(u, v);
        case ILLUSION_Cornsweet: return cornsweet(u, v);
        case ILLUSION_Fraser: return fraser(u, v);
        case ILLUSION_Pinna: return pinna(u, v);
        case ILLUSION_Drift: return drift(u, v);
        case ILLUSION_Ouchi: return ouchi(u, v);
        case ILLUSION_Bulge: return bulge(u, v);
        case ILLUSION_Moire: return moire(u, v);
        case ILLUSION_Lilac: return lilac(u, v);
        default: return I_MID;
    }
}

Rasteron_Image* illusionImgOp(ImageSize size){
    Rasteron_Image* illusionImg = RASTERON_ALLOC("illusion", size.height, size.width);
    _aspect = (double)size.width / (double)size.height;

    int samples = (S->smooth < 1)? 1 : (S->smooth > ILLUSION_SMOOTH_MAX)? ILLUSION_SMOOTH_MAX : S->smooth;
    double turn = -S->angle * (ILLUSION_TAU / 360.0);
    double turnCos = cos(turn), turnSin = sin(turn);

    for(unsigned row = 0; row < size.height; row++)
        for(unsigned col = 0; col < size.width; col++){
            unsigned sums[3] = { 0, 0, 0 };
            for(int sy = 0; sy < samples; sy++)
                for(int sx = 0; sx < samples; sx++){ // several samples per pixel smooth the edges
                    double x = ((col + ((sx + 0.5) / samples)) / size.width) - 0.5, y = ((row + ((sy + 0.5) / samples)) / size.height) - 0.5;
                    double u = x * _aspect / S->zoom, v = y / S->zoom;
                    unsigned color = illusionColor((u * turnCos) - (v * turnSin), (u * turnSin) + (v * turnCos));
                    sums[0] += (color >> 16) & 0xFF; sums[1] += (color >> 8) & 0xFF; sums[2] += color & 0xFF;
                }
            unsigned total = (unsigned)(samples * samples);
            *(illusionImg->data + ((size_t)row * size.width) + col) = 0xFF000000 | (((sums[0] + (total / 2)) / total) << 16) | (((sums[1] + (total / 2)) / total) << 8) | ((sums[2] + (total / 2)) / total);
        }

    return illusionImg;
}
