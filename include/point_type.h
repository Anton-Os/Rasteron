#ifndef RASTERON_PIXELPOINT_H

#include "Image.h"

#define MAX_PIXEL_POS TWOPOWER(11) // 2046

typedef struct {
	double xFrac; // fraction of image along the x axis
	double yFrac; // fraction of image along the y axis
} PixelPoint;

typedef struct {
	PixelPoint points[MAX_PIXEL_POS];
	unsigned pointCount; // = 0;
} PixelPointTable;

typedef struct {
	PixelPoint point;
	unsigned color;
} ColorPoint;

typedef struct {
	ColorPoint points[MAX_PIXEL_POS];
	unsigned pointCount; // = 0;
} ColorPointTable;

void addPixelPoint(PixelPointTable* table, double xFrac, double yFrac);
void addColorPoint(ColorPointTable* table, unsigned color, double xFrac, double yFrac);

unsigned getPixOffset(PixelPoint pixPos, ref_image_t refImage); // gets pixel offset from image corner
unsigned getPixCursorOffset(PixelPoint pixPos, ref_image_t refImage); // gets pixel offset from cursor window position

#define RASTERON_PIXELPOINT_H
#endif