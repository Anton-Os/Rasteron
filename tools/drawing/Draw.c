#include "DrawTool.h"

// --------------------------------   Helpers    -------------------------------- //

typedef struct { double x, y, along; } Draw_Vertex; // pixel position and distance from the start of the stroke

typedef struct { Draw_Vertex* verts; unsigned count, capacity; } Draw_Outline;

typedef struct { // per pixel scratch data of the layer being drawn
    unsigned width, height;
    float* nearest; // strokes: signed distance to the stroke center, points: coverage
    float* along; // strokes: distance along the stroke, points: gradient position
    float* heading; // strokes: direction of the nearest segment in radians
    unsigned char* inside; // filled area of closed paths
} Draw_Scratch;

static double clampUnit(double v){ return (v < 0.0)? 0.0 : (v > 1.0)? 1.0 : v; }

static double smoothUnit(double v){ v = clampUnit(v); return v * v * (3.0 - (2.0 * v)); }

static double hashNoise(int x, int y, unsigned seed){ // repeatable random value from 0 to 1 for a position
    unsigned h = ((unsigned)x * 374761393u) + ((unsigned)y * 668265263u) + (seed * 2246822519u);
    h = (h ^ (h >> 13)) * 1274126177u;
    h ^= h >> 16;
    return (double)(h & 0xFFFFFF) / (double)0xFFFFFF;
}

static double valueNoise(double position, int lane, unsigned seed){ // smooth random value along one axis
    double base = floor(position);
    double t = smoothUnit(position - base);
    return (hashNoise((int)base, lane, seed) * (1.0 - t)) + (hashNoise((int)base + 1, lane, seed) * t);
}

static double edgeAlpha(double dist, double radius, double softness){ // 1 inside the radius, fading out at the edge
    double feather = (radius * clampUnit(softness)) + 1.0; // at least one pixel of antialiasing
    double alpha = clampUnit((radius + 0.5 - dist) / feather);
    return (softness > 0.0)? smoothUnit(alpha) : alpha;
}

static unsigned blendPixel(unsigned below, unsigned color, unsigned backdrop, double alpha, int blend){
    unsigned result = 0xFF000000;
    for(unsigned shift = 0; shift <= 16; shift += 8){
        double under = (double)((below >> shift) & 0xFF), over = (double)((color >> shift) & 0xFF), target;
        switch(blend){
            case BLEND_Add: target = fmin(255.0, under + over); break;
            case BLEND_Multiply: target = under * over / 255.0; break;
            case BLEND_Screen: target = 255.0 - ((255.0 - under) * (255.0 - over) / 255.0); break;
            case BLEND_Diff: target = fabs(under - over); break;
            case BLEND_Erase: target = (double)((backdrop >> shift) & 0xFF); break;
            default: target = over; break;
        }
        result |= (unsigned)(under + ((target - under) * alpha) + 0.5) << shift;
    }
    return result;
}

static unsigned gradientColor(const Draw_Style* style, double position){
    if(style->gradient == GRADIENT_None) return style->colors[0];
    return blendPixel(style->colors[0], style->colors[1], 0, clampUnit(position), BLEND_Normal);
}

const Draw_Style* drawLayerStyle(const Draw_State* state, int index){
    return (index == state->selected)? &state->pen : &state->layers[index].style;
}

// --------------------------------   Outlines    -------------------------------- //

static void outlinePush(Draw_Outline* outline, double x, double y){
    if(outline->count == outline->capacity){
        outline->capacity = (outline->capacity == 0)? 256 : outline->capacity * 2;
        outline->verts = (Draw_Vertex*)realloc(outline->verts, outline->capacity * sizeof(Draw_Vertex));
    }
    outline->verts[outline->count].x = x;
    outline->verts[outline->count].y = y;
    outline->verts[outline->count].along = 0.0;
    outline->count++;
}

static double outlineMeasure(Draw_Outline* outline){ // fills in distances along the outline and returns its length
    double length = 0.0;
    for(unsigned v = 1; v < outline->count; v++){
        length += hypot(outline->verts[v].x - outline->verts[v - 1].x, outline->verts[v].y - outline->verts[v - 1].y);
        outline->verts[v].along = length;
    }
    return length;
}

