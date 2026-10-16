#include "TileTool.h"

#define TILE_TAU 6.28318530717958647692

// --------------------------------   TileTest Callbacks    -------------------------------- //

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
    else return fuse_colors(colors[0], colors[1], angle + (dists[0] / fabs(yArg)));
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

// --------------------------------   TileTest Mosaic    -------------------------------- //

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

// --------------------------------   TileTest Truschet    -------------------------------- //

unsigned sharpTruschetTile(double x, double y){
    return blend_colors(_tile.colors[0], _tile.colors[1], asin(pow(x, y)) + acos(pow(y, x)));
}

Rasteron_Image* truschetImgOp(ImageSize size, ref_image_t truschetImg, unsigned short wDiv, unsigned short hDiv){
    Rasteron_Image* truschetTile = (truschetImg == NULL)? mapImgOp((ImageSize){ size.height / hDiv, size.width / wDiv }, sharpTruschetTile) : copyImgOp(truschetImg);
    Rasteron_Image* truschetTile2 = flipImgOp(truschetTile, FLIP_Upside);
    Rasteron_Image* truschetTile3 = flipImgOp(truschetTile, FLIP_Clock);
    Rasteron_Image* truschetTile4 = flipImgOp(truschetTile, FLIP_Counter);

    Rasteron_Image* finalImg = RASTERON_ALLOC("truschet", size.height, size.width);
    for(unsigned p = 0; p < size.width * size.height; p++){
        double x = (1.0 / (double)size.width) * (p % size.width);
        double y = (1.0 / (double)size.height) * (p / size.width);

        unsigned c = (unsigned)(x * wDiv);
        unsigned r = (unsigned)(y * hDiv);

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

// --------------------------------   Points & Palettes    -------------------------------- //

static double wrap01(double v){ return v - floor(v); }

void tilePoints(PixelPointTable* table){
    const Tile_State* s = &_tile;
    unsigned short w = (unsigned short)s->reps[0], h = (unsigned short)s->reps[1];
    table->pointCount = 0;

    switch(s->points){
    case POINTS_Square: pixPoints_tiling(table, TILE_Square, w, h); break;
    case POINTS_Triangle: pixPoints_tiling(table, TILE_Triangle, w, h); break;
    case POINTS_Hexagon: pixPoints_tiling(table, TILE_Hexagon, w, h); break;
    case POINTS_Diagonal: pixPoints_tiling(table, TILE_Diagonal, w, h); break;
    case POINTS_Perimeter: pixPoints_tiling(table, TILE_Perimiter, w, h); break;
    case POINTS_Random:
        for(int p = 0; p < s->count; p++) pixelPointToTable(table, (double)rand() / RAND_MAX, (double)rand() / RAND_MAX);
        break;
    case POINTS_Spiral: // phyllotaxis with the golden angle
        for(int p = 0; p < s->count; p++){
            double r = 0.5 * sqrt((p + 0.5) / (double)s->count) * 1.35;
            double a = p * 2.39996322972865332;
            pixelPointToTable(table, 0.5 + r * cos(a), 0.5 + r * sin(a));
        }
        break;
    case POINTS_Circle: { // concentric rings, reps[1] rings with count points on the outer one
        pixelPointToTable(table, 0.5, 0.5);
        int rings = (h > 0)? h : 1;
        for(int ring = 1; ring <= rings; ring++){
            int ringPoints = (s->count * ring) / rings;
            if(ringPoints < 3) ringPoints = 3;
            double r = 0.48 * ring / rings;
            for(int p = 0; p < ringPoints; p++){
                double a = TILE_TAU * p / ringPoints + ring * 0.3;
                pixelPointToTable(table, 0.5 + r * cos(a), 0.5 + r * sin(a));
            }
        }
    } break;
    }

    if(s->expand > 0) pixPoionts_expand(table, (unsigned short)s->expand);

    for(unsigned p = 0; p < table->pointCount; p++){
        if(s->jitter > 0.0){
            double cellX = 1.0 / ((w > 0)? w : 1), cellY = 1.0 / ((h > 0)? h : 1);
            table->points[p].x += (((double)rand() / RAND_MAX) - 0.5) * s->jitter * cellX;
            table->points[p].y += (((double)rand() / RAND_MAX) - 0.5) * s->jitter * cellY;
        }
        if(s->phase != 0.0){ // small orbit per point, one turn of phase loops
            double offset = p * 2.17;
            table->points[p].x += 0.025 * cos(s->phase + offset);
            table->points[p].y += 0.025 * sin(s->phase + offset * 1.3);
        }
    }
}

void tileColors(ColorPointTable* colorTable, const PixelPointTable* table){
    const Tile_State* s = &_tile;
    ColorSwatch swatch = createSwatch(s->colors[0], 0x40);
    colorTable->pointCount = table->pointCount;

    for(unsigned p = 0; p < table->pointCount; p++){
        double x = table->points[p].x, y = table->points[p].y;
        unsigned color;
        switch(s->palette){
            case PALETTE_Gradient: color = blend_colors(s->colors[0], s->colors[1], (table->pointCount > 1)? (double)p / (table->pointCount - 1) : 0.0); break;
            case PALETTE_Two: color = (p % 2 == 0)? s->colors[0] : s->colors[1]; break;
            case PALETTE_Swatch: color = swatch.colors[p % 8]; break;
            case PALETTE_Position: color = blend_colors(blend_colors(s->colors[0], s->colors[1], wrap01(x)), s->colors[2], wrap01(y) * 0.6); break;
            default: color = RAND_COLOR(); break;
        }
        colorTable->points[p] = (ColorPoint){ color | 0xFF000000, x, y };
    }
}

double tileDistance(double dx, double dy){
    dx = fabs(dx); dy = fabs(dy);
    switch(_tile.distance){
        case DIST_Manhattan: return dx + dy;
        case DIST_Chebyshev: return (dx > dy)? dx : dy;
        case DIST_Minkowski: return pow(pow(dx, _tile.exponent) + pow(dy, _tile.exponent), 1.0 / _tile.exponent);
        case DIST_Octagonal: { double m = (dx > dy)? dx : dy; double sum = (dx + dy) * 0.70710678; return (m > sum)? m : sum; }
        case DIST_Hexagonal: { double a = dx * 0.8660254 + dy * 0.5; double m = (a > dy)? a : dy; return m; }
        default: return sqrt(dx * dx + dy * dy);
    }
}

// --------------------------------   Field & Voronoi    -------------------------------- //

typedef struct { double dist; unsigned index; } Tile_Nearest;

static void nearestThree(double x, double y, const ColorPointTable* table, Tile_Nearest best[3]){
    for(unsigned b = 0; b < 3; b++){ best[b].dist = 1e9; best[b].index = 0; }
    for(unsigned t = 0; t < table->pointCount; t++){
        double d = tileDistance(x - table->points[t].x, y - table->points[t].y);
        if(d >= best[2].dist) continue;
        unsigned slot = 2;
        while(slot > 0 && d < best[slot - 1].dist){ best[slot] = best[slot - 1]; slot--; }
        best[slot].dist = d;
        best[slot].index = t;
    }
}

Rasteron_Image* tileFieldImgOp(ImageSize size, const ColorPointTable* colorPointTable, fieldCallback3 callback){
    Rasteron_Image* fieldImg = RASTERON_ALLOC("tile_field", size.height, size.width);
    Tile_Nearest best[3];
    unsigned colors[3];
    double dists[3];
    PixelPoint pixPoints[3];

    for(unsigned p = 0; p < size.width * size.height; p++){
        double x = (double)(p % size.width) / size.width;
        double y = (double)(p / size.width) / size.height;
        nearestThree(x, y, colorPointTable, best);
        for(unsigned b = 0; b < 3; b++){
            unsigned i = (b < colorPointTable->pointCount)? best[b].index : best[0].index;
            colors[b] = colorPointTable->points[i].color;
            dists[b] = (best[b].dist < 1.0)? best[b].dist : 1.0;
            pixPoints[b] = (PixelPoint){ x - colorPointTable->points[i].x, y - colorPointTable->points[i].y };
        }
        *(fieldImg->data + p) = callback(colors, dists, pixPoints) | 0xFF000000;
    }
    return fieldImg;
}

Rasteron_Image* tileVoronoiImgOp(ImageSize size, const ColorPointTable* colorPointTable){
    Rasteron_Image* voronoiImg = RASTERON_ALLOC("tile_voronoi", size.height, size.width);
    Tile_Nearest best[3];
    double cellSize = 1.0 / sqrt((double)((colorPointTable->pointCount > 0)? colorPointTable->pointCount : 1));

    for(unsigned p = 0; p < size.width * size.height; p++){
        double x = (double)(p % size.width) / size.width;
        double y = (double)(p / size.width) / size.height;
        nearestThree(x, y, colorPointTable, best);

        unsigned color = colorPointTable->points[best[0].index].color;
        double gap = (colorPointTable->pointCount > 1)? best[1].dist - best[0].dist : 1.0; // zero along cell borders
        double edge = _tile.edge * cellSize;

        switch(_tile.shading){
            case SHADE_Shaded: color = blend_colors(color, 0xFF000000, fmin(1.0, best[0].dist / cellSize)); break;
            case SHADE_Edges: color = (gap < edge)? _tile.colors[2] : _tile.colors[1]; break;
            case SHADE_Stained: if(gap < edge) color = _tile.colors[2]; break;
            case SHADE_Ridges: color = blend_colors(_tile.colors[2], _tile.colors[1], fmin(1.0, gap / (cellSize * 0.5))); break;
            default: break;
        }
        *(voronoiImg->data + p) = color | 0xFF000000;
    }
    return voronoiImg;
}

// --------------------------------   Truchet    -------------------------------- //

Rasteron_Image* truchetRandomImgOp(ImageSize size, int style, unsigned short wDiv, unsigned short hDiv){
    Rasteron_Image* truchetImg = RASTERON_ALLOC("truchet", size.height, size.width);
    unsigned cellCount = (unsigned)wDiv * hDiv;
    unsigned char* flips = (unsigned char*)malloc(cellCount);
    for(unsigned c = 0; c < cellCount; c++) flips[c] = (unsigned char)(rand() % 2);

    double width = _tile.edge * 0.5; // line half width relative to a cell
    for(unsigned p = 0; p < size.width * size.height; p++){
        double x = (double)(p % size.width) / size.width * wDiv;
        double y = (double)(p / size.width) / size.height * hDiv;
        unsigned cx = (unsigned)x, cy = (unsigned)y;
        if(cx >= wDiv) cx = wDiv - 1;
        if(cy >= hDiv) cy = hDiv - 1;
        double u = x - cx, v = y - cy;
        if(flips[cy * wDiv + cx]) u = 1.0 - u; // mirrored tile

        unsigned color = _tile.colors[0];
        if(style == TRUCHET_Arcs){
            double d1 = fabs(sqrt(u * u + v * v) - 0.5);
            double d2 = fabs(sqrt((1.0 - u) * (1.0 - u) + (1.0 - v) * (1.0 - v)) - 0.5);
            double d = (d1 < d2)? d1 : d2;
            if(d < width) color = blend_colors(_tile.colors[1], _tile.colors[2], d / width);
        }
        else if(style == TRUCHET_Diagonals){
            double d = fabs(u - v) * 0.70710678;
            if(d < width) color = _tile.colors[1];
        }
        else color = (u > v)? _tile.colors[1] : _tile.colors[0]; // triangles
        *(truchetImg->data + p) = color | 0xFF000000;
    }
    free(flips);
    return truchetImg;
}

// --------------------------------   Penrose    -------------------------------- //

typedef struct { int kind; double ax, ay, bx, by, cx, cy; } Tile_Robinson; // robinson triangle, kind 0 thin, 1 thick

static double edgeDist(double px, double py, double ax, double ay, double bx, double by){
    double vx = bx - ax, vy = by - ay;
    double t = ((px - ax) * vx + (py - ay) * vy) / (vx * vx + vy * vy);
    t = (t < 0.0)? 0.0 : (t > 1.0)? 1.0 : t;
    double dx = px - (ax + t * vx), dy = py - (ay + t * vy);
    return sqrt(dx * dx + dy * dy);
}

Rasteron_Image* penroseImgOp(ImageSize size, unsigned short subdivisions){
    const double golden = (1.0 + sqrt(5.0)) / 2.0;
    unsigned capacity = 10;
    for(unsigned short s = 0; s < subdivisions; s++) capacity *= 3;
    Tile_Robinson* tris = (Tile_Robinson*)malloc(sizeof(Tile_Robinson) * capacity);
    Tile_Robinson* next = (Tile_Robinson*)malloc(sizeof(Tile_Robinson) * capacity);
    unsigned count = 0;

    double rotation = _tile.phase / 5.0; // a full phase turn rotates by 72 degrees, matching the 5-fold symmetry
    for(int i = 0; i < 10; i++){ // starting wheel
        double a1 = (2 * i - 1) * 3.14159265358979 / 10.0 + rotation, a2 = (2 * i + 1) * 3.14159265358979 / 10.0 + rotation;
        Tile_Robinson t = { 0, 0.0, 0.0, cos(a1), sin(a1), cos(a2), sin(a2) };
        if(i % 2 == 0){ double tx = t.bx, ty = t.by; t.bx = t.cx; t.by = t.cy; t.cx = tx; t.cy = ty; }
        tris[count++] = t;
    }

    for(unsigned short s = 0; s < subdivisions; s++){
        unsigned nextCount = 0;
        for(unsigned i = 0; i < count; i++){
            Tile_Robinson t = tris[i];
            if(t.kind == 0){
                double px = t.ax + (t.bx - t.ax) / golden, py = t.ay + (t.by - t.ay) / golden;
                next[nextCount++] = (Tile_Robinson){ 0, t.cx, t.cy, px, py, t.bx, t.by };
                next[nextCount++] = (Tile_Robinson){ 1, px, py, t.cx, t.cy, t.ax, t.ay };
            } else {
                double qx = t.bx + (t.ax - t.bx) / golden, qy = t.by + (t.ay - t.by) / golden;
                double rx = t.bx + (t.cx - t.bx) / golden, ry = t.by + (t.cy - t.by) / golden;
                next[nextCount++] = (Tile_Robinson){ 1, rx, ry, t.cx, t.cy, t.ax, t.ay };
                next[nextCount++] = (Tile_Robinson){ 1, qx, qy, rx, ry, t.bx, t.by };
                next[nextCount++] = (Tile_Robinson){ 0, rx, ry, qx, qy, t.ax, t.ay };
            }
        }
        Tile_Robinson* swap = tris; tris = next; next = swap;
        count = nextCount;
    }

    Rasteron_Image* penroseImg = solidImgOp(size, _tile.colors[2]);
    double scale = 0.75 * ((size.width > size.height)? size.width : size.height) * _tile.zoom; // wheel covers the corners
    double ox = size.width * 0.5, oy = size.height * 0.5;
    double lineWidth = _tile.edge * scale * pow(1.0 / golden, subdivisions) * 0.25;

    for(unsigned i = 0; i < count; i++){
        Tile_Robinson t = tris[i];
        double ax = ox + t.ax * scale, ay = oy + t.ay * scale;
        double bx = ox + t.bx * scale, by = oy + t.by * scale;
        double cx = ox + t.cx * scale, cy = oy + t.cy * scale;

        unsigned fill;
        if(_tile.palette == PALETTE_Random || _tile.palette == PALETTE_Swatch){ // shared color for both halves of a rhombus
            double mx = (t.bx + t.cx) * 0.5, my = (t.by + t.cy) * 0.5;
            unsigned hash = (unsigned)(fabs(sin(mx * 12.9898 + my * 78.233)) * 43758.5453 * 1000.0);
            ColorSwatch swatch = createSwatch((t.kind == 0)? _tile.colors[0] : _tile.colors[1], 0x30);
            fill = swatch.colors[hash % 8];
        }
        else if(_tile.palette == PALETTE_Position) fill = blend_colors(_tile.colors[0], _tile.colors[1], fmin(1.0, sqrt(t.ax * t.ax + t.ay * t.ay)));
        else fill = (t.kind == 0)? _tile.colors[0] : _tile.colors[1];

        int minX = (int)floor(fmin(ax, fmin(bx, cx))), maxX = (int)ceil(fmax(ax, fmax(bx, cx)));
        int minY = (int)floor(fmin(ay, fmin(by, cy))), maxY = (int)ceil(fmax(ay, fmax(by, cy)));
        if(maxX < 0 || maxY < 0 || minX >= (int)size.width || minY >= (int)size.height) continue;
        if(minX < 0) minX = 0;
        if(minY < 0) minY = 0;
        if(maxX >= (int)size.width) maxX = size.width - 1;
        if(maxY >= (int)size.height) maxY = size.height - 1;

        double area = (bx - ax) * (cy - ay) - (cx - ax) * (by - ay);
        if(fabs(area) < 1e-9) continue;
        for(int y = minY; y <= maxY; y++)
            for(int x = minX; x <= maxX; x++){
                double px = x + 0.5, py = y + 0.5;
                double w0 = ((bx - px) * (cy - py) - (cx - px) * (by - py)) / area;
                double w1 = ((cx - px) * (ay - py) - (ax - px) * (cy - py)) / area;
                double w2 = 1.0 - w0 - w1;
                if(w0 < 0.0 || w1 < 0.0 || w2 < 0.0) continue;
                // outline the rhombus edges (a to b and a to c), the shared base b to c stays open
                double d = fmin(edgeDist(px, py, ax, ay, bx, by), edgeDist(px, py, ax, ay, cx, cy));
                *(penroseImg->data + y * size.width + x) = ((d < lineWidth)? _tile.colors[2] : fill) | 0xFF000000;
            }
    }

    free(tris);
    free(next);
    return penroseImg;
}
