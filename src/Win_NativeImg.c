#include "Toolbox.h"

BITMAP createWinBmapRaw(uint32_t width, uint32_t height, uint32_t* data){
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

#ifdef USE_IMG_TIFF
	case(IMG_Tiff):
		switchRasterRB(image->imageData.tiff.raster, image->imageData.tiff.width * image->imageData.tiff.length); // Should maybe move to ImgTIFF
		return createWinBmapRaw(image->imageData.tiff.width, image->imageData.tiff.length, image->imageData.tiff.raster);
#endif
#ifdef USE_IMG_BMP
	case(IMG_Bmp):
		return createWinBmapRaw(abs(image->imageData.bmp.width), abs(image->imageData.bmp.height), image->imageData.bmp.data);
#endif
#ifdef USE_IMG_PNG
	case(IMG_Png): 
		return createWinBmapRaw(image->imageData.png.width, image->imageData.png.height, image->imageData.png.rgbaData);
#endif
	default:
		puts("Image Format not yet supported!!!");
		break;
	}

	BITMAP bmap = { 0 };
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