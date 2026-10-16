#include "SimTool.h"

// --------------------------------   SimTest Rules    -------------------------------- //

unsigned shuffleRules(unsigned cell, unsigned nerbs[8]) {
    static unsigned r = 0;
    r++;
    for (unsigned n = 0; n < 8; n++) if(nerbs[n] == cell && nerbs[n] != NO_COLOR) return nerbs[r % 8];
    else return cell;
}

unsigned conwayRules(unsigned cell, unsigned nerbs[8]) {
    unsigned short lives = neighbor_count(SWATCH_GA, nerbs) + neighbor_count(SWATCH_L, nerbs); // countLives(nerbs);
    unsigned short kills = neighbor_count(SWATCH_RA, nerbs) + neighbor_count(SWATCH_D, nerbs); // countKills(nerbs);
    // printf("Lives is %d, %d", lives, kills);

    if ((cell == SWATCH_GA || cell == SWATCH_L) && lives < 2) return SWATCH_RA; // Any live cell with fewer than two live nerbs dies, as if by underpopulation.
    else if ((cell == SWATCH_GA || cell == SWATCH_L) && (lives == 2 || lives == 3)) return SWATCH_GA; // Any live cell with two or three live nerbs lives on to the next generation.
    else if ((cell == SWATCH_GA || cell == SWATCH_L) && lives > 3) return SWATCH_RA; // Any live cell with more than three live nerbs dies, as if by overpopulation.
    else if ((cell == SWATCH_RA || cell == SWATCH_D) && lives == 3) return SWATCH_L; // Any kills cell with exactly three live nerbs becomes a live cell, as if by reproduction.
    else if (cell == SWATCH_RA) return SWATCH_D;
    else return cell;
}

unsigned randWalkRules(unsigned cell, unsigned nerbs[8]) {
    unsigned short direction = 0;

    if (cell == SWATCH_L) return SWATCH_GA;
    else if (cell == SWATCH_RA) return SWATCH_D;
    else if (nerbs[direction] == SWATCH_L || nerbs[direction] == SWATCH_GA) {
        direction = rand() % 8; // randomize walk direction
        return SWATCH_L;
    }
    else if (nerbs[direction] == SWATCH_D || nerbs[direction] == SWATCH_RA) {
        direction = rand() % 8; // randomize walk direction
        return SWATCH_RA;
    }
    else return cell;
}

unsigned amplifyRules(unsigned cell, unsigned nerbs[8]) {
    unsigned short lives = neighbor_count(SWATCH_GA, nerbs) + neighbor_count(SWATCH_L, nerbs); // countLives(nerbs);
    unsigned short kills = neighbor_count(SWATCH_RA, nerbs) + neighbor_count(SWATCH_D, nerbs); // countKills(nerbs);
    // printf("Lives is %d, %d", lives, kills);

    if (lives > kills) return (cell == SWATCH_GA || cell == SWATCH_L) ? SWATCH_GA : SWATCH_L; // color_level(cell, 0.5 + (lives * (0.5 / 8.0)));
    else if (kills > lives) return (cell == SWATCH_RA || cell == SWATCH_D) ? SWATCH_D : SWATCH_RA; // color_level(cell, 0.5 - (lives * (0.5 / 8.0)));
    else return cell;
}

unsigned recursiveRules(unsigned cell, unsigned nerbs[8]) {
    cell += (nerbs[NEBR_Left] > nerbs[NEBR_Right]) ? 16 : -16;
    cell += (nerbs[NEBR_Top] > nerbs[NEBR_Bot]) ? 16 : -16;
    cell += (nerbs[NEBR_Top_Left] + nerbs[NEBR_Top_Right] > nerbs[NEBR_Bot_Left] + nerbs[NEBR_Bot_Right]) ? 32 : -32;
    return cell;
}

