#include "PixelPoint.h"

void addPixelPoint(PixelPointTable* table, double xFrac, double yFrac) {
	table->points[table->pointCount].xFrac = xFrac;
	table->points[table->pointCount].yFrac = yFrac;
	table->pointCount++;
}

void addColorPoint(ColorPointTable* table, unsigned color, double xFrac, double yFrac) {
	table->points[table->pointCount].point.xFrac = xFrac;
	table->points[table->pointCount].point.yFrac = yFrac;
	table->points[table->pointCount].color = color;
	table->pointCount++;
}

unsigned getPixOffset(PixelPoint pixPoint, const Rasteron_Image* refImage){
	unsigned xOffset; // clamping X
	if(pixPoint.xFrac <= 0.0) xOffset = 0;
	else if(pixPoint.xFrac >= 1.0) xOffset = refImage->width - 1;
	else xOffset = (unsigned)((double)refImage->width * pixPoint.xFrac);

	unsigned yOffset;
	if(pixPoint.yFrac <= 0.0) yOffset = 0;
	else if(pixPoint.yFrac >= 1.0) yOffset = refImage->height;
	else yOffset = (unsigned)((double)refImage->height * pixPoint.yFrac);

	unsigned pixIndex = (yOffset * refImage->width) + xOffset;
	return pixIndex;
}

unsigned getPixCursorOffset(PixelPoint pixPoint, const Rasteron_Image* refImage){
	double xFrac; // clamping X
	if(pixPoint.xFrac <= -1.0) xFrac = -1.0;
	else if(pixPoint.xFrac >= 1.0) xFrac = 1.0;
	else xFrac = pixPoint.xFrac;

	double yFrac; // clamping Y
	if(pixPoint.yFrac <= -1.0) yFrac = -1.0;
	else if(pixPoint.yFrac >= 1.0) yFrac = 1.0;
	else yFrac = pixPoint.yFrac;
	yFrac *= -1.0; // Y value needs to be flipped

	PixelPoint adjPoint = (PixelPoint){ (xFrac / 2) + 0.5, (yFrac / 2) + 0.5 };
	return getPixOffset(adjPoint, refImage);
}