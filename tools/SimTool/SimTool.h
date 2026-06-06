
#define NSIM_GROW 0.1
#define NSIM_COUNT 12

#define SWATCH_L _swatch.colors[SWATCH_Light] // from Tool.h, but used here for ease of use
#define SWATCH_RA _swatch.colors[SWATCH_Red_Add] // from Tool.h, but used here for ease of use
#define SWATCH_RS _swatch.colors[SWATCH_Red_Sub] // from Tool.h, but used here for ease of use
#define SWATCH_GA _swatch.colors[SWATCH_Green_Add] // from Tool.h, but used here for ease of use
#define SWATCH_GS _swatch.colors[SWATCH_Green_Sub] // from Tool.h, but used here for ease of use
#define SWATCH_BA _swatch.colors[SWATCH_Blue_Add] // from Tool.h, but used here for ease of use
#define SWATCH_BS _swatch.colors[SWATCH_Blue_Sub] // from Tool.h, but used here for ease of use
#define SWATCH_D _swatch.colors[SWATCH_Dark] // from Tool.h, but used here for ease of use

#include "Rasteron.h"

extern ColorSwatch _swatch;

// Scanline Rules

static unsigned addlineRules(unsigned color, unsigned neighbors[2]) { return (neighbors[0] == neighbors[1]) ? neighbors[0] + neighbors[1] : color; }

static unsigned levelineRules(unsigned color, unsigned neighbors[2]) { return (color_gray(color) > color_gray(neighbors[1]))? color_level(color, 0.33) : color_level(neighbors[0], 0.66); }

static unsigned serpinskyRules(unsigned color, unsigned neighbors[2]) {
    unsigned short s = 0;
    if (color == SWATCH_GA || color == SWATCH_L) s++;
    if (neighbors[0] == SWATCH_GA || neighbors[0] == SWATCH_L) s++;
    if (neighbors[1] == SWATCH_GA || neighbors[1] == SWATCH_L) s++;
    return (s == 1) ? SWATCH_GA : SWATCH_RA;
}

// Full-Cell Rules

