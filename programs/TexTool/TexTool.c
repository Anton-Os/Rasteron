#define RASTERON_ENABLE_ANIM

#define TEXTOOL_OCTAVES 2
#define TEXTOOL_COUNT 10
#define TEXTOOL_POWER 3
#define TEXTOOL_PURTURB 0.0

#include "../_Prog.h"

static char mode = 'a';
static ColorGrid grid = { 1, 1, 0xFF333333, 0xFFEEEEEE };

#include "Tex.c"

// Overriden Functions

mixCallback mixer = NULL;

void _onKeyEvent(char key){ 
    static unsigned mode = 0;
    grid = (ColorGrid){ pow(2, _dimens[0]), pow(2, _dimens[1]),  _swatch.colors[SWATCH_Light],  _swatch.colors[SWATCH_Dark] };

    if(isspace(key) && _outputImg != NULL) saveToFile(_outputImg, IMG_Bmp);
    else if(tolower(key) == 'q' || tolower(key) == 'w' || tolower(key) == 'e' || tolower(key) == 'r' || tolower(key) == 't' || tolower(key) == 'y' || tolower(key) == 'u' || tolower(key) == 'i' || tolower(key) == 'o' || tolower(key) == 'p'){
        switch(tolower(key)){
            case 'w': _swatch = createSwatch(0xFF0000FF, 0x32); _swatch.colors[SWATCH_Light] = 0xFF0000FF; _swatch.colors[SWATCH_Dark] = 0xFFFF0000; break;
            case 'e': _swatch = createSwatch(0xFFff9cd2, 0xAA); _swatch.colors[SWATCH_Light] = 0xFFff9cd2; _swatch.colors[SWATCH_Dark] = 0xFF0b6485; break;
            case 'r': _swatch = createSwatch(0xFFFF00FF, 0xF6); _swatch.colors[SWATCH_Light] = 0xFFFF00FF; _swatch.colors[SWATCH_Dark] = 0xFF00FF00; break;
            case 't': _swatch = createSwatch(0xFFffbf00, 0x0a); _swatch.colors[SWATCH_Light] = 0xFFffbf00; _swatch.colors[SWATCH_Dark] = 0xFF381c0a; break;
            case 'y': _swatch = createSwatch(0xFF66fff7, 0x66); _swatch.colors[SWATCH_Light] = 0xFF66fff7; _swatch.colors[SWATCH_Dark] = 0xFFc4ff66; break; // good for entwine algorithm
            case 'u': _swatch = createSwatch(0xFF091796, 0xEE); _swatch.colors[SWATCH_Light] = 0xFF091796; _swatch.colors[SWATCH_Dark] = 0xFFEEEEEE; break;
            case 'i': _swatch = createSwatch(0xFF81ff38, 0x88); _swatch.colors[SWATCH_Light] = 0xFF81ff38; _swatch.colors[SWATCH_Dark] = 0xFF888888; break;
            case 'o': _swatch = createSwatch(0xFFff6369, 0x11); _swatch.colors[SWATCH_Light] = 0xFFff6369; _swatch.colors[SWATCH_Dark] = 0xFF111111; break;
            case 'p': _swatch = createSwatch(RAND_COLOR(), RAND_COLOR() % 256); _swatch.colors[SWATCH_Light] = RAND_COLOR(); _swatch.colors[SWATCH_Dark] = RAND_COLOR(); break;
            default: _swatch.colors[SWATCH_Light] = 0xFF333333; _swatch.colors[SWATCH_Dark] = 0xFFEEEEEE; break;
        }
    }

    if(tolower(key) == 'a' || tolower(key) == 's' || tolower(key) == 'd' || tolower(key) == 'f' || tolower(key) == 'g' || tolower(key) == 'h' || tolower(key) == 'j' || tolower(key) == 'k' || tolower(key) == 'l' || tolower(key) == 'p')
        mode = tolower(key);
        
    if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    switch(tolower(mode)){
        case 'a': _outputImg = noiseImgOp((ImageSize){ 1024, 1024 }, grid); break;
        // case 's': _outputImg = noiseImgOp_crossed((ImageSize){ 1024, 1024 }, *grid); break;
        // case 'd': _outputImg = noiseImgOp_stepped((ImageSize){ 1024, 1024 }, *grid); break;
        case 's': _outputImg = purturbNoiseImgOp((ImageSize){ 1024, 1024 }, grid, powerNoiseMod, TEXTOOL_PURTURB, TEXTOOL_PURTURB); break;
        case 'd': _outputImg = purturbNoiseImgOp((ImageSize){ 1024, 1024 }, grid, trigNoiseMod, TEXTOOL_PURTURB, TEXTOOL_PURTURB); break;
        case 'f': _outputImg = purturbNoiseImgOp((ImageSize){ 1024, 1024 }, grid, rangeNoiseMod, TEXTOOL_PURTURB, TEXTOOL_PURTURB); break;
        case 'g': _outputImg = noiseImgOp_add((ImageSize){ 1024, 1024 }, grid, TEXTOOL_OCTAVES); break;
        case 'h': _outputImg = noiseImgOp_diff((ImageSize){ 1024, 1024 }, grid, TEXTOOL_OCTAVES); break;
        case 'j': _outputImg = noiseImgOp_low((ImageSize){ 1024, 1024 }, grid, TEXTOOL_OCTAVES); break;
        case 'k': _outputImg = noiseImgOp_hi((ImageSize){ 1024, 1024 }, grid, TEXTOOL_OCTAVES); break;
        case 'l': _outputImg = noiseExtImgOp_octave((ImageSize){ 1024, 1024 }, grid, TEXTOOL_OCTAVES, asm_rgb); break;
        //case 'j': _outputImg = noiseExtImgOp((ImageSize){ 1024, 1024 }, *grid, levelsNoiseMod); break;
        //case 'k': _outputImg = noiseExtImgOp((ImageSize){ 1024, 1024 }, *grid, cosMod); break;
        //case 'l': _outputImg = noiseExtImgOp((ImageSize){ 1024, 1024 }, *grid, tanMod); break;
        // default: _outputImg = noiseImgOp((ImageSize){ 1024, 1024 }, grid); break;
    }

    if(_outputImg != NULL){
        Rasteron_Image* currentImg = copyImgOp(_outputImg);
        Rasteron_Image* mixerImg = (_savedImg != NULL)? copyImgOp(_savedImg) : noiseImgOp((ImageSize){ 1024, 1024 }, grid); // texImgOp(mode, &grid);

        RASTERON_DEALLOC(_outputImg);
        switch(tolower(key)){
            case 'z': _outputImg = mixingImgOp(currentImg, mixerImg, root_colors); break;
            case 'x': _outputImg = mixingImgOp(currentImg, mixerImg, mult_colors); break;
            case 'c': _outputImg = mixingImgOp(currentImg, mixerImg, asm_rgb); break;
            case 'v': _outputImg = mixingImgOp(currentImg, mixerImg, bit_colors_and); break;
            case 'b': _outputImg = mixingImgOp(currentImg, mixerImg, bit_colors_or); break;
            case 'n': _outputImg = mixingImgOp(currentImg, mixerImg, bit_colors_xor); break;
            case 'm': _outputImg = warpingImgOp(currentImg, mixerImg); break;
            default: _outputImg = copyImgOp(currentImg); break;
        }
        RASTERON_DEALLOC(currentImg);
        RASTERON_DEALLOC(mixerImg);
    }
}

