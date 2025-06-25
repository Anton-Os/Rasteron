static int mode = -1;

static char keysave = 'a';

static double dotDist = 0.05;

static double c1 = 0.01, c2 = -0.01, c3 = 0.0;
static double d1 = 0.05, d2 = 0.0, d3 = 1.0;

#include "../_Prog.h"

PixelPointTable table;
ColorPointTable colorTable;

#include "Tile.c"

// Overriden Functions

void _onKeyEvent(char key){ 
    static fieldCallback3 callback = &eqTiling;

    if(isdigit(key))
        switch(key - '0'){
            case 1: pixPoints_tiling(&table, TILE_Square, _dimens[0], _dimens[1]); break;
            case 2: pixPoints_tiling(&table, TILE_Triangle, _dimens[0], _dimens[1]); break;
            case 3: pixPoints_tiling(&table, TILE_Hexagon, _dimens[0], _dimens[1]); break;
            case 4: pixPoints_tiling(&table, TILE_Diagonal, _dimens[0], _dimens[1]); break;
            case 5: pixPoints_tiling(&table, TILE_Perimiter, _dimens[0], _dimens[1]); break;
            case 6: pixPoints_tiling(&table, -1, _dimens[0], _dimens[1]); break;
            case 7: pixPoionts_expand(&table, 1); break;
            case 8: pixPoionts_expand(&table, 4); break;
            case 9: pixPoionts_expand(&table, 12); break;
        }
    else if(isalpha(key)) keysave = (char)tolower(key);

    // colorTable.pointCount = table.pointCount;
    colorTable.pointCount = table.pointCount;
    for(unsigned t = 0; t < colorTable.pointCount; t++)
        colorTable.points[t] = (ColorPoint){ RAND_COLOR(), table.points[t].x, table.points[t].y };

    switch(keysave){
        // Basic Tiling
        case 'q': callback = &eqTiling; break;
        case 'w': callback = &softTiling; break;
        case 'e': callback = &hardTiling; break;
        case 'r': callback = &dotTiling1; break;
        case 't': callback = &dotTiling2; break;
        case 'y': callback = &dotTiling3; break;
        case 'u': callback = &breakTiling; break;
        case 'i': callback = &crossTiling1; break;
        case 'o': callback = &crossTiling2; break;
        case 'p': callback = &shineTiling; break;
        // Complex Tiling
        case 'a': callback = &lumenTiling; break;
        case 's': callback = &flashTiling; break;
        case 'd': callback = &focalTiling; break;
        case 'f': callback = &amorphTiling; break;
        case 'g': callback = &zebraTiling; break;
        case 'h': callback = &surroundTiling; break;
        case 'j': callback = &complexTiling; c1 = 0.01; c2 = 0.01; c3 = 0.0; break;
        case 'k': callback = &complexTiling; c1 = 0.15; c2 = -0.15; c3 = 0.5; break;
        case 'l': callback = &complexTiling; c1 = 0.25; c2 = -0.25; c3 = 1.0; break;
        // Deviated Tiling
        case 'z': d1 = 0.05; d2 = 0.0; d3 = 1.0; break;
        case 'x': d1 = 0.01; d2 = 0.01; d3 = 0.99; break;
        case 'c': d1 = -0.1; d2 = -0.01; d3 = 1.01; break;
        case 'v': d1 = 0.05; d2 = 0.1; d3 = 1.1; break;
        case 'b': d1 = 0.005; d2 = -0.1; d3 = 0.9; break;
        case 'n': d1 = -0.25; d2 = 0.5; d3 = 1.5; break;
        case 'm': d1 = -0.05; d2 = -0.5; d3 = 0.5; break;
    }

    if(isalnum(key) && colorTable.pointCount >= 1) {
        if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
        switch(keysave){
            case 'z': case 'x': case 'c': case 'v': case 'b': case 'n': case 'm':
                _outputImg = mosaicImgOp((ImageSize){ 1024, 1024 }, &colorTable, callback); break;
            default: _outputImg = fieldExtImgOp((ImageSize){ 1024, 1024 }, &colorTable, callback); break;
        }
    } else if(_outputImg != NULL && key == ';'){
        puts("Generating truschet image");
        Rasteron_Image* tempImg = copyImgOp(_outputImg);
        RASTERON_DEALLOC(_outputImg);
        _outputImg = truschetImgOp(tempImg, _dimens[0], _dimens[1]);
    }
}
void _onPressEvent(double x, double y){ }
void _onTickEvent(unsigned secs){}

// Generative Function

Rasteron_Image* tileTool(int argc, char** argv){
    // Parse the input and generate appropriate image
    return mosaicImgOp((ImageSize){ 1024, 1024 }, &colorTable, &eqTiling);
}

// Executable Function

int main(int argc, char** argv) {
    table.pointCount = 1;
    table.points[0] = (PixelPoint){ 0.0, 0.0 };
    colorTable.pointCount = 1;
    colorTable.points[0] = (ColorPoint){ RAND_COLOR(), 0.0, 0.0 };

    _run(argc, argv, tileTool); // system specific initialization and continuous loop

    RASTERON_DEALLOC(_outputImg); // cleanup
    return 0;
}
