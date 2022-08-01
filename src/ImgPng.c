#include "Loader.h"

#ifdef USE_IMG_PNG

#define DEFAULT_PNG_DISPLAY_EXPO 2.2

void loadFileImage_PNG(const char* fileName, Image* image) {
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

	png_get_IHDR(
		png_ptr, info_ptr,
		&image->data.png.width, &image->data.png.height,
		&image->data.png.bitDepth, &image->data.png.colorType,
		NULL, NULL, NULL
	);
	image->data.png.rowBytesCount = png_get_rowbytes(png_ptr, info_ptr);

	// Allocate png specific data
	image->data.png.row_ptrs = (png_byte**)malloc(image->data.png.height * sizeof(png_byte*));
	for (unsigned int r = 0; r < image->data.png.height; r++)
		image->data.png.row_ptrs[r] = (png_byte*)malloc(image->data.png.rowBytesCount);

	png_read_image(png_ptr, image->data.png.row_ptrs);
	image->data.png.rgbaData = (uint32_t*)malloc(sizeof(uint32_t) * image->data.png.height * image->data.png.width);

	png_byte* src_ptr;
	uint32_t* dest_ptr = image->data.png.rgbaData;
	for (unsigned int r = 0; r < image->data.png.height; r++) {
		src_ptr = image->data.png.row_ptrs[r];
		for (unsigned int c = 0; c < image->data.png.width; c++) {
			png_byte red = *src_ptr;
			png_byte green = *(++src_ptr);
			png_byte blue = *(++src_ptr);
			png_byte alpha = *(++src_ptr);
			*(dest_ptr) = (alpha << 24) | (red << 16) | (green << 8) | blue;

			dest_ptr++; src_ptr++;
		}
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	fclose(pngFile);
	return;

}

void writeFileImageRaw_PNG(const char* fileName, unsigned height, unsigned width, unsigned* data){
	FILE* pngFile;
	if ((pngFile = fopen(fileName, "wb") == NULL)) {
		printf("Error opening %s", fileName);
		return;
	}

	// Writing Meta-Data

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);

	png_init_io(png_ptr, pngFile);
	png_set_IHDR(
		png_ptr, info_ptr,
		width, height,
		8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, 
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);
	png_write_info(png_ptr, info_ptr);

	// Writing Data
	
	png_byte** dataBytes = (png_byte**)malloc(height * width * 4);

	// TODO: Implement correct copy logic
	/* for (unsigned r = 0; r < height; r++)
		memcpy(*(dataBytes + (r * width * 4)), *(data + ((height - r - 1) * width * 4)), width * 4); // check if working */

	png_write_image(png_ptr, dataBytes);
	png_write_end(png_ptr, NULL);

	png_destroy_write_struct(&png_ptr, &info_ptr);

	free(dataBytes);
	fclose(pngFile);
}

void delFileImage_PNG(Image* image) {
	if (image->fileFormat != IMG_Png) {
		puts("Image provided for deletion is not PNG type");
		return;
	}

	for (unsigned int r = 0; r < image->data.png.height; r++)
		free(image->data.png.row_ptrs[r]);
	free(image->data.png.row_ptrs);

	free(image->data.png.rgbaData);
	image->fileFormat = IMG_NonValid;
	return;
}

#endif