static unsigned shuffleRules(unsigned color, unsigned neighbors[8]) {
    static unsigned r = 0;
    r++;
    for (unsigned n = 0; n < 8; n++) if(neighbors[n] == color && neighbors[n] != NO_COLOR) return neighbors[r % 8];
    else return color;
}
static unsigned conwayRules(unsigned color, unsigned neighbors[8]) {
    unsigned short lives = neighbor_count(SWATCH_GA, neighbors) + neighbor_count(SWATCH_L, neighbors); // countLives(neighbors);
    unsigned short kills = neighbor_count(SWATCH_RA, neighbors) + neighbor_count(SWATCH_D, neighbors); // countKills(neighbors);
    // printf("Lives is %d, %d", lives, kills);

    if ((color == SWATCH_GA || color == SWATCH_L) && lives < 2) return SWATCH_RA; // Any live cell with fewer than two live neighbors dies, as if by underpopulation.
    else if ((color == SWATCH_GA || color == SWATCH_L) && (lives == 2 || lives == 3)) return SWATCH_GA; // Any live cell with two or three live neighbors lives on to the next generation.
    else if ((color == SWATCH_GA || color == SWATCH_L) && lives > 3) return SWATCH_RA; // Any live cell with more than three live neighbors dies, as if by overpopulation.
    else if ((color == SWATCH_RA || color == SWATCH_D) && lives == 3) return SWATCH_L; // Any kills cell with exactly three live neighbors becomes a live cell, as if by reproduction.
    else if (color == SWATCH_RA) return SWATCH_D;
    else return color;
}
static unsigned randWalkRules(unsigned color, unsigned neighbors[8]) {
    unsigned short direction = 0;

    if (color == SWATCH_L) return SWATCH_GA;
    else if (color == SWATCH_RA) return SWATCH_D;
    else if (neighbors[direction] == SWATCH_L || neighbors[direction] == SWATCH_GA) {
        direction = rand() % 8; // randomize walk direction
        return SWATCH_L;
    }
    else if (neighbors[direction] == SWATCH_D || neighbors[direction] == SWATCH_RA) {
        direction = rand() % 8; // randomize walk direction
        return SWATCH_RA;
    }
    else return color;
}
static unsigned amplifyRules(unsigned color, unsigned neighbors[8]) {
    unsigned short lives = neighbor_count(SWATCH_GA, neighbors) + neighbor_count(SWATCH_L, neighbors); // countLives(neighbors);
    unsigned short kills = neighbor_count(SWATCH_RA, neighbors) + neighbor_count(SWATCH_D, neighbors); // countKills(neighbors);
    // printf("Lives is %d, %d", lives, kills);

    if (lives > kills) return (color == SWATCH_GA || color == SWATCH_L) ? SWATCH_GA : SWATCH_L; // color_level(color, 0.5 + (lives * (0.5 / 8.0)));
    else if (kills > lives) return (color == SWATCH_RA || color == SWATCH_D) ? SWATCH_D : SWATCH_RA; // color_level(color, 0.5 - (lives * (0.5 / 8.0)));
    else return color;
}
static unsigned recursiveRules(unsigned color, unsigned neighbors[8]) {
    color += (neighbors[NEBR_Left] > neighbors[NEBR_Right]) ? 16 : -16;
    color += (neighbors[NEBR_Top] > neighbors[NEBR_Bot]) ? 16 : -16;
    color += (neighbors[NEBR_Top_Left] + neighbors[NEBR_Top_Right] > neighbors[NEBR_Bot_Left] + neighbors[NEBR_Bot_Right]) ? 32 : -32;
    return color;
}
static unsigned bloomRules(unsigned color, unsigned neighbors[8]) {
    if (neighbors[NEBR_Bot] == SWATCH_GA || neighbors[NEBR_Top] == SWATCH_GA || neighbors[NEBR_Left] == SWATCH_GA || neighbors[NEBR_Right] == SWATCH_GA)
        return SWATCH_RA;
    else if (neighbors[NEBR_Bot_Left] == SWATCH_RA || neighbors[NEBR_Top_Right] == SWATCH_RA || neighbors[NEBR_Top_Left] == SWATCH_RA || neighbors[NEBR_Bot_Right] == SWATCH_RA)
        return SWATCH_GA;
    else return color;
}
static unsigned unbloomRules(unsigned color, unsigned neighbors[8]) {
    if (neighbors[NEBR_Bot_Left] == SWATCH_RA || neighbors[NEBR_Top_Right] == SWATCH_RA || neighbors[NEBR_Top_Left] == SWATCH_RA || neighbors[NEBR_Bot_Right] == SWATCH_RA)
        return SWATCH_GA;
    else if (neighbors[NEBR_Bot] == SWATCH_GA || neighbors[NEBR_Top] == SWATCH_GA || neighbors[NEBR_Left] == SWATCH_GA || neighbors[NEBR_Right] == SWATCH_GA)
        return SWATCH_RA;
    else return color;
}
static unsigned levelRules(unsigned color, unsigned neighbors[8]) {
    unsigned short lives = neighbor_count(SWATCH_GA, neighbors) + neighbor_count(SWATCH_L, neighbors); // countLives(neighbors);
    unsigned short kills = neighbor_count(SWATCH_RA, neighbors) + neighbor_count(SWATCH_D, neighbors);
    unsigned short diff = abs((short)lives - (short)kills);

    if (diff == 0) return color;
    else if (diff % 2 == 1) return (diff == 1) ? SWATCH_RA : SWATCH_L;
    else return (diff == 2) ? SWATCH_GA : SWATCH_D;
}
static unsigned matchRules(unsigned color, unsigned neighbors[8]) {
    if (neighbors[NEBR_Left] == neighbors[NEBR_Right]) return neighbors[NEBR_Left];
    else if (neighbors[NEBR_Top] == neighbors[NEBR_Bot]) return neighbors[NEBR_Top];
    else if (neighbors[NEBR_Top_Left] == neighbors[NEBR_Bot_Right]) return neighbors[NEBR_Bot_Right];
    else if (neighbors[NEBR_Bot_Left] == neighbors[NEBR_Top_Right]) return neighbors[NEBR_Top_Right];
    else return neighbors[rand() % 8];
}
static unsigned colorizeRules(unsigned color, unsigned neighbors[8]) {
    if (color == SWATCH_L || color == SWATCH_GA) return blend_colors(color, SWATCH_GA, (float)rand() / RAND_MAX);
    else if (color == SWATCH_D || color == SWATCH_RA) return blend_colors(color, SWATCH_RA, (float)rand() / RAND_MAX);
    else return color;
}
static unsigned scatterRules(unsigned color, unsigned neighbors[8]) {
    if (neighbors[NEBR_Left] == neighbors[NEBR_Right] && neighbors[NEBR_Top] == neighbors[NEBR_Bot]) return _swatch.colors[rand() % 8];
    else return color;
}
static unsigned flipRules(unsigned color, unsigned neighbors[8]) {
    if (color == SWATCH_GA || color == SWATCH_L) {
        unsigned lifeCount = 0;
        for (unsigned d = 0; d < 8; d++)
            if (neighbors[d] == SWATCH_GA || neighbors[d] == SWATCH_L) lifeCount++;
        return (lifeCount % 2 == 0) ? SWATCH_RA : SWATCH_D;
    }
    else if (color == SWATCH_RA || color == SWATCH_D) {
        unsigned deadCount = 0;
        for (unsigned d = 0; d < 8; d++)
            if (neighbors[d] == SWATCH_RA || neighbors[d] == SWATCH_D) deadCount++;
        return (deadCount % 2 == 0) ? SWATCH_GA : SWATCH_L;
    }
    return color;
}
static unsigned wolframRules(unsigned color, unsigned neighbors[8]) {
    unsigned short lives = neighbor_count(SWATCH_GA, neighbors) + neighbor_count(SWATCH_L, neighbors); // countLives(neighbors);
    unsigned short kills = neighbor_count(SWATCH_RA, neighbors) + neighbor_count(SWATCH_D, neighbors);

    if (lives == 3 || lives == 5) return SWATCH_GA;
    if (kills == 2 || kills == 6) return SWATCH_RA;
    else return NO_COLOR;
}

// Image Operations

Rasteron_Image* seedImgOp(Rasteron_Image* refImg, double prob);
Rasteron_Image* growImgOp(Rasteron_Image* refImg, double balance, double exFactor);
Rasteron_Image* feedImgOp(ref_image_t refImg, unsigned short iters);
Rasteron_Image* simImgOp(ref_image_t refImage, unsigned short iters, nebrCallback8 callback);