unsigned bloomRules(unsigned cell, unsigned nerbs[8]) {
    if (nerbs[NEBR_Bot] == SWATCH_GA || nerbs[NEBR_Top] == SWATCH_GA || nerbs[NEBR_Left] == SWATCH_GA || nerbs[NEBR_Right] == SWATCH_GA)
        return SWATCH_RA;
    else if (nerbs[NEBR_Bot_Left] == SWATCH_RA || nerbs[NEBR_Top_Right] == SWATCH_RA || nerbs[NEBR_Top_Left] == SWATCH_RA || nerbs[NEBR_Bot_Right] == SWATCH_RA)
        return SWATCH_GA;
    else return cell;
}

unsigned unbloomRules(unsigned cell, unsigned nerbs[8]) {
    if (nerbs[NEBR_Bot_Left] == SWATCH_RA || nerbs[NEBR_Top_Right] == SWATCH_RA || nerbs[NEBR_Top_Left] == SWATCH_RA || nerbs[NEBR_Bot_Right] == SWATCH_RA)
        return SWATCH_GA;
    else if (nerbs[NEBR_Bot] == SWATCH_GA || nerbs[NEBR_Top] == SWATCH_GA || nerbs[NEBR_Left] == SWATCH_GA || nerbs[NEBR_Right] == SWATCH_GA)
        return SWATCH_RA;
    else return cell;
}

unsigned levelRules(unsigned cell, unsigned nerbs[8]) {
    unsigned short lives = neighbor_count(SWATCH_GA, nerbs) + neighbor_count(SWATCH_L, nerbs); // countLives(nerbs);
    unsigned short kills = neighbor_count(SWATCH_RA, nerbs) + neighbor_count(SWATCH_D, nerbs);
    unsigned short diff = abs((short)lives - (short)kills);

    if (diff == 0) return cell;
    else if (diff % 2 == 1) return (diff == 1) ? SWATCH_RA : SWATCH_L;
    else return (diff == 2) ? SWATCH_GA : SWATCH_D;
}

unsigned matchRules(unsigned cell, unsigned nerbs[8]) {
    if (nerbs[NEBR_Left] == nerbs[NEBR_Right]) return nerbs[NEBR_Left];
    else if (nerbs[NEBR_Top] == nerbs[NEBR_Bot]) return nerbs[NEBR_Top];
    else if (nerbs[NEBR_Top_Left] == nerbs[NEBR_Bot_Right]) return nerbs[NEBR_Bot_Right];
    else if (nerbs[NEBR_Bot_Left] == nerbs[NEBR_Top_Right]) return nerbs[NEBR_Top_Right];
    else return nerbs[rand() % 8];
}

unsigned colorizeRules(unsigned cell, unsigned nerbs[8]) {
    if (cell == SWATCH_L || cell == SWATCH_GA) return blend_colors(cell, SWATCH_GA, (float)rand() / RAND_MAX);
    else if (cell == SWATCH_D || cell == SWATCH_RA) return blend_colors(cell, SWATCH_RA, (float)rand() / RAND_MAX);
    else return cell;
}

unsigned scatterRules(unsigned cell, unsigned nerbs[8]) {
    if (nerbs[NEBR_Left] == nerbs[NEBR_Right] && nerbs[NEBR_Top] == nerbs[NEBR_Bot]) return _swatch.colors[rand() % 8];
    else return cell;
}

unsigned flipRules(unsigned cell, unsigned nerbs[8]) {
    if (cell == SWATCH_GA || cell == SWATCH_L) {
        unsigned lifeCount = 0;
        for (unsigned d = 0; d < 8; d++)
            if (nerbs[d] == SWATCH_GA || nerbs[d] == SWATCH_L) lifeCount++;
        return (lifeCount % 2 == 0) ? SWATCH_RA : SWATCH_D;
    }
    else if (cell == SWATCH_RA || cell == SWATCH_D) {
        unsigned deadCount = 0;
        for (unsigned d = 0; d < 8; d++)
            if (nerbs[d] == SWATCH_RA || nerbs[d] == SWATCH_D) deadCount++;
        return (deadCount % 2 == 0) ? SWATCH_GA : SWATCH_L;
    }
    return cell;
}

