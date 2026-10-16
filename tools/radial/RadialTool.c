#include "RadialTool.h"

// --------------------------------   State    -------------------------------- //

static const Radial_State _radialDefaults = {
    RADIAL_Spiral, // mode
    { 0xFF111144, 0xFF66EEFF, 0xFF000000 }, // colors
    WAVE_Sin, // wave
    6, // symmetry
    4.0, // frequency
    0.0, // twist
    1.0, // recede
    0.0, // phase
    { 0.0, 0.0 }, // center
    1.0, // zoom
    0.6, // fog
    1, // variant
    3, // iterations
    EFFECT_None, // effect
    0.5, // strength
    MIX_None // mix
};

Radial_State _radial;

static const char* const _modeNames[] = { "rings", "spiral", "star", "tunnel", "petals", "vortex", "swirl", "spirals", "hypnosis", "radial", "mandala", NULL };
static const char* const _waveNames[] = { "sin", "cos", "tan", "saw", "triangle", "square", "pulse", "arc", NULL };
static const char* const _effectNames[] = { "none", "invert", "grey", "posterize", "smooth", "vignette", "glow", "channel", NULL };
static const char* const _mixNames[] = { "none", "blend", "fuse", "add", "diff", "mult", "xor", "and", "or", "warp", NULL };

static const Tool_Param _radialParams[] = {
    TOOL_PARAM("mode", PARAM_Enum, Radial_State, mode, 1, 0, 0, _modeNames, "pattern, the last five come from HypnoTest"),
    TOOL_PARAM("colors", PARAM_Color, Radial_State, colors, 3, 0, 0, NULL, "inner, outer, and distance (fog) colors"),
    TOOL_PARAM("wave", PARAM_Enum, Radial_State, wave, 1, 0, 0, _waveNames, "periodic function shaping rings and spokes"),
    TOOL_PARAM("symmetry", PARAM_Int, Radial_State, symmetry, 1, 1, 128, NULL, "symmetry lines (spokes, arms, petals) around the center"),
    TOOL_PARAM("frequency", PARAM_Float, Radial_State, frequency, 1, 0.01, 200, NULL, "ring density, also swirl and spiral distance factor"),
    TOOL_PARAM("twist", PARAM_Float, Radial_State, twist, 1, -100, 100, NULL, "angular twist that grows with depth"),
    TOOL_PARAM("recede", PARAM_Float, Radial_State, recede, 1, 0, 4, NULL, "depth falloff toward the center, 0 flat, 1 perspective tunnel"),
    TOOL_PARAM("phase", PARAM_Float, Radial_State, phase, 1, -1000, 1000, NULL, "phase offset in radians, animated by frames/encode"),
    TOOL_PARAM("center", PARAM_Float, Radial_State, center, 2, -0.5, 0.5, NULL, "vanishing point offset x y from screen center"),
    TOOL_PARAM("zoom", PARAM_Float, Radial_State, zoom, 1, 0.05, 20, NULL, "scale of the pattern"),
    TOOL_PARAM("fog", PARAM_Float, Radial_State, fog, 1, 0, 1, NULL, "how much the center fades into the distance color"),
    TOOL_PARAM("variant", PARAM_Int, Radial_State, variant, 1, 1, 9, NULL, "mix function for hypnosis (1-3), radial (1-9), mandala (1-7)"),
    TOOL_PARAM("iterations", PARAM_Int, Radial_State, iterations, 1, 0, 30, NULL, "hypnosis mixing passes"),
    TOOL_PARAM("effect", PARAM_Enum, Radial_State, effect, 1, 0, 0, _effectNames, "post effect applied after the pattern"),
    TOOL_PARAM("strength", PARAM_Float, Radial_State, strength, 1, 0, 1, NULL, "effect strength"),
    TOOL_PARAM("mix", PARAM_Enum, Radial_State, mix, 1, 0, 0, _mixNames, "combine with the applied image (see apply)"),
};

// --------------------------------   Generation    -------------------------------- //

static mixCallback mixFunction(int mix){
    switch(mix){
        case MIX_Blend: return blend_colors_eq;
        case MIX_Fuse: return fuse_colors_eq;
        case MIX_Add: return add_rgb;
        case MIX_Diff: return diff_rgb;
        case MIX_Mult: return mult_rgb;
        case MIX_Xor: return bit_colors_xor;
        case MIX_And: return bit_colors_and;
        case MIX_Or: return bit_colors_or;
        default: return NULL;
    }
}

static Rasteron_Image* patternImgOp(ImageSize size){
    Radial_State* s = &_radial;
    unsigned colors[3] = { s->colors[0], s->colors[1], s->colors[2] };

    switch(s->mode){
    case RADIAL_Swirl: return swirlyImgOp(size, s->frequency * 2.5, s->colors[0], s->colors[1]);
    case RADIAL_Spirals: return spiralsImgOp(size, s->symmetry * (RADIAL_SEGS / 6.0), s->frequency * (RADIAL_DIST / 4.0));
    case RADIAL_Hypnosis: return hypnosisImgOp(size, s->colors[0], s->colors[1], (unsigned short)s->iterations, hypnoticMixVariant(s->variant));
    case RADIAL_Radial: return radialImgOp(size, colors, radialMixVariant(s->variant));
    case RADIAL_Mandala: return mandalaImgOp(size, mandalaMap, mandalaMixVariant(s->variant), mandalaInterpVariant(s->variant));
    default: return periodicImgOp(size);
    }
}

