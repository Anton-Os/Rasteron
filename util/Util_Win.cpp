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

BITMAP createWinBmap(Rasteron_Image* image){ return createWinBmapRaw(image->height, image->width, image->data); }

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


#ifdef RASTERON_ENABLE_QUEUE

static int encodeFailed(const char* step, HRESULT result){
	fprintf(stderr, "Encoding failed, %s (error 0x%08lX)\n", step, (unsigned long)result);
	return FALSE;
}

int encodeQueueExt(Rasteron_Queue* queue, const char* fileName, unsigned fps, const double* intervals){
	if(queue == NULL || queue->frameCount == 0 || fileName == NULL) return FALSE;
	if(fps == 0) fps = 60;

	Rasteron_Image* frameImg = *(queue->frameData + 0);

	const uint32_t frameWidth = frameImg->width;
	const uint32_t frameHeight = frameImg->height;
	const uint32_t bitrate = 5000000;

	const GUID encodeFrmt = MFVideoFormat_H264;
	const GUID inputFrmt = MFVideoFormat_RGB32;

	wchar_t mediaOutputName[1024];
	if(MultiByteToWideChar(CP_ACP, 0, fileName, -1, mediaOutputName, 1024) == 0) return encodeFailed("output name cannot be used", E_INVALIDARG);

	HRESULT result = MFStartup(MF_VERSION, MFSTARTUP_FULL);
	if(FAILED(result)) return encodeFailed("Media Foundation did not start", result);
	HRESULT comResult = CoInitialize(NULL);

	IMFSinkWriter* sinkWriter = NULL;
	IMFMediaType *mediaTypeIn = NULL, *mediaTypeOut = NULL;
	DWORD streamIndex = 0;
	const char* step = "cannot create the output file";

	result = MFCreateSinkWriterFromURL(mediaOutputName, NULL, NULL, &sinkWriter);

	if(SUCCEEDED(result)){ step = "cannot set up the output format"; result = MFCreateMediaType(&mediaTypeOut); }
	if(SUCCEEDED(result)) result = mediaTypeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	if(SUCCEEDED(result)) result = mediaTypeOut->SetGUID(MF_MT_SUBTYPE, encodeFrmt);
	if(SUCCEEDED(result)) result = mediaTypeOut->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
	if(SUCCEEDED(result)) result = mediaTypeOut->SetUINT32(MF_MT_AVG_BITRATE, bitrate);
	if(SUCCEEDED(result)) result = MFSetAttributeSize(mediaTypeOut, MF_MT_FRAME_SIZE, frameWidth, frameHeight);
	if(SUCCEEDED(result)) result = MFSetAttributeRatio(mediaTypeOut, MF_MT_FRAME_RATE, fps, 1);
	if(SUCCEEDED(result)) result = MFSetAttributeRatio(mediaTypeOut, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
	if(SUCCEEDED(result)) result = sinkWriter->AddStream(mediaTypeOut, &streamIndex); // Add to stream

	if(SUCCEEDED(result)){ step = "cannot set up the input format"; result = MFCreateMediaType(&mediaTypeIn); }
	if(SUCCEEDED(result)) result = mediaTypeIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	if(SUCCEEDED(result)) result = mediaTypeIn->SetGUID(MF_MT_SUBTYPE, inputFrmt);
	if(SUCCEEDED(result)) result = mediaTypeIn->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
	if(SUCCEEDED(result)) result = mediaTypeIn->SetUINT32(MF_MT_AVG_BITRATE, bitrate);
	if(SUCCEEDED(result)) result = MFSetAttributeSize(mediaTypeIn, MF_MT_FRAME_SIZE, frameWidth, frameHeight);
	if(SUCCEEDED(result)) result = MFSetAttributeRatio(mediaTypeIn, MF_MT_FRAME_RATE, fps, 1);
	if(SUCCEEDED(result)) result = MFSetAttributeRatio(mediaTypeIn, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
	if(SUCCEEDED(result)) result = sinkWriter->SetInputMediaType(streamIndex, mediaTypeIn, NULL); // Add to input

	// Writing to Sink
	if(SUCCEEDED(result)){ step = "the encoder rejected the video size or format"; result = sinkWriter->BeginWriting(); }

	// Writing frames
	const LONGLONG frameDuration = 10000000 / fps; // 1 second = 10M hns
	const DWORD buffSize = frameWidth * frameHeight * 4;
	LONGLONG timestamp = 0;
	unsigned written = 0; // video frames so far
	double shownUntil = 0.0; // second at which the current image is replaced
	if(SUCCEEDED(result)) step = "cannot write a frame";

	for (unsigned f = 0; f < queue->frameCount && SUCCEEDED(result); f++) {
		Rasteron_Image* frame = *(queue->frameData + f);
		if(frame == NULL || frame->width != frameWidth || frame->height != frameHeight) continue; // every image needs the size of the first

		shownUntil += (intervals != NULL && intervals[f] > 0.0)? intervals[f] : 1.0 / fps;
		do { // an image is held by writing it again until its interval has passed
			// Creating Buffer
			IMFMediaBuffer* mediaBuffer = nullptr;
			result = MFCreateMemoryBuffer(buffSize, &mediaBuffer);
			if(FAILED(result)) break;

			// Copying Pixels
			BYTE* pixels = nullptr;
			DWORD maxLen = 0, curLen = 0;
			result = mediaBuffer->Lock(&pixels, &maxLen, &curLen);
			if(SUCCEEDED(result)){
				memcpy(pixels, frame->data, buffSize); // copying to destination
				mediaBuffer->Unlock();
				mediaBuffer->SetCurrentLength(buffSize);
			}

			// Creating Sample
			IMFSample* mediaSample = NULL;
			if(SUCCEEDED(result)) result = MFCreateSample(&mediaSample);
			if(SUCCEEDED(result)){
				mediaSample->AddBuffer(mediaBuffer);
				mediaSample->SetSampleTime(timestamp);
				mediaSample->SetSampleDuration(frameDuration);

				result = sinkWriter->WriteSample(streamIndex, mediaSample); // Writing Sample
				mediaSample->Release();
			}
			mediaBuffer->Release();

			timestamp += frameDuration;
			written++;
		} while(SUCCEEDED(result) && ((double)written + 0.5) / (double)fps < shownUntil);
	}

	if(SUCCEEDED(result)){ step = "cannot finish the output file"; result = sinkWriter->Finalize(); }

	if(sinkWriter != NULL) sinkWriter->Release();
	if(mediaTypeIn != NULL) mediaTypeIn->Release();
	if(mediaTypeOut != NULL) mediaTypeOut->Release();

	MFShutdown();
	if(SUCCEEDED(comResult)) CoUninitialize();

	if(FAILED(result)) return encodeFailed(step, result);
	return (written > 0)? TRUE : FALSE;
}

void encodeQueue(Rasteron_Queue* queue){
	assert(queue->frameCount > 0);

	encodeQueueExt(queue, "Test.mp4", 60, NULL); // one video frame per image
}

#else

/* void encodeQueue(Rasteron_Queue* queue){
	assert(queue->frameCount > 0);

	for(unsigned f = 0; f < queue->frameCount; f++){
		Rasteron_Image* image = queue_getImg(queue, f);
		writeFileImageRaw(image->name, IMG_Bmp, image->height, image->width, image->data);
	}
} */

#endif