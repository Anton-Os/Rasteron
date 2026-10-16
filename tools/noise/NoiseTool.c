#include "NoiseTool.h"

// --------------------------------   State    -------------------------------- //

static const Noise_State _noiseDefaults = {
    NOISE_Perlin, MATERIAL_None,
    { 0xFF111111, 0xFFEEEEEE, 0xFFFF8800 }, // colors
    5, 0.5, 2.0, // octaves, persistence, lacunarity
    { 8, 8 }, TRUE, 8, 0.9, // cells, wrap, gradients, jitter
    { 1.0, 1.0 }, { 0.0, 0.0 }, 0.0, // stretch, offset, warp
    { 0.0, 0.0 }, // perturb
    NMOD_None, 3.0, 1.0, // modifier, power, contrast
    NMIX_None
};

Noise_State _noise;

static const char* const _typeNames[] = { "value", "perlin", "worley", "white", "octave", "add", "diff", "mult", "low", "hi", "rand", "tiled", "crossed", "stepped", "perturb", NULL };
static const char* const _materialNames[] = { "none", "grass", "ooze", "sand", "metal", "fabric", "wood", "marble", "stone", "water", "lava", "clouds", "rust", NULL };
static const char* const _modifierNames[] = { "none", "levels", "range", "power", "trig", "quilt", NULL };
static const char* const _mixNames[] = { "none", "mult", "qdroot", "assim", "and", "or", "xor", "warp", "blend", NULL };

static const Tool_Param _noiseParams[] = {
    TOOL_PARAM("type", PARAM_Enum, Noise_State, type, 1, 0, 0, _typeNames, "noise algorithm (value and octave kinds are library noises)"),
    TOOL_PARAM("material", PARAM_Enum, Noise_State, material, 1, 0, 0, _materialNames, "material callback on perlin lattice; 'material <name>' also sets colors"),
    TOOL_PARAM("colors", PARAM_Color, Noise_State, colors, 3, 0, 0, NULL, "color range low high, plus accent"),
    TOOL_PARAM("octaves", PARAM_Int, Noise_State, octaves, 1, 1, NOISE_OCTAVES_MAX, NULL, "noise layers (library octave noises use up to 8)"),
    TOOL_PARAM("persistence", PARAM_Float, Noise_State, persistence, 1, 0, 1, NULL, "amplitude kept per octave"),
    TOOL_PARAM("lacunarity", PARAM_Float, Noise_State, lacunarity, 1, 1, 4, NULL, "frequency growth per octave"),
    TOOL_PARAM("cells", PARAM_Int, Noise_State, cells, 2, 1, 256, NULL, "lattice cells across x y (one value sets both)"),
    TOOL_PARAM("wrap", PARAM_Bool, Noise_State, wrap, 1, 0, 0, NULL, "lattice wraps so the texture tiles seamlessly"),
    TOOL_PARAM("gradients", PARAM_Int, Noise_State, gradients, 1, 2, 256, NULL, "gradient directions on the perlin lattice"),
    TOOL_PARAM("jitter", PARAM_Float, Noise_State, jitter, 1, 0, 1, NULL, "worley feature point jitter inside lattice cells"),
    TOOL_PARAM("stretch", PARAM_Float, Noise_State, stretch, 2, 0.05, 20, NULL, "lattice scale along x y"),
    TOOL_PARAM("offset", PARAM_Float, Noise_State, offset, 2, -10000, 10000, NULL, "lattice offset x y, animated by frames/encode"),
    TOOL_PARAM("warp", PARAM_Float, Noise_State, warp, 1, 0, 10, NULL, "domain warp strength"),
    TOOL_PARAM("perturb", PARAM_Float, Noise_State, perturb, 2, 0, 1, NULL, "perturb noise x y offsets (TexTest)"),
    TOOL_PARAM("modifier", PARAM_Enum, Noise_State, modifier, 1, 0, 0, _modifierNames, "TexTest noise callback applied to interpolation"),
    TOOL_PARAM("power", PARAM_Float, Noise_State, power, 1, 0.1, 10, NULL, "exponent for power and trig modifiers"),
    TOOL_PARAM("contrast", PARAM_Float, Noise_State, contrast, 1, 0, 4, NULL, "1 unchanged"),
    TOOL_PARAM("mix", PARAM_Enum, Noise_State, mix, 1, 0, 0, _mixNames, "combine with the applied image (TexTest bottom row)"),
};

// --------------------------------   Materials    -------------------------------- //

typedef struct { unsigned colors[3]; int cells; int octaves; } Noise_MaterialPreset;

