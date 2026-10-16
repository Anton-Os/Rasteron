#include "FilterTool.h"

// --------------------------------   State    -------------------------------- //

static const Filter_State _filterDefaults = {
    "", // source
    SIDE_Right + 1, 1.0, // crop side and factor
    0, FALSE, // flip, mirror
    { 0, 0 }, // resize
    0.0, // corners
    0.0, 0.0, // darkness, lightness
    1.0, 1.0, 1.0, // saturation, contrast, gamma
    0.0, // hue
    0xFFFF8800, 0.0, // tint
    0.0, // threshold
    FALSE, FALSE, // invert, grey
    FCHANNEL_All, FSWAP_None,
    0, 0, // levels, smooth
    FFADE_None
};

Filter_State _filter;

static const char* const _channelNames[] = { "all", "red", "green", "blue", NULL };
static const char* const _swapNames[] = { "none", "rg", "rb", "gb", NULL };
static const char* const _fadeNames[] = { "none", "white", "black", NULL };

static const Tool_Param _filterParams[] = {
    TOOL_PARAM("cropside", PARAM_Enum, Filter_State, cropSide, 1, 0, 0, _toolSideNames, "side that is cut away by crop"),
    TOOL_PARAM("crop", PARAM_Float, Filter_State, crop, 1, 0.01, 1, NULL, "fraction of the image kept toward cropside, 1 keeps all"),
    TOOL_PARAM("flip", PARAM_Enum, Filter_State, flip, 1, 0, 0, _toolFlipNames, "rotate clockwise, counter clockwise, or upside down"),
    TOOL_PARAM("mirror", PARAM_Bool, Filter_State, mirror, 1, 0, 0, NULL, "mirror horizontally"),
    TOOL_PARAM("resize", PARAM_Int, Filter_State, resize, 2, 0, TOOL_SIZE_MAX, NULL, "width height in pixels, 0 keeps that side"),
    TOOL_PARAM("corners", PARAM_Float, Filter_State, corners, 1, 0, 2.5, NULL, "cut corners to transparent"),
    TOOL_PARAM("darkness", PARAM_Float, Filter_State, darkness, 1, 0, 1, NULL, "darken toward black"),
    TOOL_PARAM("lightness", PARAM_Float, Filter_State, lightness, 1, 0, 1, NULL, "lighten toward white"),
    TOOL_PARAM("saturation", PARAM_Float, Filter_State, saturation, 1, 0, 4, NULL, "0 grey, 1 unchanged, above 1 more vivid"),
    TOOL_PARAM("contrast", PARAM_Float, Filter_State, contrast, 1, 0, 4, NULL, "1 unchanged"),
    TOOL_PARAM("gamma", PARAM_Float, Filter_State, gamma, 1, 0.1, 5, NULL, "1 unchanged, above 1 brightens midtones"),
    TOOL_PARAM("hue", PARAM_Float, Filter_State, hue, 1, -360, 360, NULL, "hue rotation in degrees"),
    TOOL_PARAM("tint", PARAM_Color, Filter_State, tint, 1, 0, 0, NULL, "tint color"),
    TOOL_PARAM("tintamount", PARAM_Float, Filter_State, tintAmount, 1, 0, 1, NULL, "how much tint is applied"),
    TOOL_PARAM("threshold", PARAM_Float, Filter_State, threshold, 1, 0, 1, NULL, "black and white cut by brightness, 0 off"),
    TOOL_PARAM("invert", PARAM_Bool, Filter_State, invert, 1, 0, 0, NULL, "invert colors"),
    TOOL_PARAM("grey", PARAM_Bool, Filter_State, grey, 1, 0, 0, NULL, "greyscale"),
    TOOL_PARAM("channel", PARAM_Enum, Filter_State, channel, 1, 0, 0, _channelNames, "isolate one color channel"),
    TOOL_PARAM("swap", PARAM_Enum, Filter_State, swap, 1, 0, 0, _swapNames, "swap two color channels"),
    TOOL_PARAM("levels", PARAM_Int, Filter_State, levels, 1, 0, 64, NULL, "posterize into brightness levels, 0 off"),
    TOOL_PARAM("smooth", PARAM_Int, Filter_State, smooth, 1, 0, 16, NULL, "antialias passes"),
    TOOL_PARAM("fade", PARAM_Enum, Filter_State, fade, 1, 0, 0, _fadeNames, "make white or black areas transparent"),
};

// --------------------------------   Generation    -------------------------------- //

static Rasteron_Image* filterGenerate(ImageSize size, const Rasteron_Image* baseImg){
    if(baseImg == NULL){
        printf("No reference image, use: load <file.bmp|png|tiff>\n");
        return solidImgOp(size, 0xFF333333);
    }
    return filterChainImgOp(baseImg, &_filter);
}

static int filterApply(void){ // applied image becomes the new reference, filters start fresh
    char source[TOOL_TEXT_MAX];
    strcpy(source, _filter.source);
    _filter = _filterDefaults;
    strcpy(_filter.source, source);
    puts("  filter parameters reset, further filters stack on the applied image");
    return FALSE;
}

