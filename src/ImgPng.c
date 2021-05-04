#include "Support.h" // Defines USE_IMG_PNG if needed

#ifdef USE_IMG_PNG

#include "Loader.h"

#define DEFAULT_PNG_DISPLAY_EXPO 2.2

void loadImage_PNG(const char* fileName, Image* image) {
	image->fileFormat = IMG_Png;

	FILE* pngFile;
#ifdef _WIN32
	errno_t err = fopen_s(&pngFile, fileName, "rb");
	if (err) {
		printf("Error opening %s fopen_s error code %d", fileName, err);
		return;
	}
#else
	if ((pngFile = fopen(fileName, "rb") == NULL)) {
		printf("Error opening %s", fileName);
		return;
	}
#endif // _WIN32

	unsigned char pngSig[8];
	fread(pngSig, 1, 8, pngFile);
	if (!png_check_sig(pngSig, 8)) {
		puts("Incorrect png signature!");
		return;
	}
	// fclose(pngFile);

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);
#if (PNG_LIBPNG_VER < 10500)
	setjmp(png_ptr->jmpbuf);
#else
	setjmp(*png_set_longjmp_fn(png_ptr, longjmp, sizeof(jmp_buf)));
#endif

	png_init_io(png_ptr, pngFile);
	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr,
		&image->imageData.png.width, &image->imageData.png.height,
		&image->imageData.png.bitDepth, &image->imageData.png.colorType,
		NULL, NULL, NULL);
	image->imageData.png.rowBytes = png_get_rowbytes(png_ptr, info_ptr);

	// Allocate png specific
	image->imageData.png.row_ptrs = (png_byte**)malloc(image->imageData.png.height * sizeof(png_byte*));
	for (unsigned int r = 0; r < image->imageData.png.height; r++)
		image->imageData.png.row_ptrs[r] = (png_byte*)malloc(image->imageData.png.rowBytes);

	png_read_image(png_ptr, image->imageData.png.row_ptrs);
	image->imageData.png.rgbaData = (uint32_t*)malloc(sizeof(uint32_t) * image->imageData.png.height * image->imageData.png.width);

	png_byte* src_ptr;
	uint32_t* dest_ptr = image->imageData.png.rgbaData;
	int testIncrementor = 0;
	for (unsigned int r = 0; r < image->imageData.png.height; r++) {
		src_ptr = image->imageData.png.row_ptrs[r];

		for (unsigned int c = 0; c < image->imageData.png.width; c++) {

			png_byte red = *src_ptr;
			png_byte green = *(++src_ptr);
			png_byte blue = *(++src_ptr);
			png_byte alpha = *(++src_ptr);
			*(dest_ptr) = (alpha << 24) | (red << 16) | (green << 8) | blue;

			dest_ptr++; src_ptr++; testIncrementor++;
		}
	}

	fclose(pngFile);

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	return;

}


void delImage_PNG(Image* image) {
	if (image->fileFormat != IMG_Png) {
		puts("Image provided for deletion is not PNG type");
		return;
	}

	for (unsigned int r = 0; r < image->imageData.png.height; r++)
		free(image->imageData.png.row_ptrs[r]);
	free(image->imageData.png.row_ptrs);

	free(image->imageData.png.rgbaData);
	image->fileFormat = IMG_NonValid;
	return;
}

#endif