void _onPressEvent(double x, double y){}
void _onTickEvent(unsigned secs){}

// Generative Function

Rasteron_Image* texTool(char* args) {
    char buffer[256];
    unsigned b = 0; // buffer offset

    if (strlen(args) > 1) 
        for (unsigned c = 0; c < strlen(args); c++) {
            buffer[b] = args[c];
            if(isspace(args[c])) {
                for (unsigned e = 0; e < b; e++) buffer[e] = '\0'; // clears buffer
                if (buffer[0] == '-') { // starts with --this
                    printf("Parsing arg value: %s", buffer);
                    if (strstr(buffer, "algo")) {
                        puts("Algorithm selected");
                        if (strstr(buffer, "octave")) puts("Octave noise selected");
                        else puts("Perlin noise selected");
                    }
                    else if (strstr(buffer, "arg")) {
                        puts("Arguments selected");
                    }
                    else if (strstr(buffer, "grid")) {
                        puts("Grid selected");
                    }
                    else if (strstr(buffer, "mix")) {
                        puts("Mix selected");
                        if (strstr(buffer, "and")) mixer = bit_colors_and;
                        else if (strstr(buffer, "xor")) mixer = bit_colors_xor;
                        else if (strstr(buffer, "or")) mixer = bit_colors_or;
                        else puts("Mixer unselected");
                    }
                    else perror("Invalid arg selected");
                }
                else _onKeyEvent(buffer[0]);
            }
        }
    return noiseImgOp((ImageSize){ 1024, 1024 }, grid);
}

// Executable Function

int main(int argc, char** argv) {
    _run(argc, argv, texTool); // system specific initialization and continuous loop

    // RASTERON_QUEUE_DEALLOC(_mainQueue);
    RASTERON_DEALLOC(_outputImg);

    return 0;
}