static const Noise_MaterialPreset _materialPresets[] = {
    { { 0xFF111111, 0xFFEEEEEE, 0xFFFF8800 }, 8, 5 }, // none
    { { 0xFF0F3D0A, 0xFF6BBF3A, 0xFFC8B560 }, 6, 5 }, // grass
    { { 0xFF1A0F2E, 0xFF7CFC3C, 0xFFF0FFE0 }, 3, 5 }, // ooze
    { { 0xFFB08850, 0xFFEBD3A0, 0xFF5A4630 }, 6, 5 }, // sand
    { { 0xFF4A4E55, 0xFFC9CED6, 0xFFFFFFFF }, 4, 6 }, // metal
    { { 0xFF20263A, 0xFF8A3B3B, 0xFFC9B48A }, 14, 4 }, // fabric
    { { 0xFF5A3418, 0xFFB57A45, 0xFF3A200C }, 3, 5 }, // wood
    { { 0xFF8E8E92, 0xFFF4F2EE, 0xFF4A4A50 }, 3, 6 }, // marble
    { { 0xFF55524D, 0xFF9C978E, 0xFF1E1C1A }, 6, 5 }, // stone
    { { 0xFF04365E, 0xFF1A8CB8, 0xFFDFF8FF }, 4, 5 }, // water
    { { 0xFF1A0A06, 0xFF8A1A05, 0xFFFFD040 }, 4, 6 }, // lava
    { { 0xFF3A78C8, 0xFFFFFFFF, 0xFFFFF4D8 }, 4, 6 }, // clouds
    { { 0xFF5A5E60, 0xFF9A4A20, 0xFF2A1408 }, 5, 5 }, // rust
};

// --------------------------------   Generation    -------------------------------- //

static Rasteron_Image* contrastImgOp(ref_image_t refImage, double contrast){
    Rasteron_Image* contrastImg = RASTERON_ALLOC("contrast", refImage->height, refImage->width);
    for(unsigned p = 0; p < refImage->width * refImage->height; p++){
        unsigned color = *(refImage->data + p);
        unsigned result = color & ALPHA_CHANNEL;
        for(int shift = 0; shift <= 16; shift += 8){
            double c = ((color >> shift) & 0xFF);
            c = (c - 128.0) * contrast + 128.0;
            result |= (unsigned)((c < 0.0)? 0.0 : (c > 255.0)? 255.0 : c) << shift;
        }
        *(contrastImg->data + p) = result;
    }
    return contrastImg;
}

static mixCallback mixFunction(int mix){
    switch(mix){
        case NMIX_Mult: return mult_rgb;
        case NMIX_Qdroot: return qdroot_colors;
        case NMIX_Assim: return assimMix_rgb;
        case NMIX_And: return bit_colors_and;
        case NMIX_Or: return bit_colors_or;
        case NMIX_Xor: return bit_colors_xor;
        case NMIX_Blend: return blend_colors_eq;
        default: return NULL;
    }
}

static Rasteron_Image* baseNoiseImgOp(ImageSize size){
    Noise_State* s = &_noise;
    ColorGrid grid = { (unsigned)s->cells[0], (unsigned)s->cells[1], s->colors[0], s->colors[1] };
    unsigned short octaves = (unsigned short)((s->octaves > 8)? 8 : s->octaves);
    noiseCallback modifier = noiseModifier(s->modifier);

    if(s->material != MATERIAL_None) return latticeImgOp(size);
    switch(s->type){
        case NOISE_Value: return (modifier != NULL)? noiseExtImgOp(size, grid, modifier) : noiseImgOp(size, grid);
        case NOISE_Perlin: case NOISE_Worley: return latticeImgOp(size);
        case NOISE_White: return noiseImgOp_white(size, s->colors[0], s->colors[1]);
        case NOISE_Octave: return noiseImgOp_octave(size, grid, octaves);
        case NOISE_Add: return noiseImgOp_add(size, grid, octaves);
        case NOISE_Diff: return noiseImgOp_diff(size, grid, octaves);
        case NOISE_Mult: return noiseImgOp_mult(size, grid, octaves);
        case NOISE_Low: return noiseImgOp_low(size, grid, octaves);
        case NOISE_Hi: return noiseImgOp_hi(size, grid, octaves);
        case NOISE_Rand: return noiseImgOp_rand(size, grid, octaves);
        case NOISE_Tiled: return noiseImgOp_tiled(size, grid);
        case NOISE_Crossed: return noiseImgOp_crossed(size, grid);
        case NOISE_Stepped: return noiseImgOp_stepped(size, grid);
        case NOISE_Perturb: return purturbNoiseImgOp(size, grid, modifier, s->perturb[0], s->perturb[1]);
        default: return latticeImgOp(size);
    }
}

