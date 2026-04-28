static int mode = -1;

static char keysave = 'a';

static double dotDist = 0.05;

#include "../_Tool.h"

PixelPointTable table;
ColorPointTable colorTable;

#include "Tile.c"

// Overriden Functions

void _onKeyEvent(char key){ 
    static fieldCallback3 callback = &eqTiling;
    static coordCallback coordCallback = NULL;

    if(isalpha(key)) keysave = (char)tolower(key);

    // colorTable.pointCount = table.pointCount;
    colorTable.pointCount = table.pointCount;
    for(unsigned t = 0; t < colorTable.pointCount; t++)
        colorTable.points[t] = (ColorPoint){ RAND_COLOR(), table.points[t].x, table.points[t].y };

    switch(keysave){
        case 'q': pixPoints_tiling(&table, TILE_Square, _dimens[0], _dimens[1]); break;
        case 'w': pixPoints_tiling(&table, TILE_Triangle, _dimens[0], _dimens[1]); break;
        case 'e': pixPoints_tiling(&table, TILE_Hexagon, _dimens[0], _dimens[1]); break;
        case 'r': pixPoints_tiling(&table, TILE_Diagonal, _dimens[0], _dimens[1]); break;
        case 't': pixPoints_tiling(&table, TILE_Perimiter, _dimens[0], _dimens[1]); break;
        case 'y': pixPoints_tiling(&table, -1, _dimens[0], _dimens[1]); break;
        case 'u': pixPoionts_expand(&table, 1); break;
        case 'i': pixPoionts_expand(&table, 2); break;
        case 'o': pixPoionts_expand(&table, 4); break;
        case 'p': pixPoionts_expand(&table, 8); break;
        // Basic Tiling
        case 'a': callback = &eqTiling; break;
        case 's': callback = &softTiling; break;
        case 'd': callback = &hardTiling; break;
        case 'f': callback = &dotTiling1; break;
        // case 't': callback = &dotTiling2; break;
        case 'g': callback = &dotTiling3; break;
        case 'h': callback = &lumenTiling; break;
        case 'j': callback = &flashTiling; break;
        case 'k': callback = &amorphTiling; break;
        case 'l': callback = &focalTiling; break; // &shineTiling; break;
        // Complex Tiling
        case 'z': callback = &stripeTiling1; break;
        case 'x': callback = &stripeTiling2; break;
        // case 'c': callback = &stripeTiling3; break;
        case 'c': callback = &breakTiling; break;
        case 'v': callback = &linedTiling1; break;
        case 'b': callback = &linedTiling2; break;
        // case 'j': callback = &complexTiling; c1 = 0.01; c2 = 0.01; c3 = 0.0; break;
        case 'n': callback = &complexTiling1; break;
        case 'm': callback = &complexTiling2; break;
    }

    if(isalnum(key) && colorTable.pointCount >= 1) {
        if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);

        if(KEYS_BOT_ROW(key)) _outputImg = mosaicImgOp((ImageSize) { 1024, 1024 }, &colorTable, callback);
        else _outputImg = fieldExtImgOp((ImageSize) { 1024, 1024 }, & colorTable, callback);
    } else if(_outputImg != NULL && key == ','){
        puts("Generating truschet image");
        Rasteron_Image* tempImg = copyImgOp(_outputImg);
        RASTERON_DEALLOC(_outputImg);
        _outputImg = truschetImgOp(tempImg, _dimens[0], _dimens[1]);
    }
}
void _onPressEvent(double x, double y){ }
void _onTickEvent(unsigned secs){}

// Generative Function

Rasteron_Image* tileTool(char* args){
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