unsigned wolframRules(unsigned cell, unsigned nerbs[8]) {
    unsigned short lives = neighbor_count(SWATCH_GA, nerbs) + neighbor_count(SWATCH_L, nerbs); // countLives(nerbs);
    unsigned short kills = neighbor_count(SWATCH_RA, nerbs) + neighbor_count(SWATCH_D, nerbs);

    if (lives == 3 || lives == 5) return SWATCH_GA;
    if (kills == 2 || kills == 6) return SWATCH_RA;
    else return NO_COLOR;
}

// --------------------------------   SimTest Growth    -------------------------------- //

Rasteron_Image* growImgOp(ImageSize size, double balance, double exFactor){
    Rasteron_Image* growthImg = solidImgOp(size, _swatch.base);
    
    for(unsigned p = 0; p < growthImg->width * growthImg->height; p++){
        double prob = rand() / (double)RAND_MAX;

        unsigned color = _swatch.base;
        if(prob > (1.0 - exFactor) * balance) color = SWATCH_L;
        else if(prob < exFactor * (1.0 / balance)) color = SWATCH_D;
        else if(prob >= 0.5 * balance) color = SWATCH_GA;
        else if(prob <= 0.5 * (1.0 / balance)) color = SWATCH_RA;
        else color = _swatch.base;

        *(growthImg->data + p) = color;
    }
    
    return growthImg;
}


// --------------------------------   Seeds    -------------------------------- //

typedef struct { int x, y; } Sim_Cell;

static const Sim_Cell _glider[] = { {1,0}, {2,1}, {0,2}, {1,2}, {2,2} };
static const Sim_Cell _rpentomino[] = { {1,0}, {2,0}, {0,1}, {1,1}, {1,2} };
static const Sim_Cell _acorn[] = { {1,0}, {3,1}, {0,2}, {1,2}, {4,2}, {5,2}, {6,2} };
static const Sim_Cell _gun[] = { // gosper glider gun
    {24,0}, {22,1}, {24,1}, {12,2}, {13,2}, {20,2}, {21,2}, {34,2}, {35,2}, {11,3}, {15,3}, {20,3}, {21,3}, {34,3}, {35,3},
    {0,4}, {1,4}, {10,4}, {16,4}, {20,4}, {21,4}, {0,5}, {1,5}, {10,5}, {14,5}, {16,5}, {17,5}, {22,5}, {24,5},
    {10,6}, {16,6}, {24,6}, {11,7}, {15,7}, {12,8}, {13,8}
};

static void setCell(unsigned char* grid, unsigned width, unsigned height, int x, int y){
    x %= (int)width; y %= (int)height;
    if(x < 0) x += width;
    if(y < 0) y += height;
    grid[y * width + x] = 1;
}

static void placePattern(unsigned char* grid, unsigned width, unsigned height, const Sim_Cell* cells, unsigned count, int ox, int oy, int orient){
    for(unsigned c = 0; c < count; c++){
        int x = cells[c].x, y = cells[c].y;
        if(orient & 1) x = -x; // mirror
        if(orient & 2) y = -y; // flip
        setCell(grid, width, height, ox + x, oy + y);
    }
}

