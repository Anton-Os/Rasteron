#include "IllusionTool.h"

// --------------------------------   State    -------------------------------- //

static const Illusion_State _illusionDefaults = {
    ILLUSION_CafeWall, // type
    { 0xFF000000, 0xFFFFFFFF, 0xFFE03030, 0xFF808080 }, // colors
    { 12, 9 }, // count
    0.5, // size
    0.5, // strength
    0.4, // thickness
    0.0, // angle
    1.0, // zoom
    0.0, // phase
    FALSE, // reveal
    2, // smooth
    IMIX_None // mix
};

Illusion_State _illusion;

static const char* const _typeNames[] = { "cafewall", "hermann", "scintillating", "zollner", "hering", "poggendorff", "mullerlyer", "ponzo", "ebbinghaus",
    "jastrow", "kanizsa", "white", "munker", "contrast", "chevreul", "cornsweet", "fraser", "pinna", "drift", "ouchi", "bulge", "moire", "lilac", NULL };
static const char* const _mixNames[] = { "none", "blend", "add", "diff", "mult", "xor", NULL };

static const Tool_Param _illusionParams[] = {
    TOOL_PARAM("type", PARAM_Enum, Illusion_State, type, 1, 0, 0, _typeNames, "illusion, keeps the other settings (preset <name> also loads its recommended ones)"),
    TOOL_PARAM("colors", PARAM_Color, Illusion_State, colors, 4, 0, 0, NULL, "dark, light, accent, and mid colors"),
    TOOL_PARAM("count", PARAM_Int, Illusion_State, count, 2, 1, 64, NULL, "repetitions across x y (tiles, lines, discs, rings)"),
    TOOL_PARAM("scale", PARAM_Float, Illusion_State, size, 1, 0, 1, NULL, "relative size of the main elements"),
    TOOL_PARAM("strength", PARAM_Float, Illusion_State, strength, 1, 0, 1, NULL, "strength of the pattern causing the illusion, 0 removes the effect"),
    TOOL_PARAM("thickness", PARAM_Float, Illusion_State, thickness, 1, 0, 1, NULL, "width of lines, mortar, and streets"),
    TOOL_PARAM("angle", PARAM_Float, Illusion_State, angle, 1, -360, 360, NULL, "rotation of the whole figure in degrees"),
    TOOL_PARAM("zoom", PARAM_Float, Illusion_State, zoom, 1, 0.1, 10, NULL, "scale of the whole figure"),
    TOOL_PARAM("phase", PARAM_Float, Illusion_State, phase, 1, -1000, 1000, NULL, "motion offset in radians, animated by frames/encode"),
    TOOL_PARAM("reveal", PARAM_Bool, Illusion_State, reveal, 1, 0, 0, NULL, "remove or mark the misleading part to check what is really drawn"),
    TOOL_PARAM("smooth", PARAM_Int, Illusion_State, smooth, 1, 1, ILLUSION_SMOOTH_MAX, NULL, "samples per pixel along each axis, 1 is fastest"),
    TOOL_PARAM("mix", PARAM_Enum, Illusion_State, mix, 1, 0, 0, _mixNames, "combine with the applied image (see apply)"),
};

// --------------------------------   Generation    -------------------------------- //

static mixCallback mixFunction(int mix){
    switch(mix){
        case IMIX_Blend: return blend_colors_eq;
        case IMIX_Add: return add_rgb;
        case IMIX_Diff: return diff_rgb;
        case IMIX_Mult: return mult_rgb;
        case IMIX_Xor: return bit_colors_xor;
        default: return NULL;
    }
}

static Rasteron_Image* illusionGenerate(ImageSize size, const Rasteron_Image* baseImg){
    Rasteron_Image* image = illusionImgOp(size);

    if(baseImg != NULL && _illusion.mix != IMIX_None){
        Rasteron_Image* mixedImg = mixImgOp(image, baseImg, mixFunction(_illusion.mix));
        RASTERON_DEALLOC(image);
        image = mixedImg;
        for(unsigned p = 0; p < image->width * image->height; p++) *(image->data + p) |= 0xFF000000; // mixing keeps the result opaque
    }
    return image;
}

