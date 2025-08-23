#include "Loader.h"

#ifdef USE_IMG_BMP

Rasteron_Image* loadImgOp_bmp(const char* fileName){
	FILE* bmpFile;
#ifdef _WIN32
	errno_t err = fopen_s(&bmpFile, fileName, "rb");
	if (err) return errorImgOp("Cannot open file");
#else
	bmpFile = fopen(fileName, "rb");
	if (bmpFile == NULL) return errorImgOp("Cannot open file");
#endif // _WIN32
	
	uint16_t typeCheck;

	fread(&typeCheck, sizeof(uint16_t), 1, bmpFile);
	if(typeCheck != 0x4D42) return errorImgOp("Invalid file format");

	uint32_t offset;
	int32_t height, width;

	fseek(bmpFile, 10, SEEK_SET);
	fread(&offset, sizeof(uint32_t), 1, bmpFile);
	fseek(bmpFile, 18, SEEK_SET);
	fread(&width, sizeof(int32_t), 1, bmpFile);
	fseek(bmpFile, 22, SEEK_SET);
	fread(&height, sizeof(int32_t), 1, bmpFile);

	Rasteron_Image* bmpImg = RASTERON_ALLOC("bmp", (unsigned)abs(height), (unsigned)abs(width));

	fseek(bmpFile, offset, SEEK_SET);
	uint32_t* data = (uint32_t*)malloc(abs(height) * abs(width) * sizeof(uint32_t));
	fread(data, sizeof(uint32_t), abs(height) * abs(width), bmpFile);
	for (unsigned p = 0; p < abs(height) * abs(width); p++) *(bmpImg->data + p) = *(data + p);

	free(data);
	fclose(bmpFile);

	return bmpImg;
}

void writeFileImageRaw_bmp(const char* fileName, unsigned height, unsigned width, unsigned* data){
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

	// int filesize = 54 + (width * height * 3);
	int filesize = 54 + (4 * width * height);

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

#endif
