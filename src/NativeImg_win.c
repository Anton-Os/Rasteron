#include "ImageLoader.h"

BITMAP createWinBmap(const Image* image) {
	BITMAP bmap = { 0 };

	switch (image->fileFormat) {

	case(IMG_Tiff):
#ifdef USE_IMG_TIFF
		bmap.bmWidth = image->imageData.tiff.width;
		bmap.bmHeight = image->imageData.tiff.length;
		bmap.bmWidthBytes = (image->imageData.tiff.width * sizeof(uint32)) / sizeof(BYTE);

		revrsColorBits_RB(image->imageData.tiff.raster, bmap.bmHeight * bmap.bmWidth); // Should maybe move to ImgTIFF
		bmap.bmBits = image->imageData.tiff.raster;
#endif
		break;

	case(IMG_Bmp):
#ifdef USE_IMG_BMP
		bmap.bmWidth = abs(image->imageData.bmp.width);
		bmap.bmHeight = abs(image->imageData.bmp.height);
		bmap.bmWidthBytes = (image->imageData.bmp.width * sizeof(uint32_t)) / sizeof(BYTE);

		bmap.bmBits = image->imageData.bmp.data;
#endif
		break;

	case(IMG_Png):
#ifdef USE_IMG_PNG
		bmap.bmWidth = image->imageData.png.width;
		bmap.bmHeight = image->imageData.png.height;
		bmap.bmWidthBytes = (image->imageData.png.width * sizeof(uint32_t)) / sizeof(BYTE);

		bmap.bmBits = image->imageData.png.rgbaData;
#endif
		break;

	default:
		puts("Image Format not yet supported!!!");
		break;
	}

	// All bitmaps have these properties regardless
	bmap.bmType = 0;
	bmap.bmPlanes = 1;
	bmap.bmBitsPixel = 32;

    // In case a macro isnt defined include an empty bitmap error check here

	return bmap;
}

BITMAP createWinBmap_Raw(uint32_t width, uint32_t height, uint32_t* data){
	BITMAP bmap = { 0 };
	bmap.bmWidth = width;
	bmap.bmHeight = height;
	bmap.bmWidthBytes = (width * sizeof(uint32_t)) / sizeof(BYTE);
	bmap.bmType = 0;
	bmap.bmPlanes = 1;
	bmap.bmBitsPixel = 32;

	bmap.bmBits = data;

	return bmap;
}

void drawWinBmap(HWND hwnd, const BITMAP* bmap){
    HBITMAP hBmap = CreateBitmapIndirect(bmap);

    PAINTSTRUCT pStruct;
    HDC hPDC = BeginPaint(hwnd, &pStruct);
    HDC hTmpDC = CreateCompatibleDC(NULL);

    SelectObject(hTmpDC, hBmap);
    BitBlt(hPDC, 0, 0, bmap->bmWidth, bmap->bmHeight, hTmpDC, 0, 0, SRCCOPY);

    EndPaint(hwnd, &pStruct);
    ReleaseDC(hwnd, hTmpDC);
}