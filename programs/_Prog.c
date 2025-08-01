#include "_Prog.h"

// --------------------------------   Objects for Demo    -------------------------------- //

ColorSwatch _swatch = { 0xFF888888, { 0xFFFF8888, 0xFF88FF88, 0xFF8888FF, 0xFF333333, 0xFFEEEEEE, 0xFF888800, 0xFF880088, 0xFF008888 }, 0xF }; // determines color oprations
// ColorSwatch _swatch = { 0xFF111111, { 0xFFFF1111, 0xFF11FF11, 0xFF1111FF, 0xFF333333, 0xFFEEEEEE, 0xFF111100, 0xFF110011, 0xFF001111 }, 0xF }; // determines color oprations
// ColorSwatch _swatch = { 0xFF888888, { 0xFFFF8888, 0xFF88AA88, 0xFF8888AA, 0xFF333333, 0xFFEEEEEE, 0xFFAAAA00, 0xFFAA00AA, 0xFF00AAAA }, 0xF }; // determines color oprations
// Catalouged Rules/Callbacks


Rasteron_Image* _savedImg = NULL;
Rasteron_Image* _outputImg = NULL;

#ifdef RASTERON_ENABLE_ANIM
Rasteron_Queue* _mainQueue = NULL;
#endif

unsigned elapseSecs = 0;

unsigned _dimens[2] = { 1, 1};

// --------------------------------  Functions for Demo    -------------------------------- //

void parseInput(char lastInput){
    if(isdigit(lastInput)){
        printf("Parsing numeric input for %d", lastInput);
        // TODO: Change Properties
    }
    else if(lastInput == '-'){ if(_dimens[0] > 0) _dimens[0]--; if(_dimens[1] > 0) _dimens[1]--; }
    else if(lastInput == '='){ if(_dimens[0] < 20) _dimens[0]++; if(_dimens[1] < 20) _dimens[1]++; }
}

// --------------------------------   Porting layer for Demo    -------------------------------- //

#ifdef _WIN32

BITMAP bmap;

void CALLBACK wndTimerCallback(HWND hwnd, UINT uMsg, UINT timerId, DWORD dwTime){
    elapseSecs++;
    if(_onTickEvent != NULL) _onTickEvent(elapseSecs);
}

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    RECT rect;

    switch (message) {
    case (WM_CREATE): {
        if(_outputImg == NULL) _outputImg = solidImgOp((ImageSize){ 1024, 1024}, 0xFFFFFF00);
        if(_onTickEvent != NULL) SetTimer(hwnd, 0, 1000, &wndTimerCallback);
        bmap = createWinBmap(_outputImg);
    }
    case (WM_CHAR): { if(wParam != 0){
        GetClientRect(hwnd, &rect);
        InvalidateRect(hwnd, &rect, FALSE);

        // TODO: Save image on special sequence
        if(_onKeyEvent != NULL){
            parseInput((char)wParam);
            _onKeyEvent(wParam);
            if(_outputImg != NULL){
                if(_savedImg != NULL) RASTERON_DEALLOC(_savedImg);
                _savedImg = copyImgOp(_outputImg);
            }
            bmap = createWinBmap(_outputImg);
        }
    }
    case (WM_PAINT): {
        InvalidateRect(hwnd, &rect, TRUE);
        drawWinBmap(hwnd, &bmap);
    }
    case (WM_TIMER): {
        GetClientRect(hwnd, &rect);
        InvalidateRect(hwnd, &rect, FALSE);
        if(_outputImg != NULL && _onTickEvent != NULL) bmap = createWinBmap(_outputImg);
    }
    case (WM_LBUTTONDOWN): { if(message == WM_LBUTTONDOWN && _onPressEvent != NULL){
        _onPressEvent((double)GET_X_LPARAM(lParam) / RASTERON_WIN_WIDTH, (double)GET_Y_LPARAM(lParam) / RASTERON_WIN_HEIGHT);
    }}
    case (WM_RBUTTONDOWN): { if(message == WM_RBUTTONDOWN && _onPressEvent != NULL){
        _onPressEvent((double)GET_X_LPARAM(lParam) / RASTERON_WIN_WIDTH, (double)GET_Y_LPARAM(lParam) / RASTERON_WIN_HEIGHT);
    }}
    case (WM_CLOSE): {}
    default: return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
    }
}

#elif defined __linux__

