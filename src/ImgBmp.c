#include "Loader.h"

#ifdef USE_IMG_BMP

void loadFileImage_BMP(const char* fileName, Image* image) {
	image->fileFormat = IMG_Bmp;

	FILE* bmpFile;
#ifdef _WIN32
	errno_t err = fopen_s(&bmpFile, fileName, "rb");
	if (err) {
		printf("Error opening %s fopen_s error code %d", fileName, err);
		image = NULL;
		return;
	}
#else
	bmpFile = fopen(fileName, "rb");
	if (bmpFile == NULL) {
		printf("Error opening %s", fileName);
		image = NULL;
		return;
	}
#endif // _WIN32

	fread(&image->data.bmp.typeCheck, sizeof(uint16_t), 1, bmpFile);
	if (image->data.bmp.typeCheck != 0x4D42) {
		printf("%s is not a valid BMP file", fileName);
		return;
	}

	// checkout https://solarianprogrammer.com/2018/11/19/cpp-reading-writing-bmp-images/
	// Reading Meta-Data

	fseek(bmpFile, 10, SEEK_SET);
	fread(&image->data.bmp.offset, sizeof(uint32_t), 1, bmpFile);

	fseek(bmpFile, 18, SEEK_SET);
	fread(&image->data.bmp.width, sizeof(int32_t), 1, bmpFile);
	fseek(bmpFile, 22, SEEK_SET);
	fread(&image->data.bmp.height, sizeof(int32_t), 1, bmpFile);

	// Reading Data

	image->data.bmp.data = (uint32_t*)malloc(abs(image->data.bmp.width) * abs(image->data.bmp.height) * (uint32_t)sizeof(uint32_t));
	fseek(bmpFile, image->data.bmp.offset, SEEK_SET);
	fread((uint32_t*)image->data.bmp.data, sizeof(uint32_t), abs(image->data.bmp.width) * abs(image->data.bmp.height), bmpFile);

	fclose(bmpFile);
	return;
}

void writeFileImageRaw_BMP(const char* fileName, unsigned height, unsigned width, unsigned* data){
	FILE* bmpFile;
#ifdef _WIN32
	errno_t err = fopen_s(&bmpFile, fileName, "wb");
	if (err) {
		printf("Error opening %s fopen_s error code %d", fileName, err);
		return;
	}
#else
	bmpFile = fopen(fileName, "wb");
	if(bmpFile == NULL) {
		printf("Error opening %s", fileName);
		return;
	}
#endif

	// https://stackoverflow.com/questions/2654480/writing-bmp-image-in-pure-c-c-without-other-libraries
	// Writing Meta-Data

	int filesize = 54 + (width * height * 3);
	// int filesize = 54 + (4 * width * height);

	unsigned char bmpFileHeader[14] = { 'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0 };
	unsigned char bmpInfoHeader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0 };
	unsigned char padding[3] = { 0,0,0 };

	bmpFileHeader[2] = (unsigned char)(filesize); // filesize
	bmpFileHeader[3] = (unsigned char)(filesize >> 8);
	bmpFileHeader[4] = (unsigned char)(filesize >> 16);
	bmpFileHeader[5] = (unsigned char)(filesize >> 24);
	bmpInfoHeader[4] = (unsigned char)(width); // width
	bmpInfoHeader[5] = (unsigned char)(width >> 8);
	bmpInfoHeader[6] = (unsigned char)(width >> 16);
	bmpInfoHeader[7] = (unsigned char)(width >> 24);
	bmpInfoHeader[8] = (unsigned char)(height); // height
	bmpInfoHeader[9] = (unsigned char)(height >> 8);
	bmpInfoHeader[10] = (unsigned char)(height >> 16);
	bmpInfoHeader[11] = (unsigned char)(height >> 24);

	fwrite(bmpFileHeader, 1, 14, bmpFile);
	fwrite(bmpInfoHeader, 1, 40, bmpFile);

	// Writing Data

	unsigned char* colorBytes = malloc(width * height * 3);
	for(unsigned r = 0; r < height; r++)
		for(unsigned c = 0; c < width; c++){
			unsigned destOffset = (r * width) + c;
			unsigned srcOffset = (height * width) - ((r + 1) * width) + c;
			*(colorBytes + (destOffset * 3)) = *(data + srcOffset);
			*(colorBytes + (destOffset * 3) + 1) = *(data + srcOffset) >> 8;
			*(colorBytes + (destOffset * 3) + 2) = *(data + srcOffset) >> 16;
		}

	for (int r = 0; r < height; r++)
		fwrite(colorBytes + (r * width * 3), 3, width, bmpFile);
		// fwrite(padding, 1, (4 - (width * 3) % 4) % 4, bmpFile);
	
	free(colorBytes);
	fclose(bmpFile);
}

void delFileImage_BMP(Image* image) {
	if (image->fileFormat != IMG_Bmp) {
		puts("Image provided for deletion is not BMP type!");
		return;
	}
	free(image->data.bmp.data);
	image->fileFormat = IMG_NonValid;
}

#endif
