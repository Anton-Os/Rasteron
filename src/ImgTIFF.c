#include "ImageLoader.h"

void loadImage_TIFF(const char* fileName, Image* image) {
	image->fileFormat = IMG_Tiff;
	TIFF** tiff = &image->imageData.tiff.tiff; // For less typing, points to tiff file type

	*(tiff) = TIFFOpen(fileName, "r");
	if (!*(tiff)) {
		printf("Could not open file: %s", fileName);
		return;
	}
	TIFFGetField(*(tiff), TIFFTAG_IMAGELENGTH, &image->imageData.tiff.length);
	TIFFGetField(*(tiff), TIFFTAG_IMAGEWIDTH, &image->imageData.tiff.width);
	TIFFGetField(*(tiff), TIFFTAG_BITSPERSAMPLE, &image->imageData.tiff.bitsPerSample);
	TIFFGetField(*(tiff), TIFFTAG_SAMPLESPERPIXEL, &image->imageData.tiff.samplesPerPixel);
	TIFFGetField(*(tiff), TIFFTAG_ORIENTATION, &image->imageData.tiff.orientation);

	image->imageData.tiff.raster = (uint32*)_TIFFmalloc(image->imageData.tiff.length * image->imageData.tiff.width * sizeof(uint32));
	TIFFReadRGBAImageOriented(
		image->imageData.tiff.tiff,
		image->imageData.tiff.width,
		image->imageData.tiff.length,
		image->imageData.tiff.raster,
		ORIENTATION_TOPRIGHT,
		0);
	TIFFClose(*(tiff));
	return;
}

void delImage_TIFF(Image* image){
	if (image->fileFormat != IMG_Tiff) {
		puts("Image provided for deletion is not TIFF type");
		return;
	}
	_TIFFfree(image->imageData.tiff.raster);
	image->fileFormat = IMG_NonValid;
	return;
}