static Rasteron_Image* noiseGenerate(ImageSize size, const Rasteron_Image* baseImg){
    Rasteron_Image* image = baseNoiseImgOp(size);

    if(_noise.contrast != 1.0){
        Rasteron_Image* contrastImg = contrastImgOp(image, _noise.contrast);
        RASTERON_DEALLOC(image);
        image = contrastImg;
    }
    if(baseImg != NULL && _noise.mix != NMIX_None){
        Rasteron_Image* mixedImg = (_noise.mix == NMIX_Warp)? warpingImgOp(image, baseImg) : mixImgOp(image, baseImg, mixFunction(_noise.mix));
        RASTERON_DEALLOC(image);
        image = mixedImg;
    }
    return image;
}

static void noiseAnimate(unsigned frame, unsigned frameCount){ // scrolls the lattice, one full period loops when wrapping
    double t = (double)frame / (double)frameCount;
    _noise.offset[0] += t * (_noise.wrap? _noise.cells[0] * _noise.stretch[0] : 4.0);
}

// --------------------------------   Commands    -------------------------------- //

static int cmd_material(int argc, char** argv){
    int material = (argc > 1)? toolArgs_choice(argv[1], _materialNames) : -1;
    if(material < 0){
        printf("  materials:"); for(unsigned m = 0; _materialNames[m] != NULL; m++) printf(" %s", _materialNames[m]); printf("\n");
        return TOOL_Fail;
    }
    _noise.material = material;
    for(unsigned c = 0; c < 3; c++) _noise.colors[c] = _materialPresets[material].colors[c];
    _noise.cells[0] = _noise.cells[1] = _materialPresets[material].cells;
    _noise.octaves = _materialPresets[material].octaves;
    printf("  material %s, colors %08X %08X %08X, cells %d, octaves %d\n", _materialNames[material],
        _noise.colors[0], _noise.colors[1], _noise.colors[2], _noise.cells[0], _noise.octaves);
    return TOOL_Render;
}

static int cmd_palette(int argc, char** argv){
    int material = (argc > 1)? toolArgs_choice(argv[1], _materialNames) : -1;
    if(material < 0) return TOOL_Fail;
    for(unsigned c = 0; c < 3; c++) _noise.colors[c] = _materialPresets[material].colors[c];
    printf("  colors = %08X %08X %08X\n", _noise.colors[0], _noise.colors[1], _noise.colors[2]);
    return TOOL_Render;
}

static int cmd_swap(int argc, char** argv){
    unsigned temp = _noise.colors[0];
    _noise.colors[0] = _noise.colors[1];
    _noise.colors[1] = temp;
    return TOOL_Render;
}

static int cmd_lattice(int argc, char** argv){ // cells, wrap, gradients in one command
    if(argc < 2) return TOOL_Fail;
    int x, y = -1, gradients = _noise.gradients;
    if(!toolArgs_int(argv[1], &x) || x < 1 || x > 256) return TOOL_Fail;
    int a = 2;
    if(argc > a && toolArgs_int(argv[a], &y)) a++;
    if(y < 1 || y > 256) y = x;
    int wrap = _noise.wrap;
    for(; a < argc; a++){
        if(toolArgs_equals(argv[a], "wrap")) wrap = TRUE;
        else if(toolArgs_equals(argv[a], "nowrap")) wrap = FALSE;
        else if(!toolArgs_int(argv[a], &gradients) || gradients < 2 || gradients > 256) return TOOL_Fail;
    }
    _noise.cells[0] = x; _noise.cells[1] = y; _noise.wrap = wrap; _noise.gradients = gradients;
    printf("  lattice %dx%d, %s, %d gradients\n", x, y, wrap? "wrapping" : "not wrapping", gradients);
    return TOOL_Render;
}

static const Tool_Command _noiseCommands[] = {
    { "material", "<name>", "pick a material callback and its palette, cells, and octaves", cmd_material },
    { "palette", "<material>", "only load a material's color range", cmd_palette },
    { "swap", "", "swap low and high colors", cmd_swap },
    { "lattice", "<x> [y] [wrap|nowrap] [gradients]", "set lattice properties together", cmd_lattice },
};

// --------------------------------   Tool Description    -------------------------------- //

static const Tool_Desc _noiseTool = {
    "noise", "NoiseTool", "procedural noise and material textures, based on TexTest",
    &_noise, &_noiseDefaults, sizeof(Noise_State),
    _noiseParams, TOOL_COUNT(_noiseParams),
    _noiseCommands, TOOL_COUNT(_noiseCommands),
    noiseGenerate, noiseAnimate, NULL, NULL, NULL
};

int main(int argc, char** argv) { return tool_run(&_noiseTool, argc, argv); }