static void filterAnimate(unsigned frame, unsigned frameCount){ // sweeps from the reference to the filtered result
    double t = (double)frame / (double)((frameCount > 1)? frameCount - 1 : 1);
    Filter_State target = _filter;
    _filter = _filterDefaults;
    strcpy(_filter.source, target.source);
    _filter.darkness = target.darkness * t; _filter.lightness = target.lightness * t;
    _filter.saturation = 1.0 + (target.saturation - 1.0) * t;
    _filter.contrast = 1.0 + (target.contrast - 1.0) * t;
    _filter.gamma = 1.0 + (target.gamma - 1.0) * t;
    _filter.hue = target.hue * t;
    _filter.tint = target.tint; _filter.tintAmount = target.tintAmount * t;
    _filter.crop = 1.0 - (1.0 - target.crop) * t; _filter.cropSide = target.cropSide;
    _filter.resize[0] = target.resize[0]; _filter.resize[1] = target.resize[1]; // keeps frame sizes stable
}

// --------------------------------   Commands    -------------------------------- //

static int loadReference(const char* fileName){
    FILE* file = fopen(fileName, "rb");
    if(file == NULL){ printf("Cannot open '%s'\n", fileName); return FALSE; }
    fclose(file);
    if(strlen(fileName) < 5 || getFormat(fileName) == IMG_NonValid){ printf("'%s' is not a bmp, png, or tiff file\n", fileName); return FALSE; }

    Rasteron_Image* image = loadImgOp(fileName);
    if(image == NULL || image->width == 0 || image->height == 0){ printf("Failed to read '%s'\n", fileName); return FALSE; }

    tool_setBase(image);
    snprintf(_filter.source, TOOL_TEXT_MAX, "%s", fileName);
    printf("Loaded %s (%ux%u)\n", fileName, image->width, image->height);
    return TRUE;
}

static int cmd_load(int argc, char** argv){
    if(argc < 2) return TOOL_Fail;
    char fileName[1024];
    if(argc == 2 && strchr(argv[1], '/') == NULL && strchr(argv[1], '\\') == NULL){ // bare name, try cwd then assets
        FILE* file = fopen(argv[1], "rb");
        if(file != NULL){ fclose(file); snprintf(fileName, sizeof(fileName), "%s", argv[1]); }
        else genFullFilePath(argv[1], fileName);
    }
    else {
        fileName[0] = '\0';
        for(int a = 1; a < argc; a++){ // rejoin paths with spaces typed without quotes
            if(a > 1) strncat(fileName, " ", sizeof(fileName) - strlen(fileName) - 1);
            strncat(fileName, argv[a], sizeof(fileName) - strlen(fileName) - 1);
        }
    }
    if(!loadReference(fileName)) return TOOL_Fail;
    char source[TOOL_TEXT_MAX];
    strcpy(source, _filter.source);
    _filter = _filterDefaults; // new image starts unfiltered
    strcpy(_filter.source, source);
    return TOOL_Render;
}

static int cmd_crop(int argc, char** argv){
    if(argc < 3) return TOOL_Fail;
    int side = toolArgs_choice(argv[1], _toolSideNames);
    double factor;
    if(side < 1 || side > SIDE_Right + 1 || !toolArgs_float(argv[2], &factor) || factor <= 0.0 || factor > 1.0){
        printf("  side is top|bottom|left|right, factor is the fraction kept between 0 and 1\n");
        return TOOL_Fail;
    }
    _filter.cropSide = side;
    _filter.crop = factor;
    printf("  crop %s %g\n", _toolSideNames[side], factor);
    return TOOL_Render;
}

static int cmd_original(int argc, char** argv){
    if(_filter.source[0] == '\0'){ printf("No reference image loaded yet\n"); return TOOL_Ok; }
    char source[TOOL_TEXT_MAX];
    strcpy(source, _filter.source);
    if(!loadReference(source)) return TOOL_Ok;
    _filter = _filterDefaults;
    strcpy(_filter.source, source);
    return TOOL_Render;
}

static int cmd_info(int argc, char** argv){
    const Rasteron_Image* base = tool_getBase();
    const Rasteron_Image* output = tool_getOutput();
    printf("  source: %s\n", (_filter.source[0] != '\0')? _filter.source : "(none)");
    if(base != NULL) printf("  reference: %ux%u\n", base->width, base->height);
    if(output != NULL) printf("  output: %ux%u\n", output->width, output->height);
    printf("  filters: %s\n", filterIsNeutral(&_filter)? "none active" : "active (get lists values)");
    return TOOL_Ok;
}

static const Tool_Command _filterCommands[] = {
    { "load", "<file>", "load a reference image (a bare name also searches assets)", cmd_load },
    { "crop", "<top|bottom|left|right> <factor>", "set crop side and kept fraction together", cmd_crop },
    { "original", "", "reload the reference file, dropping applied filters", cmd_original },
    { "info", "", "show reference and output sizes", cmd_info },
};

// --------------------------------   Tool Description    -------------------------------- //

static void filterInit(int argc, char** argv){
    char fileName[1024];
    genFullFilePath(FILTER_DEFAULT_IMAGE, fileName);
    FILE* file = fopen(fileName, "rb");
    if(file != NULL){
        fclose(file);
        loadReference(fileName);
    }
    else printf("Default image %s not found, use: load <file>\n", fileName);
}

static const Tool_Desc _filterTool = {
    "filters", "FilterTool", "crop, flip, and tune a reference image (load <file> or pass it as the first argument)",
    &_filter, &_filterDefaults, sizeof(Filter_State),
    _filterParams, TOOL_COUNT(_filterParams),
    _filterCommands, TOOL_COUNT(_filterCommands),
    filterGenerate, filterAnimate, filterApply, filterInit, NULL
};

int main(int argc, char** argv) { return tool_run(&_filterTool, argc, argv); }