static void outlineWave(Draw_Outline* outline, double length, double amplitude, double cycles){ // pushes the outline sideways along a sine
    if(outline->count < 2 || length <= 0.0) return;
    Draw_Outline waved = { NULL, 0, 0 };
    unsigned steps = (unsigned)ceil(length / 2.0); // a vertex every two pixels
    unsigned segment = 0;

    for(unsigned s = 0; s <= steps; s++){
        double along = length * s / steps;
        while(segment + 2 < outline->count && outline->verts[segment + 1].along < along) segment++;
        const Draw_Vertex* from = &outline->verts[segment];
        const Draw_Vertex* to = &outline->verts[segment + 1];
        double span = to->along - from->along;
        double t = (span > 0.0)? (along - from->along) / span : 0.0;
        double dirX = (span > 0.0)? (to->x - from->x) / span : 1.0, dirY = (span > 0.0)? (to->y - from->y) / span : 0.0;
        double offset = amplitude * sin(DRAW_TAU * cycles * along / length);
        outlinePush(&waved, from->x + ((to->x - from->x) * t) - (dirY * offset), from->y + ((to->y - from->y) * t) + (dirX * offset));
    }
    free(outline->verts);
    *outline = waved;
}

static double layerOutline(Draw_Outline* outline, const Draw_Layer* layer, const Draw_Style* style, ImageSize size){ // connects the points, returns the length
    int count = layer->pointCount;
    int closed = (style->line == LINE_Closed || style->line == LINE_Loop) && count > 2;
    int curved = (style->line == LINE_Curve || style->line == LINE_Loop) && count > 2;
    int segments = closed? count : count - 1;
    outline->verts = NULL; outline->count = 0; outline->capacity = 0;

    if(count == 1){ // a single point becomes a dab
        outlinePush(outline, layer->points[0][0] * size.width, layer->points[0][1] * size.height);
        outlinePush(outline, layer->points[0][0] * size.width, layer->points[0][1] * size.height);
    }
    for(int s = 0; s < segments; s++){
        double px[4], py[4]; // points before, at the start, at the end, and after this segment
        for(int p = 0; p < 4; p++){
            int index = s + p - 1;
            if(closed) index = ((index % count) + count) % count;
            else index = (index < 0)? 0 : (index > count - 1)? count - 1 : index;
            px[p] = layer->points[index][0] * size.width;
            py[p] = layer->points[index][1] * size.height;
        }
        int steps = 1;
        if(curved){
            steps = (int)ceil(hypot(px[2] - px[1], py[2] - py[1]) / 4.0);
            steps = (steps < 1)? 1 : (steps > 512)? 512 : steps;
        }
        for(int k = 0; k < steps; k++){ // Catmull-Rom spline passing through every point
            double t = (double)k / steps, t2 = t * t, t3 = t2 * t;
            if(!curved) outlinePush(outline, px[1], py[1]);
            else outlinePush(outline,
                0.5 * ((2.0 * px[1]) + ((px[2] - px[0]) * t) + (((2.0 * px[0]) - (5.0 * px[1]) + (4.0 * px[2]) - px[3]) * t2) + (((3.0 * px[1]) - px[0] - (3.0 * px[2]) + px[3]) * t3)),
                0.5 * ((2.0 * py[1]) + ((py[2] - py[0]) * t) + (((2.0 * py[0]) - (5.0 * py[1]) + (4.0 * py[2]) - py[3]) * t2) + (((3.0 * py[1]) - py[0] - (3.0 * py[2]) + py[3]) * t3)));
        }
        if(s == segments - 1) outlinePush(outline, px[2], py[2]);
    }

    double length = outlineMeasure(outline);
    if(style->wave > 0.0 && length > 0.0){
        outlineWave(outline, length, style->wave * style->width * size.height, style->frequency);
        length = outlineMeasure(outline);
    }
    return length;
}

