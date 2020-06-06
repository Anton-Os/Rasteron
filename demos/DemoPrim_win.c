#include "ImageLoader.h"
#include "Rasteron.h"

#include <stdio.h>
#include <stdlib.h>

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
	}
	case (WM_PAINT): {
		Image img2 = { 0 };
		loadImage_TIFF("C:\\AntonDocs\\Design\\PurpleCult.tif", &img2);

		Rasteron_Image* rImage = rstnCreate_ImgBase(&img2);
		// changeSolidColor(rImage->data, rImage->height * rImage->width, 0xFF526870, 0x00FFFFFF);
		Rasteron_Palette* rPalette = rstnCreate_Palette(rImage);
		Rasteron_Outline* rOutline = rstnCreate_Outline(rImage);
		Rasteron_Sprite* rSprite = rstnCreate_Sprite(rImage);

		Rasteron_Image* rImageGrey = rstnCreate_ImgGrey(rImage);

		BITMAP bmapTiff = createWinBmap(&img2);
		BITMAP bmapBase = createWinBmap_Raw(rImage->width, rImage->height, rImage->data);
		// BITMAP bmapBase = createWinBmap_Raw(rImageGrey->width, rImageGrey->height, rImageGrey->data);
		drawWinBmap(hwnd, &bmapBase);

		rstnDel_Img(rImage);
		rstnDel_Img(rImageGrey);

		rstnDel_Palette(rPalette);
		rstnDel_Outline(rOutline);
		rstnDel_Sprite(rSprite);
		
        delImage_TIFF(&img2);
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

int main(int argc, char** argv) {

	WNDCLASS wndClass = { 0 };
	// wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.lpfnWndProc = wndProc;
	wndClass.lpszClassName = "Default Class";
	RegisterClass(&wndClass);

	HWND wndWindow = CreateWindow(
		"Default Class",
		"Team Purple",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1200, 1100,
		NULL, NULL, GetModuleHandle(NULL), NULL
	);

	ShowWindow(wndWindow, 1);
	UpdateWindow(wndWindow);

	MSG wndMessage;
	BOOL bRet;


	return 0;
}