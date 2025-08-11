#include "Util_OS.h"

static IWICImagingFactory *wicFactory = NULL;
static IWICBitmapEncoder *wicEncoder = NULL;
static IWICBitmapFrameEncode *wicBitmapFrame = NULL;

// Path Generation

void replaceFwdSlash(char* str){
	unsigned len = strlen(str);
	for(unsigned l = 0; l < len; l++) if(*(str + l) == '/') *(str + l) = '\\'; // switch the dash type
}

void genFullFilePath(const char* name, char* fullFilePath){
	strcpy(fullFilePath, ASSETS_DIR);
	strcat(fullFilePath, "\\");
	strcat(fullFilePath, name);
	replaceFwdSlash(fullFilePath);
}

// Windows Specific

void createWindow(WNDPROC wndProc, LPCTSTR name, unsigned width, unsigned height){
	WNDCLASS windowClass = { 0 };
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.lpfnWndProc = wndProc;
	windowClass.lpszClassName = "Rasteron";
	RegisterClass(&windowClass);

	// HWND window = capCreateCaptureWindow(
	HWND window = CreateWindowA(
		"Rasteron",
		name,
		WS_OVERLAPPEDWINDOW, // WS_CHILD | WS_VISIBLE, 
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		NULL, NULL, GetModuleHandle(NULL), NULL
	);

	ShowWindow(window, 1);
	UpdateWindow(window);

	SetTimer(window, TIMER_EVENT_ID, 1000, NULL); // creates timer event for every second

	/* HWND capWindow = capCreateCaptureWindow(
		"Capture Window",
		WS_CHILD | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width / 2, height / 2,
		window, 1
	);

	capDriverConnect (window, 0);
	capCaptureSequence (window); */
}

void eventLoop(eventLoopCallback callback){
	MSG message;
	BOOL bRet;

	while (1) {
		bRet = GetMessage(&message, NULL, 0, 0);
		if (bRet > 0){  // (bRet > 0 indicates a message that must be processed.)
			TranslateMessage(&message);
			DispatchMessage(&message);
		} 
		else break;  // (bRet == 0 indicates "exit program".)

		if(callback != NULL) callback();
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


#ifdef RASTERON_ENABLE_ANIM

void encodeQueue(Rasteron_Queue* queue){
	static IMFSinkWriter* sinkWriter = NULL;
	static IMFMediaType *mediaTypeIn = NULL, *mediaTypeOut = NULL;

	const uint32_t frameWidth = queue_getImg(queue, 0)->width;
	const uint32_t frameHeight = queue_getImg(queue, 0)->height;
	const uint32_t frameRes = frameWidth * frameHeight;
	const uint32_t fps = 30;
	const uint32_t frameCount = queue-frameCount * fps;
	const uint32_t bitrate = 800000; // is this correct?
	const uint64_t duration = (queue->frameCount * 1000 * 1000) / fps; // is this correct?

	const GUID encodeFrmt = MFVideoFormat_WMV3;
	const GUID inputFrmt = MFVideoFormat_RGB32;

	DWORD* data = (DWORD*)malloc(sizeof(DWORD) * frameRes * queue->frameCount);

	unsigned d = 0;
	for(unsigned f = 0; f < queue->frameCount && d < frameRes; f++){
		Rasteron_Image* frame = queue_getImg(queue, f);
		for(unsigned p = 0; p < frame->width * frame->height; p++){
			*(data + d) = *(frame->data + p);
			d++;
		}
	}

	wchar_t mediaOutputName[1024];
	unsigned nameLen = strlen(queue->prefix);
	mbstowcs_s(&nameLen, mediaOutputName, queue->prefix, nameLen);

	if(!SUCCEEDED(MFCreateSinkWriterFromURL(mediaOutputName, NULL, NULL, &sinkWriter))) return perror("Failed to create sink writer"); // TODO: Update name

	DWORD streamIndex;

	if(!SUCCEEDED(MFCreateMediaType(&mediaTypeOut))) return perror("Failed to create media type for output");
	if(!SUCCEEDED(mediaTypeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video))) return perror("Failed to set parameter");
	if(!SUCCEEDED(mediaTypeOut->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_WMV3))) return perror("Failed to set parameter");
	if(!SUCCEEDED(mediaTypeOut->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive))) return perror("Failed to set parameter");
	if(!SUCCEEDED(mediaTypeOut->SetUINT32(MF_MT_AVG_BITRATE, 800000))) return perror("Failed to set parameter");
	if(!SUCCEEDED(MFSetAttributeSize(mediaTypeOut, MF_MT_FRAME_SIZE, 1024, 1024))) return perror("Failed to set parameter");
	if(!SUCCEEDED(MFSetAttributeRatio(mediaTypeOut, MF_MT_FRAME_RATE, 60, 1))) return perror("Failed to set parameter");
	if(!SUCCEEDED(MFSetAttributeRatio(mediaTypeOut, MF_MT_PIXEL_ASPECT_RATIO, 1, 1))) return perror("Failed to set parameter");
	// Add to stream
	if(!SUCCEEDED(sinkWriter->AddStream(mediaTypeOut, &streamIndex))) return perror("Failed to add stream");

	if(!SUCCEEDED(MFCreateMediaType(&mediaTypeIn))) return perror("Failed to create media type for input");
	if(!SUCCEEDED(mediaTypeIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video))) return perror("Failed to set parameter");
	if(!SUCCEEDED(mediaTypeIn->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_WMV3))) return perror("Failed to set parameter");
	if(!SUCCEEDED(mediaTypeIn->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive))) return perror("Failed to set parameter");
	if(!SUCCEEDED(MFSetAttributeSize(mediaTypeIn, MF_MT_FRAME_SIZE, 1024, 1024))) return perror("Failed to set parameter");
	if(!SUCCEEDED(MFSetAttributeRatio(mediaTypeIn, MF_MT_FRAME_RATE, 60, 1))) return perror("Failed to set parameter");
	if(!SUCCEEDED(MFSetAttributeRatio(mediaTypeIn, MF_MT_PIXEL_ASPECT_RATIO, 1, 1))) return perror("Failed to set parameter");
	// Add to input
	if(!SUCCEEDED(sinkWriter->SetInputMediaType(streamIndex, mediaTypeIn, NULL)))  return perror("Failed to add input");

	// Writing to Sink
	if(!SUCCEEDED(!sinkWriter->BeginWriting())) return perror("Writing failed!");

	SafeRelease(&sinkWriter);
	SafeRelease(&mediaTypeOut);
	SafeRelease(&mediaTypeIn);

	free(data);
}
#endif