// --------------------------------   Coverage    -------------------------------- //

static double dotAlpha(const Draw_Style* style, double dx, double dy, double radius, int x, int y, unsigned seed){
    double dist = hypot(dx, dy);
    switch(style->dot){
    case DOT_Ring: return edgeAlpha(fabs(dist - (radius * 0.72)), radius * 0.28, style->softness);
    case DOT_Soft: return exp(-(dist * dist) / (radius * radius) * 3.0);
    case DOT_Square: return edgeAlpha(fmax(fabs(dx), fabs(dy)), radius, style->softness);
    case DOT_Diamond: return edgeAlpha((fabs(dx) + fabs(dy)) * 0.70710678, radius * 0.70710678, style->softness);
    case DOT_Cross: return fmax(edgeAlpha(fmax(fabs(dx), fabs(dy) * 4.0), radius, style->softness), edgeAlpha(fmax(fabs(dy), fabs(dx) * 4.0), radius, style->softness));
    case DOT_Star: {
        double sector = DRAW_TAU / 10.0; // half of one of the five arms
        double angle = fmod(fabs(atan2(dx, -dy)), sector * 2.0);
        if(angle > sector) angle = (sector * 2.0) - angle;
        double inner = 0.42;
        double edge = (inner * sin(sector)) / (sin(angle) + (inner * sin(sector - angle))); // edge distance for a radius of one
        return edgeAlpha(dist / edge, radius, style->softness);
    }
    case DOT_Half: return edgeAlpha(dist, radius, style->softness) * clampUnit(dx + 0.5);
    case DOT_Spray: return (hashNoise(x, y, seed) < exp(-(dist * dist) / (radius * radius) * 2.5) * 0.8)? 1.0 : 0.0;
    default: return edgeAlpha(dist, radius, style->softness);
    }
}

static double strokeAlpha(const Draw_Style* style, double dist, double side, double along, double length, double radius, double heading, int x, int y, unsigned seed){
    double position = (length > 0.0)? along / length : 0.0;

    if(style->kind == DRAW_Path){
        double alpha = edgeAlpha(dist, radius, style->softness);
        if(style->cut > 0.0){ // dashes
            double period = 4.0 * style->cut * radius;
            double phase = along / period;
            phase -= floor(phase);
            double toEdge = ((phase < 0.5)? fmin(phase, 0.5 - phase) : -fmin(phase - 0.5, 1.0 - phase)) * period;
            alpha *= clampUnit(toEdge + 0.5);
        }
        return alpha;
    }

    switch(style->brush){
    case BRUSH_Flat: return edgeAlpha(dist, radius * (0.2 + (0.8 * fabs(sin(heading - (DRAW_TAU / 8.0))))), style->softness); // wide across one diagonal, thin along it
    case BRUSH_Power: return edgeAlpha(dist, radius * pow(sin(DRAW_TAU * 0.5 * clampUnit(position)), 0.6), style->softness); // thin at both ends
    case BRUSH_Bulge: return edgeAlpha(dist, radius * (0.55 + (0.45 * sin(DRAW_TAU * style->frequency * position))), style->softness);
    case BRUSH_Bristle: {
        int hairs = (int)fmax(4.0, radius / 2.0);
        int hair = (int)(clampUnit((side * dist / radius * 0.5) + 0.5) * hairs);
        double ink = hashNoise(hair, 7, seed), dry = valueNoise(along * 0.02, hair, seed + 1); // every hair carries its own ink and runs dry now and then
        return edgeAlpha(dist, radius, style->softness) * (0.12 + (0.88 * clampUnit(((ink * 0.6) + (dry * 0.9) - 0.4) * 4.0)));
    }
    case BRUSH_Chalk: {
        double grain = hashNoise(x, y, seed), lump = hashNoise(x / 3, y / 3, seed + 1);
        return edgeAlpha(dist, radius * (0.8 + (0.25 * lump)), 0.2 + style->softness) * (((grain * 0.6) + (lump * 0.6) > 0.42)? 1.0 : 0.25);
    }
    case BRUSH_Spray: return (hashNoise(x, y, seed) < exp(-(dist * dist) / (radius * radius) * 2.5) * 0.75)? 1.0 : 0.0;
    case BRUSH_Stamp: {
        double spacing = fmax(2.0, (length > 0.0 && style->frequency >= 1.0)? length / floor(style->frequency) : radius * 2.4);
        double local = along - ((floor(along / spacing) + 0.5) * spacing);
        return edgeAlpha(hypot(local, dist), fmin(radius, spacing * 0.45), style->softness);
    }
    default: return edgeAlpha(dist, radius, fmax(style->softness, 0.35)); // round
    }
}

