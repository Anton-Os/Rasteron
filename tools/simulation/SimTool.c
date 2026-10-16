#include "SimTool.h"

// --------------------------------   State    -------------------------------- //

static const Sim_State _simDefaults = {
    RULE_Life, "B3/S23", 30, // rule, lifeRule, wolfram
    60, { 260, 260 }, // iterations, cells
    SEED_Random, 0.3, 6, { 0.5, 0.5 }, // seed, density, seedCount, seedPos
    TRUE, 8, // wrap, trail
    { 0xFF7CFC9A, 0xFF0B1020, 0xFF2E6BFF }, // colors
    0.035, 0.065, { 1.0, 0.5 }, // feed, kill, diffusion
    PROCESS_None
};

Sim_State _sim;
ColorSwatch _swatch = { 0xFF888888, { 0xFFFF8888, 0xFF88FF88, 0xFF8888FF, 0xFF333333, 0xFFEEEEEE, 0xFF888800, 0xFF880088, 0xFF008888 }, 0xF }; // SimTest default

static const char* const _ruleNames[] = { "life", "highlife", "seeds", "daynight", "maze", "coral", "custom", "sierpinski", "wolfram", "turing",
    "shuffle", "randwalk", "amplify", "level", "match", "recursive", "bloom", "unbloom", "wolfram8", "flip", NULL };
static const char* const _lifeRules[] = { "B3/S23", "B36/S23", "B2/S", "B3678/S34678", "B3/S12345", "B3/S45678" }; // matches life to coral
static const char* const _seedNames[] = { "random", "center", "glider", "rpentomino", "acorn", "gun", "line", "cross", "ring", "grid", "symmetric", "spots", NULL };
static const char* const _processNames[] = { "none", "same", "shuffle", "colorize", "scatter", "recursive", "flip", NULL };

static const Tool_Param _simParams[] = {
    TOOL_PARAM("rule", PARAM_Enum, Sim_State, rule, 1, 0, 0, _ruleNames, "automaton: life-like, 1D elementary, turing reaction, or SimTest rules"),
    TOOL_PARAM("liferule", PARAM_Text, Sim_State, lifeRule, 1, 0, 0, NULL, "birth/survival counts for rule custom, e.g. B36/S23"),
    TOOL_PARAM("wolfram", PARAM_Int, Sim_State, wolfram, 1, 0, 255, NULL, "elementary rule number for rule wolfram (30, 90, 110...)"),
    TOOL_PARAM("iterations", PARAM_Int, Sim_State, iterations, 1, 0, 100000, NULL, "generations, 1D rows, or turing steps (turing needs thousands)"),
    TOOL_PARAM("cells", PARAM_Int, Sim_State, cells, 2, 8, SIM_CELLS_MAX, NULL, "grid size in cells x y (one value sets both)"),
    TOOL_PARAM("placement", PARAM_Enum, Sim_State, seed, 1, 0, 0, _seedNames, "where the starting cells are placed (set seed changes the random seed)"),
    TOOL_PARAM("density", PARAM_Float, Sim_State, density, 1, 0, 1, NULL, "fill probability for random and symmetric seeds"),
    TOOL_PARAM("seedcount", PARAM_Int, Sim_State, seedCount, 1, 1, 500, NULL, "gliders or spots placed, grid spacing"),
    TOOL_PARAM("seedpos", PARAM_Float, Sim_State, seedPos, 2, 0, 1, NULL, "where placed seeds start, 0.5 0.5 is the center"),
    TOOL_PARAM("wrap", PARAM_Bool, Sim_State, wrap, 1, 0, 0, NULL, "edges wrap around"),
    TOOL_PARAM("trail", PARAM_Int, Sim_State, trail, 1, 0, 200, NULL, "generations a dead cell takes to fade, 0 off"),
    TOOL_PARAM("colors", PARAM_Color, Sim_State, colors, 3, 0, 0, NULL, "alive, dead, and trail colors (also used for SimTest swatch)"),
    TOOL_PARAM("feed", PARAM_Float, Sim_State, feed, 1, 0, 0.12, NULL, "turing feed rate"),
    TOOL_PARAM("kill", PARAM_Float, Sim_State, kill, 1, 0, 0.12, NULL, "turing kill rate"),
    TOOL_PARAM("diffusion", PARAM_Float, Sim_State, diffusion, 2, 0, 1, NULL, "turing diffusion of a and b"),
    TOOL_PARAM("process", PARAM_Enum, Sim_State, process, 1, 0, 0, _processNames, "SimTest post process pass (z to m keys)"),
};

