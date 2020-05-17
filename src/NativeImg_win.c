#include "ImageLoader.h"

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

BITMAP createWinBmap(const Image* image) {
	switch (image->fileFormat) {

	case(IMG_Tiff):
#ifdef USE_IMG_TIFF
		revrsColorBits_RB(image->imageData.tiff.raster, image->imageData.tiff.width * image->imageData.tiff.length); // Should maybe move to ImgTIFF
		return createWinBmap_Raw(image->imageData.tiff.width, image->imageData.tiff.length, image->imageData.tiff.raster);
#endif
		break;

	case(IMG_Bmp):
#ifdef USE_IMG_BMP
		return createWinBmap_Raw(abs(image->imageData.bmp.width), abs(image->imageData.bmp.height), image->imageData.bmp.data);
#endif
		break;

	case(IMG_Png):
#ifdef USE_IMG_PNG
		return createWinBmap_Raw(image->imageData.png.width, image->imageData.png.height, image->imageData.png.rgbaData);
#endif
		break;

	default:
		puts("Image Format not yet supported!!!");
		break;
	}

	BITMAP bmap = { 0 };
	puts("Make sure the correct libraries are built and ImageSupport.h has correct macro definitions");
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