void seedCells(unsigned char* grid, unsigned width, unsigned height){
    const Sim_State* s = &_sim;
    memset(grid, 0, width * height);
    int cx = (int)(s->seedPos[0] * width), cy = (int)(s->seedPos[1] * height);

    switch(s->seed){
    case SEED_Random:
        for(unsigned p = 0; p < width * height; p++) grid[p] = ((double)rand() / RAND_MAX < s->density)? 1 : 0;
        break;
    case SEED_Center: { // random blob around the seed position
        int r = (int)((width < height)? width : height) / 10 + 2;
        for(int y = -r; y <= r; y++)
            for(int x = -r; x <= r; x++)
                if(x * x + y * y <= r * r && (double)rand() / RAND_MAX < 0.5) setCell(grid, width, height, cx + x, cy + y);
    } break;
    case SEED_Glider:
        for(int g = 0; g < s->seedCount; g++)
            placePattern(grid, width, height, _glider, TOOL_COUNT(_glider), (g == 0)? cx : rand() % width, (g == 0)? cy : rand() % height, rand() % 4);
        break;
    case SEED_RPentomino: placePattern(grid, width, height, _rpentomino, TOOL_COUNT(_rpentomino), cx - 1, cy - 1, 0); break;
    case SEED_Acorn: placePattern(grid, width, height, _acorn, TOOL_COUNT(_acorn), cx - 3, cy - 1, 0); break;
    case SEED_Gun: placePattern(grid, width, height, _gun, TOOL_COUNT(_gun), cx - 18, cy - 4, 0); break;
    case SEED_Line: for(int x = -(int)width / 4; x < (int)width / 4; x++) setCell(grid, width, height, cx + x, cy); break;
    case SEED_Cross:
        for(int i = -(int)width / 4; i < (int)width / 4; i++){ setCell(grid, width, height, cx + i, cy); setCell(grid, width, height, cx, cy + i); }
        break;
    case SEED_Ring: {
        double r = ((width < height)? width : height) * 0.25;
        for(int a = 0; a < 2000; a++) setCell(grid, width, height, cx + (int)(r * cos(a * 0.00314159)), cy + (int)(r * sin(a * 0.00314159)));
    } break;
    case SEED_Grid: {
        int step = (s->seedCount > 0)? (int)width / (s->seedCount + 1) : 8;
        if(step < 2) step = 2;
        for(unsigned y = step; y < height; y += step)
            for(unsigned x = step; x < width; x += step) setCell(grid, width, height, x, y);
    } break;
    case SEED_Symmetric: // random quadrant mirrored four ways
        for(unsigned y = 0; y < height / 2; y++)
            for(unsigned x = 0; x < width / 2; x++)
                if((double)rand() / RAND_MAX < s->density){
                    setCell(grid, width, height, x, y); setCell(grid, width, height, width - 1 - x, y);
                    setCell(grid, width, height, x, height - 1 - y); setCell(grid, width, height, width - 1 - x, height - 1 - y);
                }
        break;
    case SEED_Spots: {
        int r = (int)((width < height)? width : height) / 40 + 1;
        for(int g = 0; g < s->seedCount; g++){
            int sx = (g == 0)? cx : rand() % width, sy = (g == 0)? cy : rand() % height;
            for(int y = -r; y <= r; y++) for(int x = -r; x <= r; x++) setCell(grid, width, height, sx + x, sy + y);
        }
    } break;
    }
}

// --------------------------------   Life-like Automata    -------------------------------- //

int parseLifeRule(const char* text, unsigned short* birth, unsigned short* survive){
    *birth = 0; *survive = 0;
    unsigned short* target = NULL;
    int sawBirth = FALSE;
    for(const char* c = text; *c != '\0'; c++){
        char ch = (char)toupper((unsigned char)*c);
        if(ch == 'B'){ target = birth; sawBirth = TRUE; }
        else if(ch == 'S') target = survive;
        else if(ch == '/' || ch == ' ' || ch == ',') { if(!sawBirth) target = survive; }
        else if(ch >= '0' && ch <= '8'){
            if(target == NULL) target = birth;
            *target |= (unsigned short)(1 << (ch - '0'));
        }
        else return FALSE;
    }
    return sawBirth || *survive != 0;
}

static unsigned cellColor(int age){ // age 0 alive, positive generations since death, negative never alive
    if(age == 0) return _sim.colors[0];
    if(age < 0 || _sim.trail <= 0) return _sim.colors[1];
    if(age > _sim.trail) return _sim.colors[1];
    return blend_colors(_sim.colors[2], _sim.colors[1], (double)(age - 1) / (double)_sim.trail);
}