static Rasteron_Image* radialGenerate(ImageSize size, const Rasteron_Image* baseImg){
    Radial_State* s = &_radial;
    Rasteron_Image* image = patternImgOp(size);

    if((s->mode == RADIAL_Hypnosis || s->mode == RADIAL_Radial) && s->phase != 0.0){ // these modes rotate with phase
        Rasteron_Image* spinImg = spinImgOp(image, s->phase, s->center[0], s->center[1]);
        RASTERON_DEALLOC(image);
        image = spinImg;
    }

    if(s->effect != EFFECT_None){
        Rasteron_Image* effectImg = radialEffectImgOp(image, s->effect, s->strength, s->center[0], s->center[1]);
        RASTERON_DEALLOC(image);
        image = effectImg;
    }

    if(baseImg != NULL && s->mix != MIX_None){
        Rasteron_Image* mixedImg = (s->mix == MIX_Warp)? warpingImgOp(image, baseImg) : mixImgOp(image, baseImg, mixFunction(s->mix));
        RASTERON_DEALLOC(image);
        image = mixedImg;
    }
    return image;
}

static void radialAnimate(unsigned frame, unsigned frameCount){
    double t = (double)frame / (double)frameCount;
    _radial.phase += t * 6.28318530717958647692; // one full cycle loops seamlessly
}

// --------------------------------   Commands    -------------------------------- //

static int cmd_swap(int argc, char** argv){
    unsigned temp = _radial.colors[0];
    _radial.colors[0] = _radial.colors[1];
    _radial.colors[1] = temp;
    printf("  colors = %08X %08X %08X\n", _radial.colors[0], _radial.colors[1], _radial.colors[2]);
    return TOOL_Render;
}

static int cmd_randomize(int argc, char** argv){
    int keepMode = (argc > 1 && toolArgs_equals(argv[1], "colors"));
    for(unsigned c = 0; c < 3; c++) _radial.colors[c] = RAND_COLOR();
    if(!keepMode){
        _radial.mode = rand() % (RADIAL_Vortex + 1);
        _radial.wave = rand() % (WAVE_Arc + 1);
        _radial.symmetry = 2 + rand() % 14;
        _radial.frequency = 1.0 + (rand() % 80) / 10.0;
        _radial.twist = ((rand() % 200) - 100) / 10.0;
    }
    printf("  mode %s, wave %s, symmetry %d, frequency %g, twist %g\n", _modeNames[_radial.mode], _waveNames[_radial.wave], _radial.symmetry, _radial.frequency, _radial.twist);
    return TOOL_Render;
}

static int cmd_preset(int argc, char** argv){
    static const char* const presets[] = { "hypno", "tunnel", "flower", "vortex", "target", NULL };
    int preset = (argc > 1)? toolArgs_choice(argv[1], presets) : -1;
    if(preset < 0){
        printf("Presets: hypno tunnel flower vortex target\n");
        return TOOL_Fail;
    }
    Radial_State* s = &_radial;
    *s = _radialDefaults;
    switch(preset){
    case 0: s->mode = RADIAL_Spiral; s->colors[0] = 0xFF000000; s->colors[1] = 0xFFFFFFFF; s->symmetry = 2; s->frequency = 6.0; s->wave = WAVE_Square; s->fog = 0.0; break;
    case 1: s->mode = RADIAL_Tunnel; s->colors[0] = 0xFF101830; s->colors[1] = 0xFFE0F0FF; s->symmetry = 12; s->frequency = 2.0; s->twist = 0.5; break;
    case 2: s->mode = RADIAL_Petals; s->colors[0] = 0xFF3A0030; s->colors[1] = 0xFFFFA0C8; s->colors[2] = 0xFFFFF4C0; s->symmetry = 8; s->frequency = 1.5; s->recede = 0.6; break;
    case 3: s->mode = RADIAL_Vortex; s->colors[0] = 0xFF001018; s->colors[1] = 0xFF00E0A0; s->symmetry = 5; s->frequency = 3.0; s->twist = 30.0; break;
    case 4: s->mode = RADIAL_Rings; s->colors[0] = 0xFFB00000; s->colors[1] = 0xFFFFFFFF; s->wave = WAVE_Square; s->frequency = 1.0; s->recede = 0.5; s->fog = 0.0; break;
    }
    printf("  preset %s\n", presets[preset]);
    return TOOL_Render;
}

static const Tool_Command _radialCommands[] = {
    { "swap", "", "swap inner and outer colors", cmd_swap },
    { "randomize", "[colors]", "random colors and pattern ('colors' keeps the pattern)", cmd_randomize },
    { "preset", "<hypno|tunnel|flower|vortex|target>", "load a starting point", cmd_preset },
};

// --------------------------------   Tool Description    -------------------------------- //

static const Tool_Desc _radialTool = {
    "radial", "RadialTool", "radial visuals receding into the screen center, based on HypnoTest",
    &_radial, &_radialDefaults, sizeof(Radial_State),
    _radialParams, TOOL_COUNT(_radialParams),
    _radialCommands, TOOL_COUNT(_radialCommands),
    radialGenerate, radialAnimate, NULL, NULL, NULL
};

int main(int argc, char** argv) { return tool_run(&_radialTool, argc, argv); }
