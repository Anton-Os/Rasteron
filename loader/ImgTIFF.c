#include "Loader.h"

#ifdef USE_IMG_TIFF

Rasteron_Image* loadImgOp_tiff(const char* fileName){
	TIFF* tiffFile = TIFFOpen(fileName, "r");
	if (!tiffFile) errorImgOp("Cannot open file");

	uint16 compression, orientation;
	uint32 width, length;
	uint16 bitsPerSample, samplesPerPixel;

	TIFFGetField(tiffFile, TIFFTAG_IMAGELENGTH, &length);
	TIFFGetField(tiffFile, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(tiffFile, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
	TIFFGetField(tiffFile, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
	TIFFGetField(tiffFile, TIFFTAG_ORIENTATION, &orientation);

	Rasteron_Image* tiffImg = RASTERON_ALLOC("tiff", length, width);

	TIFFReadRGBAImageOriented(
		tiffFile,
		tiffImg->width,
		tiffImg->height,
		tiffImg->data,
		ORIENTATION_TOPLEFT,
		0
	);

	TIFFClose(tiffFile);

	return tiffImg;
}

void writeFileImageRaw_tiff(const char* fileName, unsigned height, unsigned width, unsigned* data){
	TIFF* tiffFile = TIFFOpen(fileName, "w");
	if (!tiffFile) {
		printf("Could not open file: %s", fileName);
		return;
	}

	// Writing Meta-Data

	TIFFSetField(tiffFile, TIFFTAG_IMAGEWIDTH, width);
	TIFFSetField(tiffFile, TIFFTAG_IMAGELENGTH, height);
	TIFFSetField(tiffFile, TIFFTAG_SAMPLESPERPIXEL, 4);
	TIFFSetField(tiffFile, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(tiffFile, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(tiffFile, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(tiffFile, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
	TIFFSetField(tiffFile, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tiffFile, width * 4));

	// Flipping Color Bits

	unsigned* dataflip = (unsigned*)malloc(sizeof(unsigned) * height * width); // for flipping color values
	for(unsigned p = 0; p < width * height; p++)
		*(dataflip + p) = (*(data + p) & 0xFF000000) + ((*(data + p) & 0xFF) << 16) + (*(data + p) & 0xFF00) + ((*(data + p) >> 16) & 0xFF);

	// Writing Data

	unsigned char* colorBytes = (unsigned char*)malloc(width * 4);

	for (unsigned r = 0; r < height; r++) { // copying data
		memcpy(colorBytes, dataflip + (r * width), width * 4);
		if (TIFFWriteScanline(tiffFile, colorBytes, r, 0) < 0) break;
	}

	free(colorBytes);
	free(dataflip);

	TIFFClose(tiffFile);
}

#endif