Rasteron_Image* lifeImgOp(unsigned short birth, unsigned short survive, unsigned iterations){
    unsigned width = (unsigned)_sim.cells[0], height = (unsigned)_sim.cells[1];
    unsigned char* grid = (unsigned char*)malloc(width * height);
    unsigned char* next = (unsigned char*)malloc(width * height);
    int* ages = (int*)malloc(width * height * sizeof(int));

    seedCells(grid, width, height);
    for(unsigned p = 0; p < width * height; p++) ages[p] = grid[p]? 0 : -1;

    for(unsigned i = 0; i < iterations; i++){
        for(unsigned y = 0; y < height; y++)
            for(unsigned x = 0; x < width; x++){
                unsigned n = 0;
                for(int dy = -1; dy <= 1; dy++)
                    for(int dx = -1; dx <= 1; dx++){
                        if(dx == 0 && dy == 0) continue;
                        int nx = (int)x + dx, ny = (int)y + dy;
                        if(_sim.wrap){ nx = (nx + width) % width; ny = (ny + height) % height; }
                        else if(nx < 0 || ny < 0 || nx >= (int)width || ny >= (int)height) continue;
                        n += grid[ny * width + nx];
                    }
                unsigned char alive = grid[y * width + x];
                next[y * width + x] = alive? ((survive >> n) & 1) : ((birth >> n) & 1);
            }
        for(unsigned p = 0; p < width * height; p++){
            if(next[p]) ages[p] = 0;
            else if(ages[p] >= 0) ages[p]++;
        }
        unsigned char* swap = grid; grid = next; next = swap;
    }

    Rasteron_Image* lifeImg = RASTERON_ALLOC("life", height, width);
    for(unsigned p = 0; p < width * height; p++) *(lifeImg->data + p) = cellColor(ages[p]) | 0xFF000000;

    free(grid); free(next); free(ages);
    return lifeImg;
}

// --------------------------------   Elementary Automata    -------------------------------- //

Rasteron_Image* elementaryImgOp(unsigned char rule, unsigned generations){
    unsigned width = (unsigned)_sim.cells[0], height = (unsigned)_sim.cells[1];
    unsigned char* rows = (unsigned char*)calloc(width * height, 1); // ring of the newest rows
    unsigned char* row = (unsigned char*)calloc(width, 1);
    unsigned char* next = (unsigned char*)calloc(width, 1);

    if(_sim.seed == SEED_Random) for(unsigned x = 0; x < width; x++) row[x] = ((double)rand() / RAND_MAX < _sim.density)? 1 : 0;
    else row[(unsigned)(_sim.seedPos[0] * (width - 1))] = 1; // single cell, gives the sierpinski triangle for rule 90

    if(generations < 1) generations = 1;
    for(unsigned g = 0; g < generations; g++){
        memcpy(rows + (g % height) * width, row, width);
        for(unsigned x = 0; x < width; x++){
            int l = (int)x - 1, r = (int)x + 1;
            unsigned char left = (l >= 0)? row[l] : (_sim.wrap? row[width - 1] : 0);
            unsigned char right = (r < (int)width)? row[r] : (_sim.wrap? row[0] : 0);
            unsigned pattern = (left << 2) | (row[x] << 1) | right;
            next[x] = (rule >> pattern) & 1;
        }
        unsigned char* swap = row; row = next; next = swap;
    }

    Rasteron_Image* elemImg = solidImgOp((ImageSize){ height, width }, _sim.colors[1] | 0xFF000000);
    unsigned shown = (generations < height)? generations : height;
    unsigned first = generations - shown; // scrolls once generations exceed the rows
    for(unsigned r = 0; r < shown; r++){
        const unsigned char* src = rows + ((first + r) % height) * width;
        for(unsigned x = 0; x < width; x++)
            if(src[x]) *(elemImg->data + r * width + x) = blend_colors(_sim.colors[0], _sim.colors[2], (_sim.trail > 0)? (double)r / height : 0.0) | 0xFF000000;
    }

    free(rows); free(row); free(next);
    return elemImg;
}

// --------------------------------   Turing Patterns    -------------------------------- //