static void fillOutline(const Draw_Outline* outline, Draw_Scratch* scratch){ // even-odd scanline fill into scratch->inside
    double* crossings = (double*)malloc((outline->count + 1) * sizeof(double));
    for(unsigned y = 0; y < scratch->height; y++){
        double lineY = y + 0.5;
        unsigned found = 0;
        for(unsigned v = 0; v < outline->count; v++){
            const Draw_Vertex* from = &outline->verts[v];
            const Draw_Vertex* to = &outline->verts[(v + 1) % outline->count];
            if((from->y <= lineY) != (to->y <= lineY)) crossings[found++] = from->x + ((lineY - from->y) * (to->x - from->x) / (to->y - from->y));
        }
        for(unsigned a = 1; a < found; a++){ // insertion sort, rows have few crossings
            double value = crossings[a];
            unsigned b = a;
            for(; b > 0 && crossings[b - 1] > value; b--) crossings[b] = crossings[b - 1];
            crossings[b] = value;
        }
        for(unsigned c = 0; c + 1 < found; c += 2){
            double first = ceil(crossings[c] - 0.5), last = floor(crossings[c + 1] - 0.5);
            if(first < 0.0) first = 0.0;
            if(last > scratch->width - 1.0) last = scratch->width - 1.0;
            for(double x = first; x <= last; x += 1.0) scratch->inside[((size_t)y * scratch->width) + (size_t)x] = 1;
        }
    }
    free(crossings);
}

// --------------------------------   Layers    -------------------------------- //

static void drawPoints(Rasteron_Image* canvasImg, ref_image_t backdropImg, const Draw_Layer* layer, const Draw_Style* style, double shown, unsigned seed, Draw_Scratch* scratch){
    unsigned width = canvasImg->width, height = canvasImg->height;
    double radius = fmax(0.5, style->width * height * 0.5);
    double pad = (radius * 1.6) + 2.0;
    int visible = (int)ceil(shown * layer->pointCount);

    for(size_t p = 0; p < (size_t)width * height; p++) scratch->nearest[p] = 0.0f;
    for(int i = 0; i < visible; i++){ // dots of one layer merge before they are blended
        double centerX = layer->points[i][0] * width, centerY = layer->points[i][1] * height;
        long minX = (long)floor(centerX - pad), maxX = (long)ceil(centerX + pad), minY = (long)floor(centerY - pad), maxY = (long)ceil(centerY + pad);
        if(minX < 0) minX = 0;
        if(minY < 0) minY = 0;
        if(maxX > (long)width - 1) maxX = (long)width - 1;
        if(maxY > (long)height - 1) maxY = (long)height - 1;

        for(long y = minY; y <= maxY; y++)
            for(long x = minX; x <= maxX; x++){
                double dx = x + 0.5 - centerX, dy = y + 0.5 - centerY;
                double alpha = dotAlpha(style, dx, dy, radius, (int)x, (int)y, seed + (unsigned)i);
                size_t p = ((size_t)y * width) + (size_t)x;
                if(alpha > scratch->nearest[p]){
                    scratch->nearest[p] = (float)alpha;
                    scratch->along[p] = (float)((style->gradient == GRADIENT_Across)? hypot(dx, dy) / radius : (layer->pointCount > 1)? (double)i / (layer->pointCount - 1) : 0.0);
                }
            }
    }

    for(size_t p = 0; p < (size_t)width * height; p++)
        if(scratch->nearest[p] > 0.0f)
            *(canvasImg->data + p) = blendPixel(*(canvasImg->data + p), gradientColor(style, scratch->along[p]), *(backdropImg->data + p), scratch->nearest[p] * style->opacity, style->blend);
}

