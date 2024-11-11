#include "catalouge.h"

static int mode = -1;

static char keysave = 'a';

PixelPointTable table;
ColorPointTable colorTable;

static double dotDist = 0.05;

static double c1 = 0.01, c2 = -0.01, c3 = 0.0;
static double d1 = 0.05, d2 = 0.0, d3 = 1.0;

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

static unsigned breakTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    return (pixPoint[0].x + pixPoint[1].y > distances[0] / atan(((pixPoint[0].y + 1.0) * 0.5) / ((pixPoint[0].x + 1.0) * 0.5)))? colors_powroot(colors[2], colors[1]) : colors_powroot(colors[1], colors[2]);
}

static unsigned crossTiling1(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    // return colors_blend(colors[0], colors_blend(colors[1], colors[2], pow(pixPoint[0].x * pixPoint[1].y, distances[0])), distances[2] / distances[1]);
    return colors_blend(colors[0] + colors[1], colors[2] - colors[1], atan(pixPoint[0].x / pixPoint[0].y));
}

static unsigned crossTiling2(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    return colors_blend(colors[0], colors_blend(colors[1], colors[2], sin(distances[0] / distances[1])), cos(distances[2] / distances[1]));
}

static unsigned shineTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    return colors_fuse(colors[0], colors[1], sin(pixPoint[0].x * (1.0 / distances[0]) + pixPoint[0].y * (1.0 / distances[0])));
}

static unsigned zebraTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    unsigned color = colors[0];
    double dist = distances[0] + distances[1] + distances[2];
    if((dist * 10.0) - floor(dist * 10.0) > pow(c1, 1.0 - c2) + sin(c3 * dist)) color = colors[1];
    else if(ceil(dist * 10.0) - (dist * 10.0) > pow(c2, 1.0 - c1) + sin(c3 * dist)) color = colors[2];
    return color;
}

static unsigned amorphTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    unsigned s = (pixPoint[0].x / pixPoint[0].y) / (distances[0] / distances[1] / distances[2]);
    // s = ((s + d1) * pow(s, d2)) / (1.0 / d3);
    // s = ((s + c1) * pow(s, 1.0 + c2)) / (1.0 + c3);
    if(s < 0.0) return colors[0];
    else if(s > 1.0) return colors[1];
    else return colors[2];
}

static unsigned lumenTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]){
    if(distances[0] > c1){
        if(pixPoint[0].x / pixPoint[1].y > c2) return colors_blend(colors[1], colors[2], fabs(tan(pow(distances[0], distances[1] + distances[2]))));
        else return colors_blend(colors[0], colors[1], fabs(tan(distances[1] - distances[2]) * 10.0));
    }
    else return colors_blend(colors[0], colors[1], fabs(tan((distances[0] - distances[1]) * 10.0)));
}