// --------------------------------   Generation    -------------------------------- //

static void buildSwatch(void){ // SimTest rules compare against swatch colors, derive them from the palette
    unsigned alive = _sim.colors[0] | 0xFF000000, dead = _sim.colors[1] | 0xFF000000, trail = _sim.colors[2] | 0xFF000000;
    _swatch.base = blend_colors(alive, dead, 0.5) | 0xFF000000;
    _swatch.colors[SWATCH_Green_Add] = alive;
    _swatch.colors[SWATCH_Light] = color_level(alive, 0.75) | 0xFF000000;
    _swatch.colors[SWATCH_Red_Add] = dead;
    _swatch.colors[SWATCH_Dark] = color_level(dead, 0.25) | 0xFF000000;
    _swatch.colors[SWATCH_Blue_Add] = trail;
    _swatch.colors[SWATCH_Red_Sub] = color_level(trail, 0.3) | 0xFF000000;
    _swatch.colors[SWATCH_Green_Sub] = color_level(trail, 0.65) | 0xFF000000;
    _swatch.colors[SWATCH_Blue_Sub] = color_invert(trail) | 0xFF000000;
}

static nebrCallback8 swatchRule(int rule){
    switch(rule){
        case RULE_Shuffle: return shuffleRules;
        case RULE_RandWalk: return randWalkRules;
        case RULE_Amplify: return amplifyRules;
        case RULE_Level: return levelRules;
        case RULE_Match: return matchRules;
        case RULE_Recursive: return recursiveRules;
        case RULE_Bloom: return bloomRules;
        case RULE_Unbloom: return unbloomRules;
        case RULE_Wolfram8: return wolframRules;
        case RULE_Flip: return flipRules;
        default: return conwayRules;
    }
}

static nebrCallback8 processRule(int process, int rule){
    switch(process){
        case PROCESS_Same: return (rule >= RULE_Shuffle)? swatchRule(rule) : conwayRules;
        case PROCESS_Shuffle: return shuffleRules;
        case PROCESS_Colorize: return colorizeRules;
        case PROCESS_Scatter: return scatterRules;
        case PROCESS_Recursive: return recursiveRules;
        case PROCESS_Flip: return flipRules;
        default: return NULL;
    }
}

static Rasteron_Image* simGenerate(ImageSize size, const Rasteron_Image* baseImg){
    Sim_State* s = &_sim;
    buildSwatch();
    unsigned iterations = (unsigned)s->iterations;
    nebrCallback8 process = processRule(s->process, s->rule);
    Rasteron_Image* gridImg = NULL;

    if(s->rule <= RULE_Custom){
        const char* text = (s->rule == RULE_Custom)? s->lifeRule : _lifeRules[s->rule];
        unsigned short birth, survive;
        if(!parseLifeRule(text, &birth, &survive)){
            printf("Cannot read life rule '%s', expected something like B3/S23\n", text);
            parseLifeRule("B3/S23", &birth, &survive);
        }
        gridImg = lifeImgOp(birth, survive, iterations);
    }
    else if(s->rule == RULE_Sierpinski) gridImg = elementaryImgOp(90, iterations);
    else if(s->rule == RULE_Wolfram) gridImg = elementaryImgOp((unsigned char)s->wolfram, iterations);
    else if(s->rule == RULE_Turing){
        if(iterations < 500) printf("  turing patterns form over thousands of steps, try iterations 3000 or 'turing spots'\n");
        gridImg = turingImgOp(iterations);
    }
    else {
        gridImg = swatchRuleImgOp(swatchRule(s->rule), iterations, process);
        process = NULL; // already applied
    }

    if(process != NULL){
        Rasteron_Image* processImg = cellwiseImgOp(gridImg, process);
        RASTERON_DEALLOC(gridImg);
        gridImg = processImg;
    }

    Rasteron_Image* image = resizeImgOp(size, gridImg); // cells scale up without smoothing
    RASTERON_DEALLOC(gridImg);
    return image;
}

