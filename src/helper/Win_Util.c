#include "OS_Util.h"

void createWindow(WNDPROC wndProc, LPCTSTR name){
	WNDCLASS wndClass = { 0 };
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.lpfnWndProc = wndProc;
	wndClass.lpszClassName = "Rasteron";
	RegisterClass(&wndClass);

	HWND wndWindow = CreateWindow(
		"Rasteron",
		name,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1200, 1100,
		NULL, NULL, GetModuleHandle(NULL), NULL
	);

	ShowWindow(wndWindow, 1);
	UpdateWindow(wndWindow);

	SetTimer(wndWindow, TIMER_EVENT_ID, 1000, NULL); // creates timer event for every second
}

void eventLoop(){
	MSG message;
	BOOL bRet;

	while (1) {
		bRet = GetMessage(&message, NULL, 0, 0);
		if (bRet > 0){  // (bRet > 0 indicates a message that must be processed.)
			TranslateMessage(&message);
			DispatchMessage(&message);
		}  // (bRet == -1 indicates an error.)
		else break;  // (bRet == 0 indicates "exit program".)
	}
}

BITMAP createWinBmapRaw(uint32_t height, uint32_t width, uint32_t* data){
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

BITMAP createWinBmap(Rasteron_Image* image){
	createWinBmapRaw(image->height, image->width, image->data);
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