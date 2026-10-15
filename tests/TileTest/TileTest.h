#include "Rasteron.h"

extern double xArg;
extern double yArg;
extern int mode;

// Tiling Functions

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

// Truschet Functions

unsigned sharpTruschetTile(double x, double y);

// Image Functions

Rasteron_Image* mosaicImgOp(ImageSize size, const ColorPointTable* colorPointTable, fieldCallback3 callback);
Rasteron_Image* truschetImgOp(ref_image_t truschetImg, unsigned short wDiv, unsigned short hDiv);