Rasteron_Image* fieldMosaicImgOp(ImageSize size, const ColorPointTable* colorPointTable, fieldCallback3 callback) {
    Rasteron_Image* fieldImage = RASTERON_ALLOC("field", size.height, size.width);

    unsigned* colorPoints = malloc(colorPointTable->pointCount * sizeof(unsigned));
    for (unsigned t = 0; t < colorPointTable->pointCount; t++)
        *(colorPoints + t) = pixPoint_offset((PixelPoint){ colorPointTable->points[t].x + sin(d2 * t), colorPointTable->points[t].y - sin(d2 * t) }, fieldImage);
        // *(colorPoints + t) = pixPoint_offset((PixelPoint){ pow(colorPointTable->points[t].x + sin(d2 * t), d3), pow(colorPointTable->points[t].y - sin(d2 * t), d3) }, fieldImage);

    unsigned pixColors[3] = { NO_COLOR, NO_COLOR, NO_COLOR };
    double pixDistances[3] = { 1.0, 1.0, 1.0 };
    PixelPoint pixPoints[3] = {{ 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 }};

    for (unsigned p = 0; p < fieldImage->width * fieldImage->height; p++) {
        double x = (1.0 / (double)size.width) * (p % size.width);
        double y = (1.0 / (double)size.height) * (p / size.width);

        /* x += d1; y -= d1; // coordinate mod 1
        x *= 1.0 + d2; y /= 1.0 + d2; // coordinate mod 2
        x = pow(x, d3); y = pow(y, d3); // coordinate mod 3 */

        pixDistances[0] = 1.0; pixDistances[1] = 1.0; pixDistances[2] = 1.0; // reset
        for (unsigned t = 0; t < colorPointTable->pointCount; t++) {
            double dist = pix_dist(p, *(colorPoints + t), fieldImage->width) * (1.0 / (double)(fieldImage->width)); // distance multiplied by pixel size
            if (dist < pixDistances[0]) {
                for(unsigned d = 0; d < 3; d++){
                    pixDistances[d] = dist + (d * d1) * (d3 - d2); // experimental change
                    pixPoints[d] = (PixelPoint){
                        (x - colorPointTable->points[t].x) / (1.0 + (d * d1) + sin(d * d2)), // experimental change
                        // pow(x - colorPointTable->points[t].x, 1.0 + (d * d1) + sin(d * d2)), // experimental change
                        (y - colorPointTable->points[t].y) / (1.0 + (d * d1) + cos(d * d3)) // experimental change
                        // pow(y - colorPointTable->points[t].y, 1.0 + (d * d1) + cos(d * d3))
                    };
                }
                pixColors[0] = colorPointTable->points[t].color;
                pixColors[1] = colors_blend(colorPointTable->points[t].color, 0xFF000000, 0.5); // darken color
                pixColors[2] = colors_blend(colorPointTable->points[t].color, 0xFFFFFFFF, 0.5); // lighten color
            }
        }

        *(fieldImage->data + p) = callback(pixColors, pixDistances, pixPoints);
    }

    free(colorPoints);
    return fieldImage;
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
        case 'r': callback = &dotTiling1; break;
        case 't': callback = &dotTiling2; break;
        case 'y': callback = &dotTiling3; break;
        case 'u': callback = &breakTiling; break;
        case 'i': callback = &crossTiling1; break;
        case 'o': callback = &crossTiling2; break;
        case 'p': callback = &shineTiling; break;
        // Complex Tiling Parameters
        /* case 'a': c1 = 0.01; break; case 's': c1 = 0.25; break; case 'd': c1 = 0.5; break;
        case 'f': c2 = 0.01; break; case 'g': c2 = 0.25; break; case 'h': c2 = 0.5; break;
        case 'j': c3 = 0.0; break; case 'k': c3 = 0.25; break; case 'l': c3 = 0.5; break; */
        case 'a': callback = &zebraTiling; break;
        case 's': callback = &amorphTiling; break;
        case 'd': callback = &lumenTiling; break;
        // Deviated Tiling Parameters
        case 'z': d1 = 0.05; d2 = 0.0; d3 = 1.0;
                  c1 = 0.01; c2 = 0.01; c3 = 0.01;
        break;
        case 'x': d1 = 0.01; d2 = 0.01; d3 = 0.99; c1 = 0.25; break;
        case 'c': d1 = 0.1; d2 = -0.01; d3 = 1.01; c2 = 0.25; break;
        case 'v': d1 = 0.05; d2 = 0.1; d3 = 1.1; c3 = 0.25; break;
        case 'b': d1 = 0.005; d2 = -0.1; d3 = 0.9; c1 = 0.5; break;
        case 'n': d1 = 0.25; d2 = 0.5; d3 = 1.5; c2 = 0.5; break;
        case 'm': d1 = -0.05; d2 = -0.5; d3 = 0.5; c3 = 0.5; break;
    }

    if(isalnum(input) && colorTable.pointCount >= 1) {
        if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
        switch(keysave){
            case 'z': case 'x': case 'c': case 'v': case 'b': case 'n': case 'm':
                _outputImg = fieldMosaicImgOp((ImageSize){ 1024, 1024 }, &colorTable, callback); break;
            default: _outputImg = fieldExtImgOp((ImageSize){ 1024, 1024 }, &colorTable, callback); break;
        }


        if(_dimens[0] > 1 || _dimens[1] > 1){
            Rasteron_Image* resizeImg = resizeImgOp((ImageSize){ 1024 / _dimens[1], 1024 / _dimens[0] }, _outputImg);
            // TODO: Perform truschet tiling
            RASTERON_DEALLOC(resizeImg);
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