static void illusionAnimate(unsigned frame, unsigned frameCount){
    double t = (double)frame / (double)frameCount;
    if(_illusionInfo[_illusion.type].usesPhase) _illusion.phase += t * ILLUSION_TAU; // one full cycle loops seamlessly
    else _illusion.strength *= 0.5 - (0.5 * cos(t * ILLUSION_TAU)); // the effect grows from nothing and fades again
}

// --------------------------------   Commands    -------------------------------- //

static void printAbout(int type){
    const Illusion_Info* info = &_illusionInfo[type];
    printf("  %s: %s\n", info->name, info->about);
    if(info->revealed != NULL) printf("  reveal on: %s\n", info->revealed);
}

static int cmd_preset(int argc, char** argv){
    int type = (argc > 1)? toolArgs_choice(argv[1], _typeNames) : _illusion.type;
    if(type < 0){
        printf("Illusions:");
        for(unsigned t = 0; t < ILLUSION_Count; t++) printf(" %s", _illusionInfo[t].name);
        printf("\n");
        return TOOL_Fail;
    }
    int smooth = _illusion.smooth, mix = _illusion.mix; // quality and mixing are kept
    illusionPreset(&_illusion, type);
    _illusion.smooth = smooth; _illusion.mix = mix;
    printAbout(type);
    return TOOL_Render;
}

static int cmd_about(int argc, char** argv){
    if(argc > 1 && toolArgs_equals(argv[1], "all")){
        for(unsigned t = 0; t < ILLUSION_Count; t++) printf("  %-14s %s\n", _illusionInfo[t].name, _illusionInfo[t].about);
        return TOOL_Ok;
    }
    int type = (argc > 1)? toolArgs_choice(argv[1], _typeNames) : _illusion.type;
    if(type < 0) return TOOL_Fail;
    printAbout(type);
    return TOOL_Ok;
}

static int cmd_swap(int argc, char** argv){
    unsigned temp = _illusion.colors[ICOLOR_Dark];
    _illusion.colors[ICOLOR_Dark] = _illusion.colors[ICOLOR_Light];
    _illusion.colors[ICOLOR_Light] = temp;
    printf("  colors = %08X %08X %08X %08X\n", _illusion.colors[0], _illusion.colors[1], _illusion.colors[2], _illusion.colors[3]);
    return TOOL_Render;
}

static int cmd_next(int argc, char** argv){ // steps through the catalogue
    int back = (argc > 1 && (toolArgs_equals(argv[1], "back") || toolArgs_equals(argv[1], "previous")));
    char* presetArgs[2] = { "preset", (char*)_illusionInfo[(_illusion.type + (back? ILLUSION_Count - 1 : 1)) % ILLUSION_Count].name };
    return cmd_preset(2, presetArgs);
}

static const Tool_Command _illusionCommands[] = {
    { "preset", "[illusion]", "switch illusion and load its recommended settings (no name reloads the current one)", cmd_preset },
    { "next", "[back]", "preset of the next or previous illusion", cmd_next },
    { "about", "[illusion|all]", "what to look for in an illusion", cmd_about },
    { "swap", "", "swap dark and light colors", cmd_swap },
};

// --------------------------------   Tool Description    -------------------------------- //

static const Tool_Desc _illusionTool = {
    "illusions", "IllusionTool", "optical illusions with adjustable strength (preset <name> picks one, about explains it)",
    &_illusion, &_illusionDefaults, sizeof(Illusion_State),
    _illusionParams, TOOL_COUNT(_illusionParams),
    _illusionCommands, TOOL_COUNT(_illusionCommands),
    illusionGenerate, illusionAnimate, NULL, NULL, NULL
};

int main(int argc, char** argv) { return tool_run(&_illusionTool, argc, argv); }