static void simAnimate(unsigned frame, unsigned frameCount){
    if(_sim.rule == RULE_Turing) _sim.iterations = (int)((double)_sim.iterations * (frame + 1) / frameCount); // pattern growing in
    else _sim.iterations += (int)frame; // one generation per frame from the current state
}

// --------------------------------   Commands    -------------------------------- //

static int cmd_step(int argc, char** argv){
    int steps = 1;
    if(argc > 1 && (!toolArgs_int(argv[1], &steps) || steps == 0)) return TOOL_Fail;
    int next = _sim.iterations + steps;
    _sim.iterations = (next < 0)? 0 : (next > 100000)? 100000 : next;
    printf("  iterations = %d\n", _sim.iterations);
    return TOOL_Render;
}

static int cmd_life(int argc, char** argv){
    if(argc < 2) return TOOL_Fail;
    unsigned short birth, survive;
    if(!parseLifeRule(argv[1], &birth, &survive)){ printf("  write rules as B<counts>/S<counts>, e.g. B3/S23 or B36/S23\n"); return TOOL_Fail; }
    _sim.rule = RULE_Custom;
    snprintf(_sim.lifeRule, TOOL_TEXT_MAX, "%s", argv[1]);
    printf("  rule custom %s\n", _sim.lifeRule);
    return TOOL_Render;
}

static int cmd_wolfram(int argc, char** argv){
    int number;
    if(argc < 2 || !toolArgs_int(argv[1], &number) || number < 0 || number > 255) return TOOL_Fail;
    _sim.rule = RULE_Wolfram;
    _sim.wolfram = number;
    if(_sim.iterations < _sim.cells[1]) _sim.iterations = _sim.cells[1]; // fill the rows
    printf("  wolfram rule %d over %d rows\n", number, _sim.iterations);
    return TOOL_Render;
}

static int cmd_turing(int argc, char** argv){
    static const char* const presets[] = { "spots", "stripes", "coral", "mitosis", "worms", "holes", "maze", NULL };
    static const double rates[][2] = { { 0.035, 0.065 }, { 0.022, 0.051 }, { 0.0545, 0.062 }, { 0.0367, 0.0649 }, { 0.078, 0.061 }, { 0.039, 0.058 }, { 0.029, 0.057 } };
    int preset = (argc > 1)? toolArgs_choice(argv[1], presets) : 0;
    if(preset < 0){ printf("  presets: spots stripes coral mitosis worms holes maze\n"); return TOOL_Fail; }
    _sim.rule = RULE_Turing;
    _sim.feed = rates[preset][0];
    _sim.kill = rates[preset][1];
    _sim.diffusion[0] = 1.0; _sim.diffusion[1] = 0.5;
    if(_sim.iterations < 1000) _sim.iterations = 3000;
    if(_sim.cells[0] > 300){ _sim.cells[0] = 200; _sim.cells[1] = 200; }
    if(_sim.seed != SEED_Spots && _sim.seed != SEED_Center && _sim.seed != SEED_Ring && _sim.seed != SEED_Cross){ _sim.seed = SEED_Spots; _sim.seedCount = 12; }
    printf("  turing %s: feed %g kill %g, %d steps\n", presets[preset], _sim.feed, _sim.kill, _sim.iterations);
    return TOOL_Render;
}

static const Tool_Command _simCommands[] = {
    { "step", "[n]", "advance n generations (default 1, negative goes back)", cmd_step },
    { "life", "<B../S..>", "custom life-like rule such as B36/S23", cmd_life },
    { "wolfram", "<0-255>", "elementary automaton rule, 90 draws the sierpinski triangle", cmd_wolfram },
    { "turing", "[spots|stripes|coral|mitosis|worms|holes|maze]", "turing pattern preset (gray-scott feed and kill)", cmd_turing },
};

// --------------------------------   Tool Description    -------------------------------- //

static const Tool_Desc _simTool = {
    "simulation", "SimTool", "cellular automata, wolfram rules, and turing patterns, based on SimTest",
    &_sim, &_simDefaults, sizeof(Sim_State),
    _simParams, TOOL_COUNT(_simParams),
    _simCommands, TOOL_COUNT(_simCommands),
    simGenerate, simAnimate, NULL, NULL, NULL
};

int main(int argc, char** argv) { return tool_run(&_simTool, argc, argv); }
