#include "Catalouge.h"

static int mode = -1;

static char keysave = 'a';

PixelPointTable table;
ColorPointTable colorTable;

static double dotDist = 0.05;

#include "Util_Runner.h"

static unsigned eqTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    return colors_blend(colors[0], colors_blend(colors[1], colors[2], fabs(distances[0] - distances[2])), fabs(distances[0] - distances[1]));
}

static unsigned softTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    return colors_blend(colors[0], color_invert(colors[0]), fabs(pixPoint[0].x / pixPoint[0].y));
}

static unsigned hardTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    return (distances[0] / distances[1] > pixPoint[0].x / pixPoint[1].y)? colors[0] : colors[1];
}

static unsigned dotTiling1(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    return (distances[0] < dotDist)? colors[0] : colors[1];
}

static unsigned dotTiling2(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    return (distances[0] / distances[1] * distances[2] < dotDist)? colors[0] : colors[1];
}

static unsigned dotTiling3(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    return (distances[0] + distances[1] - distances[2] < dotDist)? colors[0] : colors[1];
}

static trialTiling1(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    return (pixPoint[0].x + pixPoint[1].y > distances[0] / distances[2])? colors[0] : colors_powroot(colors[1], colors[2]);
}

static trialTiling2(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    // return colors_blend(colors[0], colors_blend(colors[1], colors[2], pow(pixPoint[0].x * pixPoint[1].y, distances[0])), distances[2] / distances[1]);
    return colors_blend(colors[0] + colors[1], colors[2] - colors[1], pixPoint[0].x / pixPoint[0].y);
}

static trialTiling3(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    return colors_blend(colors[0], colors_blend(colors[1], colors[2], sin(distances[0] / distances[1])), cos(distances[2] / distances[1]));
}

void setup(char input){
    static fieldCallback3 callback = &eqTiling;

    if(isdigit(input))
        switch(input - '0'){
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
    else if(isalpha(input)) keysave = (char)tolower(input);

    // colorTable.pointCount = table.pointCount;
    colorTable.pointCount = table.pointCount;
    for(unsigned t = 0; t < colorTable.pointCount; t++)
        colorTable.points[t] = (ColorPoint){ RAND_COLOR(), table.points[t].x, table.points[t].y };

	switch(keysave){
        case 'q': callback = &eqTiling; break;
        case 'w': callback = &softTiling; break;
        case 'e': callback = &hardTiling; break;
        case 'a': callback = &dotTiling1; break;
        case 's': callback = &dotTiling2; break;
        case 'd': callback = &dotTiling3; break;
        case 'z': callback = &trialTiling1; break;
        case 'x': callback = &trialTiling2; break;
        case 'c': callback = &trialTiling3; break;    
	}

    if(isalnum(input) && colorTable.pointCount >= 1) {
        if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
        // _outputImg = fieldImgOp((ImageSize){ 1024, 1024 }, &colorTable, tiling);
        _outputImg = fieldExtImgOp((ImageSize){ 1024, 1024 }, &colorTable, callback);
        if(_dimens[0] > 1 || _dimens[1] > 1){
            // TODO: Perform truschet tiling
        } 
    }
}

void _onKeyEvent(char key){ setup(key); }
void _onPressEvent(double x, double y){ }
void _onTickEvent(unsigned secs){}

int main(int argc, char** argv) {
	srand(time(NULL));
	if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    _outputImg = nuTileImgOp(0, 3, 0.0, 0.0);

    table.pointCount = 1;
    table.points[0] = (PixelPoint){ 0.0, 0.0 };
    colorTable.pointCount = 1;
    colorTable.points[0] = (ColorPoint){ RAND_COLOR(), 0.0, 0.0 };

	_run(argc, argv, NULL); // system specific initialization and continuous loop

    RASTERON_DEALLOC(_outputImg); // cleanup
    return 0;
}