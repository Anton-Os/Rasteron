#define RASTERON_ENABLE_ANIM

#include "Rasteron.h"

#define OCTAVES 3
#define TEXTOOL_COUNT 10

#include "Util_Demo.h"

Rasteron_Image* savedImg = NULL; // always keep a record of last image
static char texMode = 'a';

static float noiseMod(float value){ return (value < 0.25)? 0.0 : (value > 0.75)? 1.0 : 0.5; }
/* static float noiseMod(float value){ 
    for(float n = 0; n < 5; n++) if(value < (1.0 / 5) * n) return (1.0 / 5) * n;
    return 1.0;
} */

static float quiltNoiseMod(float value){ 
	static float m = 0.0;
	float newMod = (value / m > 1.0)? value + (m * 0.5) : value - (m * 0.5);
	m = value;
	return newMod;
}

unsigned powrootMix(unsigned color1, unsigned color2){ return colors_powroot(color1, color2); }

unsigned eqMix(unsigned color1, unsigned color2){
    while(color1 & 0xFFFFFF > 0xFFFFFF - (color2 & 0xFFFFFF)) color1 /= 2;
    return color1;
}

unsigned invertMix(unsigned color1, unsigned color2){ return (0xFFFFFFFF - ((color1 > color2)? color_invert(color1 - color2) : color_invert(color2 - color1))) | 0xFF000000; }

unsigned bitwiseMix_AND(unsigned color1, unsigned color2){ return (color1 & color2) | 0xFF000000; }

unsigned bitwiseMix_OR(unsigned color1, unsigned color2){ return (color1 | color2) | 0xFF000000; }

unsigned wavyMix(unsigned color1, unsigned color2){
    return colors_fuse(color1 + color2, color1 - color2, 0.5);
}

Rasteron_Image* createTex(char t, unsigned m){
    ColorGrid grid = { pow(2, _dimens[0]), pow(2, _dimens[1]),  _swatch.colors[SWATCH_Light],  _swatch.colors[SWATCH_Dark] };

    switch(tolower(t)){
        case 'a': return noiseImgOp_value((ImageSize){ 1024, 1024 }, grid); break;
        case 's': return noiseImgOp_tiled((ImageSize){ 1024, 1024 }, grid); break;
        case 'd': return noiseImgOp_scratch((ImageSize){ 1024, 1024 }, grid); break;
        case 'f': return noiseImgOp_octave((ImageSize){ 1024, 1024 }, grid, OCTAVES); break;
        case 'g': return noiseImgOp_diff((ImageSize){ 1024, 1024 }, grid, OCTAVES); break;
        case 'h': return noiseImgOp_low((ImageSize){ 1024, 1024 }, grid, OCTAVES); break;
        case 'j': return noiseImgOp_hi((ImageSize){ 1024, 1024 }, grid, OCTAVES); break;
        case 'k': Rasteron_Image* image = noiseExtImgOp_value((ImageSize){ 1024, 1024 }, grid, noiseMod); break;
        case 'l': return noiseExtImgOp_value((ImageSize){ 1024, 1024 }, grid, quiltNoiseMod); break;
        default: return noiseImgOp_value((ImageSize){ 1024, 1024 }, grid); break;
    }

}

void _onKeyEvent(char key){ 
    static unsigned mode = 0;

    if(key == '-' && mode != 0) mode--; // decrease size
    else if(key == '=' && mode != TEXTOOL_COUNT - 1) mode++; // increase size
    else if(isspace(key) && _outputImg != NULL) saveToFile(_outputImg);
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
        texMode = tolower(key);
        
    if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
        _outputImg = createTex(texMode, mode);

    if(_outputImg != NULL){
        Rasteron_Image* currentImg = copyImgOp(_outputImg);
        Rasteron_Image* mixerImg = createTex(texMode, mode);

        RASTERON_DEALLOC(_outputImg);
        switch(tolower(key)){
            case 'z': _outputImg = mixingImgOp(currentImg, mixerImg, eqMix); break;
            case 'x': _outputImg = mixingImgOp(currentImg, mixerImg, wavyMix); break;
            case 'c': Rasteron_Image* powrootImg = mixingImgOp(currentImg, mixerImg, powrootMix);
                _outputImg = colorSwitchImgOp(powrootImg, CHANNEL_Red, CHANNEL_Green); break;
                RASTERON_DEALLOC(powrootImg);
            case 'v': _outputImg = warpingImgOp(currentImg, mixerImg); break;
            case 'b': _outputImg = mixingImgOp(currentImg, mixerImg, invertMix); break;
            case 'n': _outputImg = mixingImgOp(currentImg, mixerImg, bitwiseMix_AND); break;
            case 'm': _outputImg = mixingImgOp(currentImg, mixerImg, bitwiseMix_OR); break;
            default: _outputImg = copyImgOp(currentImg); break;
        }
        RASTERON_DEALLOC(currentImg);
        RASTERON_DEALLOC(mixerImg);
    }

    if(savedImg != NULL) RASTERON_DEALLOC(savedImg);
    savedImg = copyImgOp(_outputImg); 
}
void _onPressEvent(double x, double y){}
void _onTickEvent(unsigned secs){}

int main(int argc, char** argv) {
    if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    _outputImg = noiseImgOp_white((ImageSize){ 1024, 1024 }, 0xFF333333, 0xFFEEEEEE);

    _mainQueue = RASTERON_QUEUE_ALLOC("tex", internal_create_size(1024, 1024), TEXTOOL_COUNT);

    _run(argc, argv, NULL); // system specific initialization and continuous loop

    RASTERON_QUEUE_DEALLOC(_mainQueue);
    RASTERON_DEALLOC(_outputImg);

    return 0;
}