static void drawStroke(Rasteron_Image* canvasImg, ref_image_t backdropImg, const Draw_Layer* layer, const Draw_Style* style, double shown, unsigned seed, Draw_Scratch* scratch){
    unsigned width = canvasImg->width, height = canvasImg->height;
    ImageSize size = { height, width };
    Draw_Outline outline;
    double length = layerOutline(&outline, layer, style, size);
    double radius = fmax(0.5, style->width * height * 0.5);
    double pad = (radius * 1.6) + 2.0; // spray and soft edges reach past the radius
    double limit = (shown >= 1.0)? length + 1.0 : length * shown; // strokes are drawn up to this distance
    int filled = style->kind == DRAW_Path && style->fill && shown >= 1.0 && outline.count > 3 && (style->line == LINE_Closed || style->line == LINE_Loop);

    for(size_t p = 0; p < (size_t)width * height; p++) scratch->nearest[p] = 1.0e9f;
    if(filled){
        memset(scratch->inside, 0, (size_t)width * height);
        fillOutline(&outline, scratch);
    }

    for(unsigned v = 0; v + 1 < outline.count; v++){ // every pixel remembers its nearest piece of the stroke
        const Draw_Vertex* from = &outline.verts[v];
        const Draw_Vertex* to = &outline.verts[v + 1];
        if(from->along > limit) break;
        double dx = to->x - from->x, dy = to->y - from->y;
        double span = hypot(dx, dy);
        float heading = (float)atan2(dy, dx);

        long minX = (long)floor(fmin(from->x, to->x) - pad), maxX = (long)ceil(fmax(from->x, to->x) + pad);
        long minY = (long)floor(fmin(from->y, to->y) - pad), maxY = (long)ceil(fmax(from->y, to->y) + pad);
        if(minX < 0) minX = 0;
        if(minY < 0) minY = 0;
        if(maxX > (long)width - 1) maxX = (long)width - 1;
        if(maxY > (long)height - 1) maxY = (long)height - 1;

        for(long y = minY; y <= maxY; y++)
            for(long x = minX; x <= maxX; x++){
                double px = x + 0.5 - from->x, py = y + 0.5 - from->y;
                double t = (span > 0.0)? clampUnit(((px * dx) + (py * dy)) / (span * span)) : 0.0;
                double dist = hypot(px - (t * dx), py - (t * dy));
                size_t p = ((size_t)y * width) + (size_t)x;
                if(dist < fabs(scratch->nearest[p])){
                    scratch->nearest[p] = (float)((((dx * py) - (dy * px)) < 0.0)? -dist : dist); // sign tells the side of the stroke
                    scratch->along[p] = (float)(from->along + (t * span));
                    scratch->heading[p] = heading;
                }
            }
    }

    for(unsigned y = 0; y < height; y++)
        for(unsigned x = 0; x < width; x++){
            size_t p = ((size_t)y * width) + x;
            double dist = fabs(scratch->nearest[p]);
            double alpha = 0.0, position = 0.0;
            if(dist <= pad && scratch->along[p] <= limit){
                alpha = strokeAlpha(style, dist, (scratch->nearest[p] < 0.0f)? -1.0 : 1.0, scratch->along[p], length, radius, scratch->heading[p], (int)x, (int)y, seed);
                position = (style->gradient == GRADIENT_Across)? dist / radius : (length > 0.0)? scratch->along[p] / length : 0.0;
            }
            if(filled){ // filled shapes shade from top to bottom, or from the outline inward
                if(style->gradient != GRADIENT_Across) position = (double)y / height;
                if(scratch->inside[p]){
                    if(style->gradient == GRADIENT_Across && alpha < 1.0) position = 1.0;
                    alpha = fmax(alpha, 1.0);
                }
            }
            if(alpha > 0.0) *(canvasImg->data + p) = blendPixel(*(canvasImg->data + p), gradientColor(style, position), *(backdropImg->data + p), alpha * style->opacity, style->blend);
        }

    free(outline.verts);
}

