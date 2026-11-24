#include "Loader.h"

#ifdef USE_IMG_BMP

Rasteron_Image* loadImgOp_bmp(const char* fileName){
	FILE* bmpFile;
#ifdef _WIN32
	errno_t err = fopen_s(&bmpFile, fileName, "rb");
	if (err) return errorImgOp("Cannot open file");
#else
	bmpFile = fopen(fileName, "rb");
        if (bmpFile == NULL) return NULL; // errorImgOp("Cannot open file");
#endif // _WIN32
	
	uint16_t typeCheck = 0;

	fread(&typeCheck, sizeof(uint16_t), 1, bmpFile);
	if(typeCheck != 0x4D42){ // errorImgOp("Invalid file format");
		fclose(bmpFile);
		return NULL;
	}

	uint32_t offset = 0, compression = 0;
	int32_t height = 0, width = 0;
	uint16_t bitCount = 0;

	fseek(bmpFile, 10, SEEK_SET);
	fread(&offset, sizeof(uint32_t), 1, bmpFile);
	fseek(bmpFile, 18, SEEK_SET);
	fread(&width, sizeof(int32_t), 1, bmpFile);
	fseek(bmpFile, 22, SEEK_SET);
	fread(&height, sizeof(int32_t), 1, bmpFile);
	fseek(bmpFile, 28, SEEK_SET);
	fread(&bitCount, sizeof(uint16_t), 1, bmpFile);
	fseek(bmpFile, 30, SEEK_SET);
	fread(&compression, sizeof(uint32_t), 1, bmpFile);

	if(width <= 0 || height == 0 || (bitCount != 24 && bitCount != 32) || (compression != 0 && compression != 3)){
		fclose(bmpFile);
		return NULL; // only uncompressed 24 and 32 bit files are supported
	}

	unsigned rows = (unsigned)abs(height), cols = (unsigned)width;
	unsigned pixelBytes = bitCount / 8;
	size_t rowBytes = ((((size_t)cols * bitCount) + 31) / 32) * 4; // rows are padded to a multiple of 4 bytes

	Rasteron_Image* bmpImg = RASTERON_ALLOC(fileName, rows, cols);

	fseek(bmpFile, offset, SEEK_SET);
	unsigned char* rowData = (unsigned char*)malloc(rowBytes);
	for(unsigned r = 0; r < rows; r++){
		if(fread(rowData, 1, rowBytes, bmpFile) != rowBytes) memset(rowData, 0, rowBytes); // file is cut short
		uint32_t* dest = bmpImg->data + ((size_t)((height > 0)? rows - 1 - r : r) * cols); // rows are stored bottom to top unless height is negative
		for(unsigned c = 0; c < cols; c++){
			const unsigned char* pixel = rowData + ((size_t)c * pixelBytes); // blue, green, red, alpha
			uint32_t alpha = (pixelBytes == 4)? pixel[3] : 0xFF;
			*(dest + c) = (alpha << 24) | ((uint32_t)pixel[2] << 16) | ((uint32_t)pixel[1] << 8) | pixel[0];
		}
	}

	free(rowData);
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
	unsigned rowPad = (4 - ((width * 3) % 4)) % 4; // rows are padded to a multiple of 4 bytes
	int filesize = 54 + (((width * 3) + rowPad) * height);

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

	for (int r = 0; r < height; r++){
		fwrite(colorBytes + (r * width * 3), 3, width, bmpFile);
		fwrite(padding, 1, rowPad, bmpFile);
	}
	
	free(colorBytes);
	fclose(bmpFile);
}

#endif
