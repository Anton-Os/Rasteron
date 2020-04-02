#include "ImageLoader.h"

void loadImage_Targa(const char* fileName, Image* image) {
	image->fileFormat = IMG_Targa;

	FILE* targaFile;
#ifdef _WIN32
	errno_t err = fopen_s(&targaFile, fileName, "rb");
	if (err) {
		printf("Error opening %s fopen_s error code %d", fileName, err);
		return;
	}
#else
	if ((bmpFile = fopen(fileName, "rb") == NULL)) {
		printf("Error opening %s", fileName);
		return;
	}
#endif // _WIN32
	fseek(targaFile, 12, SEEK_SET);
	fread(&image->imageData.targa.width, sizeof(int16_t), 1, targaFile);
	fseek(targaFile, 14, SEEK_SET);
	fread(&image->imageData.targa.height, sizeof(int16_t), 1, targaFile);
	fseek(targaFile, 16, SEEK_SET);
	fread(&image->imageData.targa.depth, sizeof(int8_t), 1, targaFile);

	fseek(targaFile, 0, SEEK_SET);
	fread(&image->imageData.targa.idfield, sizeof(int8_t), 1, targaFile);
	if (image->imageData.targa.idfield != 0) {
		puts("TARGA IMAGE ID FIELD PRESENT!");
		return;
	}
	image->imageData.targa.data = (int32_t*)malloc(abs(image->imageData.targa.width) * abs(image->imageData.targa.height) * (int32_t)sizeof(int32_t));
	fseek(targaFile, 18, SEEK_SET);
	// fseek(targaFile, 100, SEEK_SET);
	// fseek(targaFile, 100, SEEK_SET);
	fread((int32_t*)image->imageData.targa.data, sizeof(int32_t), abs(image->imageData.bmp.width) * abs(image->imageData.targa.height), targaFile);
	
	fclose(targaFile);
	return;
}

void delImage_Targa(Image* image) {
	if (image->fileFormat != IMG_Targa) {
		puts("Image provided for deletion is not TARGA type");
		return;
	}
	free(image->imageData.targa.data);
	return;
}