#include "SimTool.h"

// Scanline Rules

unsigned addlineRules(unsigned cell, unsigned nerbs[2]) { return (nerbs[0] == nerbs[1]) ? nerbs[0] + nerbs[1] : cell; }

unsigned levelineRules(unsigned cell, unsigned nerbs[2]) { return (color_gray(cell) > color_gray(nerbs[1]))? color_level(cell, 0.33) : color_level(nerbs[0], 0.66); }

unsigned serpinskyRules(unsigned cell, unsigned nerbs[2]) {
    unsigned short s = 0;
    if (cell == SWATCH_GA || cell == SWATCH_L) s++;
    if (nerbs[0] == SWATCH_GA || nerbs[0] == SWATCH_L) s++;
    if (nerbs[1] == SWATCH_GA || nerbs[1] == SWATCH_L) s++;
    return (s == 1) ? SWATCH_GA : SWATCH_RA;
}

// Full-Cell Rules

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