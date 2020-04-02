#include "ImageLoader.h"

BITMAP createWinBmap(const Image* image) {
	BITMAP bmap = { 0 };

	switch (image->fileFormat) {

	// All bitmaps have these properties regardless
	bmap.bmType = 0;
	bmap.bmPlanes = 1;
	bmap.bmBitsPixel = 32;

	case(IMG_Tiff):
#ifdef USE_IMG_TIFF
		bmap.bmWidth = image->imageData.tiff.width;
		bmap.bmHeight = image->imageData.tiff.length;
		bmap.bmWidthBytes = (image->imageData.tiff.width * sizeof(uint32)) / sizeof(BYTE);

		revrsColorEncoding(image->imageData.tiff.raster, bmap.bmHeight * bmap.bmWidth); // Should maybe move to ImgTIFF
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

	case(IMG_Targa):
#ifdef USE_IMG_TARGA // DEFECTIVE!!
		bmap.bmWidth = abs(image->imageData.targa.width);
		bmap.bmHeight = abs(image->imageData.targa.height);
		bmap.bmWidthBytes = (image->imageData.targa.width * sizeof(int32_t)) / sizeof(BYTE);

		bmap.bmBits = image->imageData.targa.data;
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

    // In case a macro isnt defined include an empty bitmap error check here

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