Platform_Context unixContext;

void unixProc(char lastKey, double cursorPos[2]){
    if(_onKeyEvent != NULL){
        parseInput(lastKey);
        _onKeyEvent(lastKey);
        if(_outputImg != NULL){
            if(_savedImg != NULL) RASTERON_DEALLOC(_savedImg);
            _savedImg = copyImgOp(_outputImg);
        }
    }
    if(_onPressEvent != NULL) _onPressEvent(cursorPos[0] / RASTERON_WIN_WIDTH, cursorPos[1] / RASTERON_WIN_HEIGHT);
    // if(_onTickEvent != NULL) // TODO: Track timer and perform updates

    if(_outputImg != NULL){
        XImage* bmap = createUnixBmap(&unixContext, _outputImg);
        drawUnixBmap(&unixContext, bmap);
        // XDestroyImage(bmap); // destroy after creation
    }
}

#endif

// --------------------------------   Callable Methods for Demo    -------------------------------- //

void saveToFile(const Rasteron_Image* image, enum IMG_FileFormat format){
    assert(image != NULL);

    char fileName[1024];
    char* fileExt = ".";
    switch(format){
        case IMG_Bmp: fileExt = ".bmp"; break;
        case IMG_Png: fileExt = ".png"; break;
        case IMG_Tiff: fileExt = ".tiff"; break;
    }

    strcpy(fileName, image->name);
    strcat(fileName, fileExt);

    unsigned short iters = 0;
    while(access(fileName, F_OK) == 0){
        iters++;

        char* tempFileName = "";
        strncpy(tempFileName, fileName, strlen(fileName) - ((iters > 1)? 6 : 4));

        char newFileName[1024];
        sprintf(newFileName, "%s-%d", tempFileName, iters);
        strcat(newFileName, fileExt);

        strcpy(fileName, newFileName);
    }
    writeFileImageRaw(fileName, format, image->height, image->width, image->data);
}

char* parseArgs(int argc, char** argv){
    for(unsigned a = 1; a < argc; a++){
        char* arg = *(argv + a);
        unsigned short argSize = strlen(arg);
        printf("Arg %d with size %d: %s, start is %c\n", a, argSize, arg, *(arg + 0));

        if(*(arg + 0) == (char)'['){ // Key Event
            char cursorTxt[2][256];
            unsigned short cursorIdx = 0, cursorTxtIdx = 0;
            for(unsigned l = 1; l < argSize && *(arg + l) != ']'; l++)
                if((*(arg + l) == ';' || *(arg + l) == ',') && cursorIdx < 1){
                    cursorIdx++;
                    cursorTxtIdx = 0; // reset for new value
                }
                else if(cursorTxtIdx < 256){
                    cursorTxt[cursorIdx][cursorTxtIdx] = *(arg + l);
                    cursorTxtIdx++;
                }
            _onPressEvent(atof(cursorTxt[0]), atof(cursorTxt[1]));
        }
        else if(isupper(*(arg + 0)) && getFormat(arg) != IMG_NonValid && _outputImg != NULL)
            saveToFile(_outputImg, getFormat(arg));
        else for(unsigned l = 0; l < argSize; l++) _onKeyEvent(*(arg + l));
    }

    char* args = convertCharray(argc, argv);
    printf("Args are %s\n", args);
    return args;
}

void _run(int argc, char** argv, imageArgCallback callback){
    char* args = parseArgs(argc, argv);
    printf("Parsed ars are %s", args);
    if(callback != NULL) _outputImg = callback(args); // pass args here
    if(argc > 1){ // Parse Command LIne
        saveToFile(_outputImg, IMG_Bmp);
        if(_mainQueue->frameCount > 0)
            for(unsigned f = 0; f < _mainQueue->frameCount; f++)
                saveToFile(queue_getImg(_mainQueue, f), IMG_Bmp);
    } else { // Open a window
#ifdef _WIN32
        createWindow(wndProc, RASTERON_WIN_NAME, RASTERON_WIN_WIDTH, RASTERON_WIN_HEIGHT);
        eventLoop(NULL);
#elif defined __linux__
        createWindow(&unixContext, RASTERON_WIN_NAME, RASTERON_WIN_WIDTH, RASTERON_WIN_HEIGHT);
        eventLoop(unixContext.display, unixContext.window, unixProc);
#endif
    }
}