Rasteron_Image* turingImgOp(unsigned iterations){ // gray-scott reaction diffusion
    unsigned width = (unsigned)_sim.cells[0], height = (unsigned)_sim.cells[1];
    unsigned count = width * height;
    float* a = (float*)malloc(count * sizeof(float)); float* b = (float*)malloc(count * sizeof(float));
    float* na = (float*)malloc(count * sizeof(float)); float* nb = (float*)malloc(count * sizeof(float));
    unsigned char* mask = (unsigned char*)malloc(count);

    seedCells(mask, width, height);
    for(unsigned p = 0; p < count; p++){
        a[p] = 1.0f;
        b[p] = mask[p]? 1.0f : 0.0f;
        if(mask[p]) a[p] = 0.5f;
    }

    const float dA = (float)_sim.diffusion[0], dB = (float)_sim.diffusion[1];
    const float feed = (float)_sim.feed, kill = (float)_sim.kill;
    for(unsigned i = 0; i < iterations; i++){
        for(unsigned y = 0; y < height; y++){
            unsigned up = (y == 0)? (_sim.wrap? height - 1 : 0) : y - 1;
            unsigned down = (y == height - 1)? (_sim.wrap? 0 : y) : y + 1;
            for(unsigned x = 0; x < width; x++){
                unsigned left = (x == 0)? (_sim.wrap? width - 1 : 0) : x - 1;
                unsigned right = (x == width - 1)? (_sim.wrap? 0 : x) : x + 1;
                unsigned p = y * width + x;
                float lapA = -a[p] + 0.2f * (a[up * width + x] + a[down * width + x] + a[y * width + left] + a[y * width + right])
                    + 0.05f * (a[up * width + left] + a[up * width + right] + a[down * width + left] + a[down * width + right]);
                float lapB = -b[p] + 0.2f * (b[up * width + x] + b[down * width + x] + b[y * width + left] + b[y * width + right])
                    + 0.05f * (b[up * width + left] + b[up * width + right] + b[down * width + left] + b[down * width + right]);
                float reaction = a[p] * b[p] * b[p];
                na[p] = a[p] + dA * lapA - reaction + feed * (1.0f - a[p]);
                nb[p] = b[p] + dB * lapB + reaction - (kill + feed) * b[p];
                if(na[p] < 0.0f) na[p] = 0.0f; else if(na[p] > 1.0f) na[p] = 1.0f;
                if(nb[p] < 0.0f) nb[p] = 0.0f; else if(nb[p] > 1.0f) nb[p] = 1.0f;
            }
        }
        float* swap = a; a = na; na = swap;
        swap = b; b = nb; nb = swap;
    }

    Rasteron_Image* turingImg = RASTERON_ALLOC("turing", height, width);
    for(unsigned p = 0; p < count; p++){
        double v = (a[p] - b[p]); // 1 is plain substrate, low values are pattern
        v = (v < 0.0)? 0.0 : (v > 1.0)? 1.0 : v;
        unsigned color = (v < 0.5)? blend_colors(_sim.colors[0], _sim.colors[2], v * 2.0) : blend_colors(_sim.colors[2], _sim.colors[1], (v - 0.5) * 2.0);
        *(turingImg->data + p) = color | 0xFF000000;
    }

    free(a); free(b); free(na); free(nb); free(mask);
    return turingImg;
}

// --------------------------------   SimTest Swatch Rules    -------------------------------- //

Rasteron_Image* swatchRuleImgOp(nebrCallback8 rule, unsigned iterations, nebrCallback8 process){
    unsigned width = (unsigned)_sim.cells[0], height = (unsigned)_sim.cells[1];
    Rasteron_Image* seedImg;

    if(_sim.seed == SEED_Random) seedImg = growImgOp((ImageSize){ height, width }, 1.0, _sim.density); // SimTest growth
    else {
        unsigned char* grid = (unsigned char*)malloc(width * height);
        seedCells(grid, width, height);
        seedImg = RASTERON_ALLOC("seed", height, width);
        for(unsigned p = 0; p < width * height; p++) *(seedImg->data + p) = grid[p]? SWATCH_L : SWATCH_RA;
        free(grid);
    }

    Rasteron_Image* simImg = cellwiseExtImgOp(seedImg, rule, (unsigned short)((iterations > 0)? iterations : 1));
    RASTERON_DEALLOC(seedImg);

    if(process != NULL){
        Rasteron_Image* processImg = cellwiseImgOp(simImg, process);
        RASTERON_DEALLOC(simImg);
        simImg = processImg;
    }
    return simImg;
}
