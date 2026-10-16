#define NSIM_GROW 0.1
#define NSIM_COUNT 12
#define SIM_CELLS_MAX 1300

#define SWATCH_L _swatch.colors[SWATCH_Light] // same shorthands as Test.h
#define SWATCH_RA _swatch.colors[SWATCH_Red_Add]
#define SWATCH_RS _swatch.colors[SWATCH_Red_Sub]
#define SWATCH_GA _swatch.colors[SWATCH_Green_Add]
#define SWATCH_GS _swatch.colors[SWATCH_Green_Sub]
#define SWATCH_BA _swatch.colors[SWATCH_Blue_Add]
#define SWATCH_BS _swatch.colors[SWATCH_Blue_Sub]
#define SWATCH_D _swatch.colors[SWATCH_Dark]

#include "../Tool.h"

// --------------------------------   Simulation State    -------------------------------- //

enum SIM_Rule {
    RULE_Life = 0, RULE_HighLife = 1, RULE_Seeds = 2, RULE_DayNight = 3, RULE_Maze = 4, RULE_Coral = 5, RULE_Custom = 6, // life-like
    RULE_Sierpinski = 7, RULE_Wolfram = 8, // elementary 1D automata drawn row by row
    RULE_Turing = 9, // gray-scott reaction diffusion
    RULE_Shuffle = 10, RULE_RandWalk = 11, RULE_Amplify = 12, RULE_Level = 13, RULE_Match = 14, RULE_Recursive = 15, // SimTest rules
    RULE_Bloom = 16, RULE_Unbloom = 17, RULE_Wolfram8 = 18, RULE_Flip = 19
};

enum SIM_Seed {
    SEED_Random = 0, SEED_Center = 1, SEED_Glider = 2, SEED_RPentomino = 3, SEED_Acorn = 4, SEED_Gun = 5,
    SEED_Line = 6, SEED_Cross = 7, SEED_Ring = 8, SEED_Grid = 9, SEED_Symmetric = 10, SEED_Spots = 11
};

enum SIM_Process { PROCESS_None = 0, PROCESS_Same = 1, PROCESS_Shuffle = 2, PROCESS_Colorize = 3, PROCESS_Scatter = 4, PROCESS_Recursive = 5, PROCESS_Flip = 6 };

typedef struct {
    int rule;
    char lifeRule[TOOL_TEXT_MAX]; // birth and survival counts for custom, e.g. B36/S23
    int wolfram; // elementary rule number for wolfram
    int iterations; // generations, rows, or reaction steps
    int cells[2]; // grid size in cells
    int seed; // seed placement
    double density; // fill probability for random seeds
    int seedCount; // gliders or spots placed
    double seedPos[2]; // center of placed seeds, 0.5 0.5 is the middle
    int wrap; // edges wrap around
    int trail; // generations dead cells take to fade
    unsigned colors[3]; // alive, dead, and trail colors
    double feed; // turing feed rate
    double kill; // turing kill rate
    double diffusion[2]; // turing diffusion of a and b
    int process; // SimTest post process pass
} Sim_State;

extern Sim_State _sim;
extern ColorSwatch _swatch; // SimTest rules read swatch colors

// --------------------------------   SimTest Rules (see Sim.c)    -------------------------------- //

unsigned shuffleRules(unsigned color, unsigned neighbors[8]);
unsigned conwayRules(unsigned color, unsigned neighbors[8]);
unsigned randWalkRules(unsigned color, unsigned neighbors[8]);
unsigned amplifyRules(unsigned color, unsigned neighbors[8]);
unsigned recursiveRules(unsigned color, unsigned neighbors[8]);
unsigned bloomRules(unsigned color, unsigned neighbors[8]);
unsigned unbloomRules(unsigned color, unsigned neighbors[8]);
unsigned levelRules(unsigned color, unsigned neighbors[8]);
unsigned matchRules(unsigned color, unsigned neighbors[8]);
unsigned colorizeRules(unsigned color, unsigned neighbors[8]);
unsigned scatterRules(unsigned color, unsigned neighbors[8]);
unsigned flipRules(unsigned color, unsigned neighbors[8]);
unsigned wolframRules(unsigned color, unsigned neighbors[8]);

Rasteron_Image* growImgOp(ImageSize size, double balance, double exFactor); // SimTest growth image

// --------------------------------   Engines (see Sim.c)    -------------------------------- //

int parseLifeRule(const char* text, unsigned short* birth, unsigned short* survive); // bit masks of neighbor counts
void seedCells(unsigned char* grid, unsigned width, unsigned height); // places seeds from _sim
Rasteron_Image* lifeImgOp(unsigned short birth, unsigned short survive, unsigned iterations);
Rasteron_Image* elementaryImgOp(unsigned char rule, unsigned rows);
Rasteron_Image* turingImgOp(unsigned iterations);
Rasteron_Image* swatchRuleImgOp(nebrCallback8 rule, unsigned iterations, nebrCallback8 process);
