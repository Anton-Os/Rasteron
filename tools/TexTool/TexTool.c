#define RASTERON_ENABLE_ANIM

#define TEXTOOL_OCTAVES 3
#define TEXTOOL_COUNT 10
#define TEXTOOL_POWER 3
#define TEXTOOL_PURTURB_X 0.0 // replace with variable
#define TEXTOOL_PURTURB_Y 0.0 // replace with variable

#include "../_Tool.h"

#include "Tex.c"
#include "TexArgs.c"

// Overriden Functions

mixCallback mixer = NULL;

void _onKeyEvent(char key){ 
    static char keysave = 'a';
    grid = (ColorGrid){ pow(2, _dimens[0]), pow(2, _dimens[1]),  _swatch.colors[SWATCH_Light],  _swatch.colors[SWATCH_Dark] };

    if(KEYS_TOP_ROW(key))
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

    if(KEYS_MID_ROW(key)) keysave = tolower(key);
        
    ImageSize imgSize = (ImageSize){ 1024, 1024 };
    if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    switch(tolower(keysave)){
        case 'a': _outputImg = noiseImgOp(imgSize, grid); break;
        case 's': _outputImg = purturbNoiseImgOp(imgSize, grid, powerNoiseMod, xArg, yArg); break;
        case 'd': _outputImg = purturbNoiseImgOp(imgSize, grid, trigNoiseMod, xArg, yArg); break;
        case 'f': _outputImg = purturbNoiseImgOp(imgSize, grid, rangeNoiseMod, xArg, yArg); break;
        case 'g': _outputImg = (mode != 1)? noiseImgOp_add(imgSize, grid, mode) : noiseImgOp_crossed(imgSize, grid); break;
        case 'h': _outputImg = (mode != 1)? noiseImgOp_diff(imgSize, grid, mode) :noiseImgOp_stepped(imgSize, grid); break; break;
        case 'j': _outputImg = (mode != 1)? noiseImgOp_low(imgSize, grid, mode) : noiseExtImgOp(imgSize, grid, rangeNoiseMod); break;
        case 'k': _outputImg = (mode != 1)? noiseImgOp_hi(imgSize, grid, mode) : noiseExtImgOp(imgSize, grid, powerNoiseMod); break;
        case 'l': _outputImg = (mode != 1)? noiseExtImgOp_octave(imgSize, grid, mode, sqroot_colors) : noiseExtImgOp(imgSize, grid, quiltNoiseMod); break;
    }

    if(_outputImg != NULL){
        Rasteron_Image* currentImg = copyImgOp(_outputImg);
        Rasteron_Image* mixerImg = (_savedImg != NULL)? copyImgOp(_savedImg) : noiseImgOp(imgSize, grid); // texImgOp(mode, &grid);

        RASTERON_DEALLOC(_outputImg);
        switch(tolower(key)){
            case 'z': _outputImg = mixingImgOp(currentImg, mixerImg, mult_rgb); break;
            case 'x': _outputImg = mixingImgOp(currentImg, mixerImg, sqroot_colors); break;
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

// Executable Function

int main(int argc, char** argv) {
    _outputImg = solidImgOp((ImageSize) { 1024, 1024 }, 0xFF333333);

    _run(argc, argv, texArgs); // system specific initialization and continuous loop

    // RASTERON_QUEUE_DEALLOC(_mainQueue);
    RASTERON_DEALLOC(_outputImg);

    return 0;
}
