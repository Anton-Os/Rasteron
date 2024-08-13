#define RASTERON_ENABLE_FONT
#define RASTERON_ENABLE_ANIM

#include "Catalouge.h"

static int mode = -1;

static char keysave = 'a';

PixelPointTable table;
ColorPointTable colorTable;

#include "Util_Runner.h"

static unsigned tiling(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]){
    return colors[0]; // TODO: Improve this
}

void setup(char input){
	if(isalnum(input)) {
		if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
		if(isalpha(input)) keysave = (char)tolower(input);
	}
    if(isdigit(input)){
        table.pointCount = 1;
        table.points[0] = (PixelPoint){ 0.0, 0.0 };

        switch(input - '0'){
            case '1': pixPoints_tiling(&table, TILE_Square, _dimens[0], _dimens[1]); break;
            case '2': pixPoints_tiling(&table, TILE_Triangle, _dimens[0], _dimens[1]); break;
            case '3': pixPoints_tiling(&table, TILE_Hexagon, _dimens[0], _dimens[1]); break;
            case '4': pixPoints_tiling(&table, TILE_Diagonal, _dimens[0], _dimens[1]); break;
            case '5': pixPoints_tiling(&table, TILE_Perimiter, _dimens[0], _dimens[1]); break;
            case '6': pixPoionts_expand(&table, 1); break;
            case '7': pixPoionts_expand(&table, 4); break;
            case '9': pixPoionts_expand(&table, 12); break;
        }
    }

    // colorTable.pointCount = table.pointCount;
    // for(unsigned t = 0; t < table.pointCount; t++)colorTable.points[t] = (ColorPoint){ _swatch.colors[t % 8], table.points[t].x, table.points[t].y };

	// switch(keysave){
		// TODO: Generate image
	// }

    if(colorTable.pointCount >= 1)
        _outputImg = fieldImgOp((ImageSize){ 1024, 1024 }, &colorTable, tiling);
}

void _onKeyEvent(char key){ setup(key); }
void _onPressEvent(double x, double y){ }
void _onTickEvent(unsigned secs){}

int main(int argc, char** argv) {
	srand(time(NULL));
	if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    _outputImg = nuTileImgOp(0, 3, 0.0, 0.0);

	_run(argc, argv, NULL); // system specific initialization and continuous loop

    RASTERON_DEALLOC(_outputImg); // cleanup
    return 0;
}