static Draw_Scratch* _scratch = NULL; // set while drawImgOp runs

void drawLayer(Rasteron_Image* canvasImg, ref_image_t backdropImg, const Draw_Layer* layer, const Draw_Style* style, double shown, unsigned seed){
    if(canvasImg == NULL || layer->pointCount < 1 || shown <= 0.0 || style->opacity <= 0.0) return;

    Draw_Scratch local = { canvasImg->width, canvasImg->height, NULL, NULL, NULL, NULL };
    Draw_Scratch* scratch = _scratch;
    if(scratch == NULL || scratch->width != canvasImg->width || scratch->height != canvasImg->height){ // called on its own
        size_t pixels = (size_t)canvasImg->width * canvasImg->height;
        local.nearest = (float*)malloc(pixels * sizeof(float));
        local.along = (float*)malloc(pixels * sizeof(float));
        local.heading = (float*)malloc(pixels * sizeof(float));
        local.inside = (unsigned char*)malloc(pixels);
        scratch = &local;
    }

    if(scratch->nearest != NULL && scratch->along != NULL && scratch->heading != NULL && scratch->inside != NULL){
        if(style->kind == DRAW_Points) drawPoints(canvasImg, backdropImg, layer, style, shown, seed, scratch);
        else drawStroke(canvasImg, backdropImg, layer, style, shown, seed, scratch);
    }
    if(scratch == &local){ free(local.nearest); free(local.along); free(local.heading); free(local.inside); }
}

// --------------------------------   Image Operations    -------------------------------- //

Rasteron_Image* backdropImgOp(ImageSize size, const Draw_State* state, ref_image_t baseImg){
    if(baseImg != NULL) // applied image carries the drawing so far
        return (baseImg->width == size.width && baseImg->height == size.height)? copyImgOp(baseImg) : resizeImgOp(size, baseImg);

    switch(state->backdrop){
        case BACKDROP_Checker: return checkeredImgOp(size, (ColorGrid){ (unsigned)state->grid, (unsigned)state->grid, state->canvas[0], state->canvas[1] });
        case BACKDROP_Gradient: return gradientImgOp(size, SIDE_Top, state->canvas[0] | 0xFF000000, state->canvas[1] | 0xFF000000);
        default: return solidImgOp(size, state->canvas[0]);
    }
}

Rasteron_Image* drawImgOp(ImageSize size, const Draw_State* state, ref_image_t baseImg){
    Rasteron_Image* backdropImg = backdropImgOp(size, state, baseImg);
    for(size_t p = 0; p < (size_t)size.width * size.height; p++) *(backdropImg->data + p) |= 0xFF000000; // drawing needs an opaque canvas
    Rasteron_Image* drawImg = copyImgOp(backdropImg);

    size_t pixels = (size_t)size.width * size.height;
    Draw_Scratch scratch = { size.width, size.height, (float*)malloc(pixels * sizeof(float)), (float*)malloc(pixels * sizeof(float)), (float*)malloc(pixels * sizeof(float)), (unsigned char*)malloc(pixels) };
    _scratch = &scratch;

    int visibleCount = 0, drawn = 0;
    for(int l = 0; l < state->layerCount; l++) if(state->layers[l].visible) visibleCount++;
    for(int l = 0; l < state->layerCount; l++){
        if(!state->layers[l].visible) continue;
        double shown = clampUnit((state->progress * visibleCount) - drawn); // layers appear one after another as progress grows
        drawLayer(drawImg, backdropImg, &state->layers[l], drawLayerStyle(state, l), shown, (unsigned)_toolCommon.seed + ((unsigned)l * 7919u));
        drawn++;
    }

    _scratch = NULL;
    free(scratch.nearest); free(scratch.along); free(scratch.heading); free(scratch.inside);
    RASTERON_DEALLOC(backdropImg);
    return drawImg;
}
