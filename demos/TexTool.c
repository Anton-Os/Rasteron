#define RASTERON_ENABLE_ANIM

#include "Rasteron.h"

#define TEXTOOL_COUNT 10

#include "Util_Demo.h"

ColorGrid grids[TEXTOOL_COUNT];

Rasteron_Image* texImages[TEXTOOL_COUNT];

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

unsigned entwineMix(unsigned color1, unsigned color2){ return colors_entwine(color1, color2); }

unsigned invertMix(unsigned color1, unsigned color2){ return (0xFFFFFFFF - ((color1 > color2)? color_invert(color1 - color2) : color_invert(color2 - color1))) | 0xFF000000; }

unsigned bitwiseMix_AND(unsigned color1, unsigned color2){ return (color1 & color2) | 0xFF000000; }

unsigned bitwiseMix_OR(unsigned color1, unsigned color2){ return (color1 | color2) | 0xFF000000; }


#define OCTAVES 3


Rasteron_Image* createTex(char t, unsigned m){
    switch(tolower(t)){
        case 'a': return noiseImgOp_value((ImageSize){ 1024, 1024 }, grids[m]); break;
        case 's': return noiseImgOp_tiled((ImageSize){ 1024, 1024 }, grids[m]); break;
        case 'd': return noiseImgOp_scratch((ImageSize){ 1024, 1024 }, grids[m]); break;
        case 'f': return noiseImgOp_octave((ImageSize){ 1024, 1024 }, grids[m], OCTAVES); break;
        case 'g': return noiseImgOp_diff((ImageSize){ 1024, 1024 }, grids[m], OCTAVES); break;
        case 'h': return noiseImgOp_low((ImageSize){ 1024, 1024 }, grids[m], OCTAVES); break;
        case 'j': return noiseImgOp_hi((ImageSize){ 1024, 1024 }, grids[m], OCTAVES); break;
        case 'k': Rasteron_Image* image = noiseExtImgOp_value((ImageSize){ 1024, 1024 }, grids[m], noiseMod); break;
        case 'l': return noiseExtImgOp_value((ImageSize){ 1024, 1024 }, grids[m], quiltNoiseMod); break;
        default: return noiseImgOp_value((ImageSize){ 1024, 1024 }, grids[m]); break;
    }

}

void _onKeyEvent(char key){ 
    static unsigned mode = 0;

    if(key - '0' >= 0 && key - '0' < TEXTOOL_COUNT) mode = key - '0';
    else if(tolower(key) == 'q' || tolower(key) == 'w' || tolower(key) == 'e' || tolower(key) == 'r' || tolower(key) == 't' || tolower(key) == 'y' || tolower(key) == 'u' || tolower(key) == 'i' || tolower(key) == 'o' || tolower(key) == 'p'){
        switch(tolower(key)){
            case 'w': grids[mode].color1 = 0xFF0000FF; grids[mode].color2 = 0xFFFF0000; break;
            case 'e': grids[mode].color1 = 0xFFff9cd2; grids[mode].color2 = 0xFF0b6485; break;
            case 'r': grids[mode].color1 = 0xFFFF00FF; grids[mode].color2 = 0xFF00FF00; break;
            case 't': grids[mode].color1 = 0xFFffbf00; grids[mode].color2 = 0xFF381c0a; break;
            case 'y': grids[mode].color1 = 0xFF66fff7; grids[mode].color2 = 0xFFc4ff66; break; // gppd fpr emtwome algo
            case 'u': grids[mode].color1 = 0xFF091796; grids[mode].color2 = 0xFFEEEEEE; break;
            case 'i': grids[mode].color1 = 0xFF81ff38; grids[mode].color2 = 0xFF888888; break;
            case 'o': grids[mode].color1 = 0xFFff6369; grids[mode].color2 = 0xFF111111; break;
            case 'p': grids[mode].color1 = RAND_COLOR(); grids[mode].color2 = RAND_COLOR(); break;
            default: grids[mode].color1 = 0xFF333333; grids[mode].color2 = 0xFFEEEEEE; break;
        }
    }

    if(tolower(key) == 'a' || tolower(key) == 's' || tolower(key) == 'd' || tolower(key) == 'f' || tolower(key) == 'g' || tolower(key) == 'h' || tolower(key) == 'j' || tolower(key) == 'k' || tolower(key) == 'l' || tolower(key) == 'p')
        texMode = tolower(key);
        
    if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    _outputImg = createTex(texMode, mode);

    if(_outputImg != NULL){
        Rasteron_Image* savedImg = copyImgOp(_outputImg);
        Rasteron_Image* mixinImg = createTex(texMode, mode);

        RASTERON_DEALLOC(_outputImg);
        switch(tolower(key)){
            case 'z': _outputImg = blendImgOp(savedImg, mixinImg); break;
            case 'x': _outputImg = fusionImgOp(savedImg, mixinImg); break;
            case 'c': _outputImg = warpingImgOp(savedImg, mixinImg); break;
            case 'v': _outputImg = mixingImgOp(savedImg, mixinImg, entwineMix); break;
            case 'b': _outputImg = mixingImgOp(savedImg, mixinImg, invertMix); break;
            case 'n': _outputImg = mixingImgOp(savedImg, mixinImg, bitwiseMix_AND); break;
            case 'm': _outputImg = mixingImgOp(savedImg, mixinImg, bitwiseMix_OR); break;
            default: _outputImg = copyImgOp(savedImg); break;
        }
        RASTERON_DEALLOC(savedImg);
        RASTERON_DEALLOC(mixinImg);
    }
}
void _onPressEvent(double x, double y){}
void _onTickEvent(unsigned secs){
    _mainQueue->index = secs % TEXTOOL_COUNT;
    // if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    // _outputImg = queue_getImg(_mainQueue, _mainQueue->index);
}

int main(int argc, char** argv) {
    if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    _outputImg = noiseImgOp_white((ImageSize){ 1024, 1024 }, 0xFF333333, 0xFFEEEEEE);

    grids[1] = (ColorGrid){ 2, 2, 0xFF333333, 0xFFEEEEEE };
    grids[2] = (ColorGrid){ 4, 8, 0xFF333333, 0xFFEEEEEE };
    grids[3] = (ColorGrid){ 8, 4, 0xFF333333, 0xFFEEEEEE };
    grids[4] = (ColorGrid){ 16, 16, 0xFF333333, 0xFFEEEEEE }; 
    grids[5] = (ColorGrid){ 16, 32, 0xFF333333, 0xFFEEEEEE };
    grids[6] = (ColorGrid){ 32, 16, 0xFF333333, 0xFFEEEEEE };
    grids[7] = (ColorGrid){ 64, 64, 0xFF333333, 0xFFEEEEEE };
    grids[8] = (ColorGrid){ 128, 128, 0xFF333333, 0xFFEEEEEE };
    grids[9] = (ColorGrid){ 256, 256, 0xFF333333, 0xFFEEEEEE };
    grids[0] = (ColorGrid){ 512, 512, 0xFF333333, 0xFFEEEEEE };


    _mainQueue = RASTERON_QUEUE_ALLOC("tex", createImgSize(1024, 1024), TEXTOOL_COUNT);

    _run(); // system specific initialization and continuous loop

    RASTERON_QUEUE_DEALLOC(_mainQueue);
    RASTERON_DEALLOC(_outputImg);
    for(unsigned n = 0; n < TEXTOOL_COUNT; n++) RASTERON_DEALLOC(texImages[n]);

    return 0;
}