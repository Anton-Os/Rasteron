#include "Loader.h"

#ifdef USE_IMG_TIFF

void loadFromFile_TIFF(const char* fileName, FileImage* image) {
	image->fileFormat = IMG_Tiff;

	TIFF* tiffFile = TIFFOpen(fileName, "r");
	if (!tiffFile) {
		printf("Could not open file: %s", fileName);
		image = NULL;
		return;
	}

	TIFFGetField(tiffFile, TIFFTAG_IMAGELENGTH, &image->data.tiff.length);
	TIFFGetField(tiffFile, TIFFTAG_IMAGEWIDTH, &image->data.tiff.width);
	TIFFGetField(tiffFile, TIFFTAG_BITSPERSAMPLE, &image->data.tiff.bitsPerSample);
	TIFFGetField(tiffFile, TIFFTAG_SAMPLESPERPIXEL, &image->data.tiff.samplesPerPixel);
	TIFFGetField(tiffFile, TIFFTAG_ORIENTATION, &image->data.tiff.orientation);

	image->data.tiff.data = (uint32*)_TIFFmalloc(image->data.tiff.length * image->data.tiff.width * sizeof(uint32));
	TIFFReadRGBAImageOriented(
		tiffFile,
		image->data.tiff.width,
		image->data.tiff.length,
		image->data.tiff.data,
		ORIENTATION_TOPRIGHT,
		0
	);

	TIFFClose(tiffFile);

	bitSwitchRB(image->data.tiff.data, image->data.tiff.length * image->data.tiff.width); // switch red and blue bits
	
	return;
}

void writeFileImageRaw_TIFF(const char* fileName, unsigned height, unsigned width, unsigned* data){
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

void delFileImage_TIFF(FileImage* image){
	if (image->fileFormat != IMG_Tiff) {
		puts("Image provided for deletion is not TIFF type");
		return;
	}
	_TIFFfree(image->data.tiff.data);
	image->fileFormat = IMG_NonValid;
}

#endif
