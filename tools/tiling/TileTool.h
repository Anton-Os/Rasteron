#include "../Tool.h"

// --------------------------------   Tiling State    -------------------------------- //

enum TILE_Algorithm { ALGO_Field = 0, ALGO_Mosaic = 1, ALGO_Voronoi = 2, ALGO_Truchet = 3, ALGO_Penrose = 4, ALGO_Checker = 5, ALGO_Lined = 6 };
enum TILE_Points { POINTS_Square = 0, POINTS_Triangle = 1, POINTS_Hexagon = 2, POINTS_Diagonal = 3, POINTS_Perimeter = 4, POINTS_Random = 5, POINTS_Spiral = 6, POINTS_Circle = 7 };
enum TILE_Distance { DIST_Euclid = 0, DIST_Manhattan = 1, DIST_Chebyshev = 2, DIST_Minkowski = 3, DIST_Octagonal = 4, DIST_Hexagonal = 5 };
enum TILE_Shading { SHADE_Flat = 0, SHADE_Shaded = 1, SHADE_Edges = 2, SHADE_Stained = 3, SHADE_Ridges = 4 };
enum TILE_Palette { PALETTE_Random = 0, PALETTE_Gradient = 1, PALETTE_Two = 2, PALETTE_Swatch = 3, PALETTE_Position = 4 };
enum TILE_Truchet { TRUCHET_Sharp = 0, TRUCHET_Arcs = 1, TRUCHET_Diagonals = 2, TRUCHET_Triangles = 3, TRUCHET_Applied = 4 };
enum TILE_Mix { TMIX_None = 0, TMIX_Blend = 1, TMIX_Xor = 2, TMIX_Mult = 3, TMIX_Add = 4, TMIX_Diff = 5 };

typedef struct {
    int algorithm;
    int callback; // TileTest field callbacks used by field and mosaic
    int points; // pixel point arrangement
    int reps[2]; // repetitions across x y
    int count; // points for random, spiral, and circle arrangements
    int expand; // extra points between neighbors (pixPoionts_expand)
    double jitter; // random displacement of each point
    double phase; // orbits points and turns penrose, animated by frames/encode
    int distance; // distance equation
    double exponent; // minkowski exponent
    int shading; // voronoi shading
    double edge; // edge width
    int palette;
    unsigned colors[3]; // palette colors, colors[2] is the edge color
    double xarg, yarg; int mode; // TileTest callback arguments
    int truchet; // truchet tile style
    int subdivisions; // penrose subdivisions
    double zoom;
    int mix; // combination with applied image
} Tile_State;

extern Tile_State _tile;

extern double xArg; // used by TileTest callbacks, synced from _tile
extern double yArg;
extern int mode;

// --------------------------------   TileTest Callbacks (see Tile.c)    -------------------------------- //

unsigned eqTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]);
unsigned softTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]);
unsigned hardTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]);
unsigned dotTiling1(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]);
unsigned dotTiling2(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]);
unsigned dotTiling3(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]);
unsigned stripeTiling1(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]);
unsigned stripeTiling2(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]);
unsigned stripeTiling3(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]);
unsigned breakTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]);
unsigned shineTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]);
unsigned lumenTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]);
unsigned flashTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]);
unsigned amorphTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]);
unsigned focalTiling(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]);
unsigned linedTiling1(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]);
unsigned linedTiling2(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]);
unsigned complexTiling1(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]);
unsigned complexTiling2(unsigned colors[3], double distances[3], PixelPoint pixPoint[3]);

unsigned sharpTruschetTile(double x, double y);

// --------------------------------   Image Operations (see Tile.c)    -------------------------------- //

void tilePoints(PixelPointTable* table); // arranges points from _tile settings
void tileColors(ColorPointTable* colorTable, const PixelPointTable* table); // colors points from the palette
double tileDistance(double dx, double dy); // distance equation from _tile settings

Rasteron_Image* mosaicImgOp(ImageSize size, const ColorPointTable* colorPointTable, fieldCallback3 callback); // TileTest
Rasteron_Image* tileFieldImgOp(ImageSize size, const ColorPointTable* colorPointTable, fieldCallback3 callback); // field with distance equations
Rasteron_Image* tileVoronoiImgOp(ImageSize size, const ColorPointTable* colorPointTable); // voronoi with shading
Rasteron_Image* truschetImgOp(ImageSize size, ref_image_t truschetImg, unsigned short wDiv, unsigned short hDiv); // TileTest
Rasteron_Image* truchetRandomImgOp(ImageSize size, int style, unsigned short wDiv, unsigned short hDiv); // arcs, diagonals, triangles
Rasteron_Image* penroseImgOp(ImageSize size, unsigned short subdivisions); // P3 rhombus tiling
