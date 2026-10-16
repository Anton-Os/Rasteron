#include "Tool.h"

#ifdef __linux__
#include <sys/select.h>
#include <unistd.h>
#endif

// --------------------------------   Objects for Tools    -------------------------------- //

typedef struct { Rasteron_Image* image; unsigned refs; } Tool_BaseRef; // shared applied image

typedef struct {
    void* state;
    Tool_Common common;
    Tool_BaseRef* base;
} Tool_Snapshot;

static const Tool_Common _commonDefaults = { { TOOL_WIDTH, TOOL_HEIGHT }, 1234 };
Tool_Common _toolCommon = { { TOOL_WIDTH, TOOL_HEIGHT }, 1234 };

static const Tool_Param _commonParams[] = {
    TOOL_PARAM("size", PARAM_Int, Tool_Common, size, 2, 16, TOOL_SIZE_MAX, NULL, "output width and height in pixels (one value sets both)"),
    TOOL_PARAM("seed", PARAM_Int, Tool_Common, seed, 1, 0, 2147483647, NULL, "random seed applied before each render, 'rand' picks a new one"),
};

static const Tool_Desc* _desc = NULL;
static Rasteron_Image* _toolOutput = NULL; // displayed image
static Tool_BaseRef* _toolBase = NULL; // applied image
static Tool_Snapshot _history[TOOL_HISTORY_MAX];
static unsigned _historyCount = 0;

static int _dirty = FALSE; // output must be regenerated
static int _quit = FALSE;
static int _headless = FALSE;
static int _saved = FALSE; // tracks if batch mode already wrote a file
static unsigned _entropy = 0;

// --------------------------------   Utilities    -------------------------------- //

static double toolTime(void){
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

static char* toolStrdup(const char* str){
    char* copy = (char*)malloc(strlen(str) + 1);
    if(copy != NULL) strcpy(copy, str);
    return copy;
}

static void reseedEntropy(void){ // keeps 'rand' values unpredictable after a seeded render
    _entropy++;
    srand((unsigned)time(NULL) ^ (_entropy * 2654435761u) ^ (unsigned)clock());
}

unsigned tool_rand(void){ return ((unsigned)rand() << 16) ^ ((unsigned)rand() << 4) ^ (unsigned)rand(); }

double tool_randf(void){ return (double)rand() / (double)RAND_MAX; }

ImageSize tool_size(void){ return (ImageSize){ (uint32_t)_toolCommon.size[1], (uint32_t)_toolCommon.size[0] }; }

const Rasteron_Image* tool_getBase(void){ return (_toolBase != NULL)? _toolBase->image : NULL; }

const Rasteron_Image* tool_getOutput(void){ return _toolOutput; }

// --------------------------------   Applied Image References    -------------------------------- //

static Tool_BaseRef* baseRetain(Tool_BaseRef* ref){ if(ref != NULL) ref->refs++; return ref; }

static void baseRelease(Tool_BaseRef* ref){
    if(ref == NULL) return;
    if(ref->refs > 1){ ref->refs--; return; }
    if(ref->image != NULL) RASTERON_DEALLOC(ref->image);
    free(ref);
}

void tool_setBase(Rasteron_Image* image){
    Tool_BaseRef* ref = NULL;
    if(image != NULL){
        ref = (Tool_BaseRef*)malloc(sizeof(Tool_BaseRef));
        ref->image = image;
        ref->refs = 1;
    }
    baseRelease(_toolBase);
    _toolBase = ref;
    _dirty = TRUE;
}

// --------------------------------   History    -------------------------------- //

static void snapshotFree(Tool_Snapshot* snapshot){
    free(snapshot->state);
    baseRelease(snapshot->base);
    snapshot->state = NULL;
    snapshot->base = NULL;
}

static void historyPush(void){
    if(_historyCount == TOOL_HISTORY_MAX){ // drop oldest entry
        snapshotFree(&_history[0]);
        memmove(&_history[0], &_history[1], sizeof(Tool_Snapshot) * (TOOL_HISTORY_MAX - 1));
        _historyCount--;
    }
    Tool_Snapshot* snapshot = &_history[_historyCount++];
    snapshot->state = malloc(_desc->stateSize);
    memcpy(snapshot->state, _desc->state, _desc->stateSize);
    snapshot->common = _toolCommon;
    snapshot->base = baseRetain(_toolBase);
}

static int historyChanged(void){ // compares live state with newest snapshot
    if(_historyCount == 0) return TRUE;
    Tool_Snapshot* snapshot = &_history[_historyCount - 1];
    return memcmp(snapshot->state, _desc->state, _desc->stateSize) != 0
        || memcmp(&snapshot->common, &_toolCommon, sizeof(Tool_Common)) != 0
        || snapshot->base != _toolBase;
}

static void historyDrop(void){ if(_historyCount > 0) snapshotFree(&_history[--_historyCount]); }

static void historyRestore(void){ // pops newest snapshot into the live state
    if(_historyCount == 0) return;
    Tool_Snapshot* snapshot = &_history[--_historyCount];
    memcpy(_desc->state, snapshot->state, _desc->stateSize);
    _toolCommon = snapshot->common;
    baseRelease(_toolBase);
    _toolBase = snapshot->base; // reference moves to live state
    snapshot->base = NULL;
    free(snapshot->state);
    snapshot->state = NULL;
    _dirty = TRUE;
}

// --------------------------------   Rendering    -------------------------------- //

static void updateTitle(void); // platform specific, see below

void tool_render(void){
    _dirty = FALSE;
    if(_desc == NULL || _desc->generate == NULL) return;

    double start = toolTime();
    srand((unsigned)_toolCommon.seed);
    Rasteron_Image* image = _desc->generate(tool_size(), tool_getBase());
    reseedEntropy();

    if(image == NULL){
        printf("Render failed, keeping previous image\n");
        return;
    }
    if(_toolOutput != NULL) RASTERON_DEALLOC(_toolOutput);
    _toolOutput = image;
    printf("[%s] rendered %ux%u in %.2fs\n", _desc->name, image->width, image->height, toolTime() - start);
    updateTitle();
}

static void flush(void){ if(_dirty) tool_render(); }

static Rasteron_Image* frameImage(unsigned frame, unsigned frameCount){ // produces one animation frame
    ImageSize size = (_toolOutput != NULL)? (ImageSize){ _toolOutput->height, _toolOutput->width } : tool_size();

    if(_desc->animate != NULL){
        void* saved = malloc(_desc->stateSize);
        memcpy(saved, _desc->state, _desc->stateSize);
        _desc->animate(frame, frameCount);
        srand((unsigned)_toolCommon.seed);
        Rasteron_Image* image = _desc->generate(tool_size(), tool_getBase());
        memcpy(_desc->state, saved, _desc->stateSize);
        free(saved);
        reseedEntropy();
        if(image != NULL && (image->width != size.width || image->height != size.height)){
            Rasteron_Image* sized = resizeImgOp(size, image);
            RASTERON_DEALLOC(image);
            image = sized;
        }
        return image;
    }

    // history fallback: every applied image in order followed by the current output
    const Rasteron_Image* steps[TOOL_HISTORY_MAX + 1];
    unsigned stepCount = 0;
    const Tool_BaseRef* last = NULL;
    for(unsigned h = 0; h < _historyCount; h++)
        if(_history[h].base != NULL && _history[h].base != last){
            last = _history[h].base;
            steps[stepCount++] = last->image;
        }
    if(_toolOutput != NULL) steps[stepCount++] = _toolOutput;
    if(stepCount == 0) return NULL;

    const Rasteron_Image* step = steps[(frame * stepCount) / frameCount];
    if(step->width == size.width && step->height == size.height) return copyImgOp(step);
    return resizeImgOp(size, step);
}

// --------------------------------   Saving    -------------------------------- //

static const char* formatExt(enum IMG_FileFormat format){
    switch(format){
        case IMG_Png: return ".png";
        case IMG_Tiff: return ".tiff";
        default: return ".bmp";
    }
}

static int fileExists(const char* fileName){ return access(fileName, F_OK) == 0; }

static void uniqueName(char* out, size_t outSize, const char* stem, const char* ext){
    snprintf(out, outSize, "%s%s", stem, ext);
    for(unsigned i = 1; fileExists(out) && i < 100000; i++) snprintf(out, outSize, "%s_%u%s", stem, i, ext);
}

static void saveImage(const Rasteron_Image* image, enum IMG_FileFormat format, const char* stem, char* outName, size_t outSize){
    uniqueName(outName, outSize, stem, formatExt(format));
    writeFileImageRaw(outName, format, image->height, image->width, image->data);
}

static void printQuickHelp(void); // see Help section

// --------------------------------   Built-in Commands    -------------------------------- //

static int setParamCommand(int argc, char** argv){ // argv[0] is the parameter name
    const Tool_Param* param = toolArgs_findParam(_commonParams, TOOL_COUNT(_commonParams), argv[0]);
    void* state = &_toolCommon;
    if(param == NULL){
        param = toolArgs_findParam(_desc->params, _desc->paramCount, argv[0]);
        state = _desc->state;
    }
    if(param == NULL){
        printf("Unknown parameter '%s'\n", argv[0]);
        printQuickHelp();
        return TOOL_Ok;
    }

    historyPush();
    int ok = toolArgs_setParam(state, param, argc - 1, argv + 1);
    if(ok && param->type == PARAM_Int && param->count == 2 && argc == 2){ // one value fills both, e.g. size 512
        int* pair = (int*)((char*)state + param->offset);
        pair[1] = pair[0];
    }
    if(!ok){
        historyRestore();
        _dirty = FALSE;
        return TOOL_Ok;
    }
    if(!historyChanged()){ historyDrop(); return TOOL_Ok; }

    printf("  %s = ", param->name);
    toolArgs_printValue(state, param, stdout);
    printf("\n");
    return TOOL_Render;
}

static int cmd_set(int argc, char** argv){
    if(argc < 3){
        printf("Usage: set <param> <value...>\n");
        printQuickHelp();
        return TOOL_Ok;
    }
    return setParamCommand(argc - 1, argv + 1);
}

static int cmd_get(int argc, char** argv){
    if(argc < 2){
        for(unsigned p = 0; p < TOOL_COUNT(_commonParams); p++){
            printf("  %-11s = ", _commonParams[p].name); toolArgs_printValue(&_toolCommon, &_commonParams[p], stdout); printf("\n");
        }
        for(unsigned p = 0; p < _desc->paramCount; p++){
            printf("  %-11s = ", _desc->params[p].name); toolArgs_printValue(_desc->state, &_desc->params[p], stdout); printf("\n");
        }
        printf("  applied image: %s, history: %u step(s)\n", (_toolBase != NULL)? "yes" : "none", _historyCount);
        return TOOL_Ok;
    }
    for(int a = 1; a < argc; a++) tool_printHelp(argv[a]);
    return TOOL_Ok;
}

static int cmd_apply(int argc, char** argv){
    if(_toolOutput == NULL){ printf("Nothing to apply yet\n"); return TOOL_Ok; }
    historyPush();
    tool_setBase(copyImgOp(_toolOutput));
    _dirty = FALSE;
    if(_desc->onApply != NULL && _desc->onApply()) _dirty = TRUE;
    printf("Applied %ux%u image (revert to undo)\n", _toolOutput->width, _toolOutput->height);
    return _dirty? TOOL_Render : TOOL_Ok;
}

static int cmd_revert(int argc, char** argv){
    int steps = 1;
    if(argc > 1 && toolArgs_equals(argv[1], "all")) steps = (int)_historyCount;
    else if(argc > 1 && (!toolArgs_int(argv[1], &steps) || steps < 1)) return TOOL_Fail;
    if(_historyCount == 0){ printf("Nothing to revert\n"); return TOOL_Ok; }

    int reverted = 0;
    while(steps-- > 0 && _historyCount > 0){ historyRestore(); reverted++; }
    printf("Reverted %d step(s), %u left\n", reverted, _historyCount);
    return TOOL_Render;
}

static int cmd_reset(int argc, char** argv){
    historyPush();
    memcpy(_desc->state, _desc->defaults, _desc->stateSize);
    _toolCommon = _commonDefaults;
    if(argc > 1 && toolArgs_equals(argv[1], "all")){
        baseRelease(_toolBase);
        _toolBase = NULL;
        if(_desc->init != NULL) _desc->init(0, NULL); // reloads tool inputs such as reference images
    }
    if(!historyChanged()){ historyDrop(); printf("Already at defaults\n"); return TOOL_Ok; }
    printf("Parameters reset to defaults (revert to undo)\n");
    return TOOL_Render;
}

static int cmd_save(int argc, char** argv){
    if(_toolOutput == NULL){ printf("Nothing to save yet\n"); return TOOL_Ok; }

    enum IMG_FileFormat format = IMG_Bmp;
    char stem[1024];
    snprintf(stem, sizeof(stem), "%s", _desc->name);

    for(int a = 1; a < argc; a++){
        if(toolArgs_equals(argv[a], "bmp")) format = IMG_Bmp;
        else if(toolArgs_equals(argv[a], "png")) format = IMG_Png;
        else if(toolArgs_equals(argv[a], "tiff") || toolArgs_equals(argv[a], "tif")) format = IMG_Tiff;
        else {
            snprintf(stem, sizeof(stem), "%s", argv[a]);
            enum IMG_FileFormat nameFormat = (strlen(stem) > 3)? getFormat(stem) : IMG_NonValid;
            if(nameFormat != IMG_NonValid){ // name carries the extension
                format = nameFormat;
                char* dot = strrchr(stem, '.');
                if(dot != NULL) *dot = '\0';
            }
        }
    }

    char fileName[1100];
    saveImage(_toolOutput, format, stem, fileName, sizeof(fileName));
    printf("Saved %s\n", fileName);
    _saved = TRUE;
    return TOOL_Ok;
}

static int parseFrameCount(int argc, char** argv, int* frames){
    *frames = TOOL_FRAMES_DEFAULT;
    if(argc > 1 && (!toolArgs_int(argv[1], frames) || *frames < 1 || *frames > TOOL_FRAMES_MAX)){
        printf("Frame count must be between 1 and %d\n", TOOL_FRAMES_MAX);
        return FALSE;
    }
    return TRUE;
}

static int cmd_frames(int argc, char** argv){
    int frames;
    if(!parseFrameCount(argc, argv, &frames)) return TOOL_Fail;
    if(_desc->animate == NULL) printf("%s has no animation, frames step through applied history\n", _desc->name);

    char stem[256], fileName[1100];
    for(int f = 0; f < frames; f++){
        Rasteron_Image* image = frameImage((unsigned)f, (unsigned)frames);
        if(image == NULL){ printf("No frames available\n"); return TOOL_Ok; }
        snprintf(stem, sizeof(stem), "%s_frame%03d", _desc->name, f);
        saveImage(image, IMG_Bmp, stem, fileName, sizeof(fileName));
        RASTERON_DEALLOC(image);
        printf("\r  wrote frame %d/%d", f + 1, frames);
        fflush(stdout);
    }
    printf("\nSaved %d frames as %s_frameNNN.bmp\n", frames, _desc->name);
    _saved = TRUE;
    return TOOL_Ok;
}

#if TOOL_ENCODE_ENABLED
static int cmd_encode(int argc, char** argv){
    int frames;
    if(!parseFrameCount(argc, argv, &frames)) return TOOL_Fail;
    if(_toolOutput == NULL){ printf("Nothing to encode yet\n"); return TOOL_Ok; }

    ImageSize size = { _toolOutput->height, _toolOutput->width };
    Rasteron_Queue* queue = RASTERON_QUEUE_ALLOC(_desc->name, _create_size(size.height, size.width), (unsigned)frames);
    for(int f = 0; f < frames; f++){
        Rasteron_Image* image = frameImage((unsigned)f, (unsigned)frames);
        if(image == NULL) break;
        queue_addImg(queue, image, (unsigned short)f);
        RASTERON_DEALLOC(image);
        printf("\r  prepared frame %d/%d", f + 1, frames);
        fflush(stdout);
    }
    printf("\nEncoding %d frames at 60 fps...\n", frames);
    double start = toolTime();
    encodeQueue(queue); // writes Test.mp4, see Util_Win.cpp
    RASTERON_QUEUE_DEALLOC(queue);

    char fileName[1100];
    uniqueName(fileName, sizeof(fileName), _desc->name, ".mp4");
    if(fileExists("Test.mp4") && rename("Test.mp4", fileName) == 0) printf("Encoded %s in %.2fs\n", fileName, toolTime() - start);
    else printf("Encoding finished, check Test.mp4\n");
    _saved = TRUE;
    return TOOL_Ok;
}
#endif

static int cmd_help(int argc, char** argv){
    tool_printHelp((argc > 1)? argv[1] : NULL);
    return TOOL_Ok;
}

static int cmd_quit(int argc, char** argv){ return TOOL_Quit; }

static const Tool_Command _builtins[] = {
    { "set", "<param> <value...>", "change a parameter and re-render", cmd_set },
    { "get", "[param...]", "show parameter values", cmd_get },
    { "apply", "", "commit the current image (filters stack on it, generators mix with it)", cmd_apply },
    { "revert", "[steps|all]", "undo the last set, apply, or tool command", cmd_revert },
    { "reset", "[all]", "restore default parameters ('all' also drops the applied image)", cmd_reset },
    { "save", "[bmp|png|tiff] [name]", "write the current image to a file", cmd_save },
    { "frames", "[count]", "write an animation sequence as numbered bmp files", cmd_frames },
#if TOOL_ENCODE_ENABLED
    { "encode", "[count]", "encode an animation sequence to mp4 (Windows, CUDA builds)", cmd_encode },
#endif
    { "help", "[command|param]", "list commands and parameters", cmd_help },
    { "quit", "", "close the tool (also: exit)", cmd_quit },
};
static const unsigned _builtinCount = TOOL_COUNT(_builtins);

// --------------------------------   Help    -------------------------------- //

static void printQuickHelp(void){
    printf("Available commands:");
    for(unsigned c = 0; c < _builtinCount; c++) printf(" %s", _builtins[c].name);
    for(unsigned c = 0; c < _desc->commandCount; c++) printf(" %s", _desc->commands[c].name);
    printf("\nAvailable parameters (set <param> <value>):");
    for(unsigned p = 0; p < TOOL_COUNT(_commonParams); p++) printf(" %s", _commonParams[p].name);
    for(unsigned p = 0; p < _desc->paramCount; p++) printf(" %s", _desc->params[p].name);
    printf("\nType 'help' for details or 'help <name>' for one command or parameter.\n");
}

void tool_printHelp(const char* topic){
    if(topic != NULL){
        const Tool_Command* command = toolArgs_findCommand(_builtins, _builtinCount, topic);
        if(command == NULL) command = toolArgs_findCommand(_desc->commands, _desc->commandCount, topic);
        if(command != NULL){ toolArgs_printCommand(command); return; }

        const Tool_Param* param = toolArgs_findParam(_commonParams, TOOL_COUNT(_commonParams), topic);
        if(param != NULL){ toolArgs_printParam(&_toolCommon, param); return; }
        param = toolArgs_findParam(_desc->params, _desc->paramCount, topic);
        if(param != NULL){ toolArgs_printParam(_desc->state, param); return; }

        printf("No command or parameter named '%s'\n", topic);
        printQuickHelp();
        return;
    }

    printf("\n%s - %s\n", _desc->title, _desc->summary);
    printf("Type commands and press Enter, separate several with ';'. 'set' may be omitted: '<param> <value>'.\n\n");
    printf("Commands:\n");
    for(unsigned c = 0; c < _builtinCount; c++) toolArgs_printCommand(&_builtins[c]);
    if(_desc->commandCount > 0){
        printf("\n%s commands:\n", _desc->name);
        for(unsigned c = 0; c < _desc->commandCount; c++) toolArgs_printCommand(&_desc->commands[c]);
    }
    printf("\nParameters:\n");
    for(unsigned p = 0; p < TOOL_COUNT(_commonParams); p++) toolArgs_printParam(&_toolCommon, &_commonParams[p]);
    for(unsigned p = 0; p < _desc->paramCount; p++) toolArgs_printParam(_desc->state, &_desc->params[p]);
    printf("\n");
}

// --------------------------------   Dispatch    -------------------------------- //

static int execTokens(int argc, char** argv){
    const char* name = argv[0];

    if(toolArgs_equals(name, "exit") || toolArgs_equals(name, "q")) return TOOL_Quit;
    if(toolArgs_equals(name, "undo")) name = "revert";
    if(toolArgs_equals(name, "show")) name = "get";
    if(toolArgs_equals(name, "?")) name = "help";

    const Tool_Command* builtin = toolArgs_findCommand(_builtins, _builtinCount, name);
    if(builtin != NULL){
        if(builtin->callback != cmd_set && builtin->callback != cmd_get && builtin->callback != cmd_help) flush();
        int result = builtin->callback(argc, argv);
        if(result == TOOL_Fail){ printf("Usage: "); toolArgs_printCommand(builtin); }
        if(result == TOOL_Render) _dirty = TRUE;
        return result;
    }

    if(!TOOL_ENCODE_ENABLED && toolArgs_equals(name, "encode")){
        printf("encode needs a Windows build with CUDA enabled, use 'frames' to write an image sequence instead\n");
        printQuickHelp();
        return TOOL_Ok;
    }

    const Tool_Command* command = toolArgs_findCommand(_desc->commands, _desc->commandCount, name);
    if(command == NULL && strlen(name) > 4 && getFormat(name) != IMG_NonValid) // bare image path loads it
        command = toolArgs_findCommand(_desc->commands, _desc->commandCount, "load");
    if(command != NULL){
        flush();
        char* loadArgs[2] = { "load", argv[0] };
        int isImplicitLoad = !toolArgs_equals(argv[0], command->name);

        historyPush();
        int result = isImplicitLoad? command->callback(2, loadArgs) : command->callback(argc, argv);
        if(result == TOOL_Fail){
            historyRestore();
            _dirty = FALSE;
            printf("Usage: "); toolArgs_printCommand(command);
            return TOOL_Ok;
        }
        if(!historyChanged()) historyDrop();
        if(result == TOOL_Render) _dirty = TRUE;
        return result;
    }

    if(toolArgs_findParam(_commonParams, TOOL_COUNT(_commonParams), name) != NULL || toolArgs_findParam(_desc->params, _desc->paramCount, name) != NULL){
        if(argc < 2){ tool_printHelp(name); return TOOL_Ok; } // bare parameter name shows it
        int result = setParamCommand(argc, argv);
        if(result == TOOL_Render) _dirty = TRUE;
        return result;
    }

    printf("Unknown command or parameter '%s'\n", argv[0]);
    printQuickHelp();
    return TOOL_Ok;
}

int tool_execLine(const char* line){
    char buffer[TOOL_LINE_MAX];
    snprintf(buffer, sizeof(buffer), "%s", line);

    char* segment = buffer;
    int result = TOOL_Ok;
    while(segment != NULL && !_quit){
        char* next = NULL;
        char quote = '\0';
        for(char* c = segment; *c != '\0'; c++){ // split on ';' outside quotes
            if((*c == '"' || *c == '\'') && (quote == '\0' || quote == *c)) quote = (quote == '\0')? *c : '\0';
            else if(*c == ';' && quote == '\0'){ *c = '\0'; next = c + 1; break; }
        }

        char* argv[TOOL_ARGS_MAX];
        int argc = (int)toolArgs_split(segment, argv, TOOL_ARGS_MAX);
        if(argc > 0){
            result = execTokens(argc, argv);
            if(result == TOOL_Quit) _quit = TRUE;
        }
        segment = next;
    }
    if(!_quit) flush();
    return result;
}

static void printPrompt(void){
    printf("%s> ", _desc->name);
    fflush(stdout);
}

// --------------------------------   Porting layer for Tools    -------------------------------- //

#ifdef _WIN32

#define WM_TOOL_LINE (WM_APP + 1)

static HWND _toolWindow = NULL;
static HANDLE _lineDone = NULL;

static void updateTitle(void){
    if(_toolWindow == NULL || _toolOutput == NULL) return;
    char title[256];
    snprintf(title, sizeof(title), "%s - %ux%u", _desc->title, _toolOutput->width, _toolOutput->height);
    SetWindowTextA(_toolWindow, title);
}

static void winPaint(HWND hwnd){
    PAINTSTRUCT paint;
    HDC hdc = BeginPaint(hwnd, &paint);
    RECT client;
    GetClientRect(hwnd, &client);

    const Rasteron_Image* image = _toolOutput;
    if(image != NULL && client.right > 0 && client.bottom > 0){
        double scale = fmin((double)client.right / image->width, (double)client.bottom / image->height);
        int drawWidth = (int)(image->width * scale), drawHeight = (int)(image->height * scale);
        int drawX = (client.right - drawWidth) / 2, drawY = (client.bottom - drawHeight) / 2;

        BITMAPINFO bmapInfo;
        ZeroMemory(&bmapInfo, sizeof(bmapInfo));
        bmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmapInfo.bmiHeader.biWidth = (LONG)image->width;
        bmapInfo.bmiHeader.biHeight = -(LONG)image->height; // top-down rows
        bmapInfo.bmiHeader.biPlanes = 1;
        bmapInfo.bmiHeader.biBitCount = 32;
        bmapInfo.bmiHeader.biCompression = BI_RGB;

        SetStretchBltMode(hdc, (scale < 1.0)? HALFTONE : COLORONCOLOR);
        SetBrushOrgEx(hdc, 0, 0, NULL);
        StretchDIBits(hdc, drawX, drawY, drawWidth, drawHeight, 0, 0, image->width, image->height, image->data, &bmapInfo, DIB_RGB_COLORS, SRCCOPY);
        ExcludeClipRect(hdc, drawX, drawY, drawX + drawWidth, drawY + drawHeight);
    }
    FillRect(hdc, &client, (HBRUSH)GetStockObject(DKGRAY_BRUSH));
    EndPaint(hwnd, &paint);
}

static LRESULT CALLBACK toolWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch(message){
    case WM_CREATE: _toolWindow = hwnd; return 0;
    case WM_TOOL_LINE: {
        char* line = (char*)lParam;
        tool_execLine(line);
        free(line);
        InvalidateRect(hwnd, NULL, FALSE);
        if(_quit) DestroyWindow(hwnd);
        SetEvent(_lineDone);
        return 0;
    }
    case WM_PAINT: winPaint(hwnd); return 0;
    case WM_ERASEBKGND: return 1;
    case WM_SIZE: InvalidateRect(hwnd, NULL, FALSE); return 0;
    case WM_CLOSE: DestroyWindow(hwnd); return 0;
    case WM_DESTROY:
        _toolWindow = NULL;
        _quit = TRUE;
        if(_lineDone != NULL) SetEvent(_lineDone);
        PostQuitMessage(0);
        return 0;
    default: return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

static DWORD WINAPI consoleThread(LPVOID param){
    char line[TOOL_LINE_MAX];
    while(!_quit){
        printPrompt();
        if(fgets(line, sizeof(line), stdin) == NULL) break;
        HWND hwnd = _toolWindow;
        if(hwnd == NULL) break;

        char* copy = toolStrdup(line);
        ResetEvent(_lineDone);
        if(!PostMessage(hwnd, WM_TOOL_LINE, 0, (LPARAM)copy)){ free(copy); break; }
        WaitForSingleObject(_lineDone, INFINITE); // command runs on the window thread
    }
    if(_toolWindow != NULL) PostMessage(_toolWindow, WM_CLOSE, 0, 0);
    return 0;
}

static int runWindow(void){
    _lineDone = CreateEvent(NULL, TRUE, FALSE, NULL);

    int windowSize = (int)(GetSystemMetrics(SM_CYSCREEN) * 0.85);
    if(windowSize > TOOL_HEIGHT || windowSize <= 0) windowSize = TOOL_HEIGHT;
    createWindow(toolWndProc, _desc->title, windowSize, windowSize);
    if(_toolWindow == NULL) return FALSE;
    updateTitle();

    HANDLE thread = CreateThread(NULL, 0, consoleThread, NULL, 0, NULL);
    eventLoop(NULL); // returns once the window is destroyed

    if(thread != NULL) CloseHandle(thread);
    CloseHandle(_lineDone);
    return TRUE;
}

#elif defined __linux__

static Platform_Context _unixContext;
static Atom _wmDelete;
static int _windowOpen = FALSE;

static void updateTitle(void){
    if(!_windowOpen || _toolOutput == NULL) return;
    char title[256];
    snprintf(title, sizeof(title), "%s - %ux%u", _desc->title, _toolOutput->width, _toolOutput->height);
    XStoreName(_unixContext.display, _unixContext.window, title);
}

static void unixDraw(void){
    if(!_windowOpen) return;
    XWindowAttributes attribs;
    XGetWindowAttributes(_unixContext.display, _unixContext.window, &attribs);
    unsigned width = (unsigned)attribs.width, height = (unsigned)attribs.height;
    if(width == 0 || height == 0) return;

    uint32_t* pixels = (uint32_t*)malloc(width * height * sizeof(uint32_t));
    for(unsigned p = 0; p < width * height; p++) pixels[p] = 0x404040; // backdrop

    const Rasteron_Image* image = _toolOutput;
    if(image != NULL){
        double scale = fmin((double)width / image->width, (double)height / image->height);
        unsigned drawWidth = (unsigned)(image->width * scale), drawHeight = (unsigned)(image->height * scale);
        unsigned drawX = (width - drawWidth) / 2, drawY = (height - drawHeight) / 2;
        for(unsigned y = 0; y < drawHeight; y++){
            const uint32_t* srcRow = image->data + (unsigned)((double)y / scale) * image->width;
            uint32_t* dstRow = pixels + (y + drawY) * width + drawX;
            for(unsigned x = 0; x < drawWidth; x++) dstRow[x] = srcRow[(unsigned)((double)x / scale)] & 0x00FFFFFF;
        }
    }

    XImage* ximage = XCreateImage(_unixContext.display, _unixContext.visual, _unixContext.depth, ZPixmap, 0, (char*)pixels, width, height, 32, 0);
    XPutImage(_unixContext.display, _unixContext.window, *_unixContext.gc, ximage, 0, 0, 0, 0, width, height);
    XDestroyImage(ximage); // frees pixels
    XFlush(_unixContext.display);
}

static int runWindow(void){
    if(getenv("DISPLAY") == NULL) return FALSE;
    Display* probe = XOpenDisplay(NULL);
    if(probe == NULL) return FALSE;
    XCloseDisplay(probe);

    createWindow(&_unixContext, _desc->title, TOOL_WIDTH, TOOL_HEIGHT);
    Display* display = _unixContext.display;
    XSelectInput(display, _unixContext.window, ExposureMask | StructureNotifyMask | KeyPressMask | ButtonPressMask);
    _wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, _unixContext.window, &_wmDelete, 1);
    _windowOpen = TRUE;
    updateTitle();

    int displayFd = ConnectionNumber(display);
    char pending[TOOL_LINE_MAX];
    size_t pendingLen = 0;

    printPrompt();
    while(!_quit){
        while(XPending(display) > 0){
            XEvent event;
            XNextEvent(display, &event);
            if(event.type == Expose && event.xexpose.count == 0) unixDraw();
            else if(event.type == ConfigureNotify) unixDraw();
            else if(event.type == ClientMessage && (Atom)event.xclient.data.l[0] == _wmDelete) _quit = TRUE;
        }
        if(_quit) break;

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        FD_SET(displayFd, &fds);
        if(select(((displayFd > STDIN_FILENO)? displayFd : STDIN_FILENO) + 1, &fds, NULL, NULL, NULL) < 0) continue;

        if(FD_ISSET(STDIN_FILENO, &fds)){
            ssize_t bytes = read(STDIN_FILENO, pending + pendingLen, sizeof(pending) - pendingLen - 1);
            if(bytes <= 0){ // end of input
                if(pendingLen > 0){ pending[pendingLen] = '\0'; tool_execLine(pending); }
                break;
            }
            pendingLen += (size_t)bytes;
            pending[pendingLen] = '\0';

            char* lineStart = pending;
            char* newline;
            int ranCommand = FALSE;
            while((newline = strchr(lineStart, '\n')) != NULL && !_quit){ // run each complete line
                *newline = '\0';
                tool_execLine(lineStart);
                unixDraw();
                ranCommand = TRUE;
                lineStart = newline + 1;
            }
            pendingLen = strlen(lineStart);
            memmove(pending, lineStart, pendingLen + 1);
            if(pendingLen >= sizeof(pending) - 1) pendingLen = 0; // overlong line dropped
            if(ranCommand && !_quit) printPrompt();
        }
    }

    _windowOpen = FALSE;
    XDestroyWindow(display, _unixContext.window);
    XCloseDisplay(display);
    return TRUE;
}

#else

static void updateTitle(void){}
static int runWindow(void){ return FALSE; }

#endif

static void runConsole(void){ // headless interactive mode
    char line[TOOL_LINE_MAX];
    while(!_quit){
        printPrompt();
        if(fgets(line, sizeof(line), stdin) == NULL) break;
        tool_execLine(line);
    }
    printf("\n");
}

// --------------------------------   Entry Point    -------------------------------- //

static void printUsage(void){
    printf("Usage: %s [-n|--headless] [-b|--batch] [commands separated by ';']\n", _desc->title);
    printf("  -n, --headless   no preview window, commands are read from the console\n");
    printf("  -b, --batch      run the given commands, save if nothing was saved, then exit\n");
    printf("Example: %s -b \"set seed 42; save png\"\n", _desc->title);
}

int tool_run(const Tool_Desc* desc, int argc, char** argv){
    _desc = desc;
    int batch = FALSE;

    int firstCommand = 1;
    for(; firstCommand < argc; firstCommand++){
        const char* arg = argv[firstCommand];
        if(!strcmp(arg, "-n") || !strcmp(arg, "--headless")) _headless = TRUE;
        else if(!strcmp(arg, "-b") || !strcmp(arg, "--batch")){ batch = TRUE; _headless = TRUE; }
        else if(!strcmp(arg, "-h") || !strcmp(arg, "--help")){
            printUsage();
            memcpy(desc->state, desc->defaults, desc->stateSize);
            tool_printHelp(NULL);
            return 0;
        }
        else break;
    }

    for(unsigned p = 0; p < desc->paramCount; p++) // tool parameters must not hide shared ones
        for(unsigned c = 0; c < TOOL_COUNT(_commonParams); c++)
            if(toolArgs_equals(desc->params[p].name, _commonParams[c].name)) printf("Warning: parameter '%s' is hidden by the shared parameter of the same name\n", desc->params[p].name);

    memcpy(desc->state, desc->defaults, desc->stateSize);
    _toolCommon = _commonDefaults;
    if(desc->init != NULL) desc->init(argc - firstCommand, argv + firstCommand);

#if USE_CUDA_LIBS
    puts("CUDA acceleration enabled!");
#endif
    printf("%s - %s\nType 'help' to list commands and parameters, 'quit' to exit.\n", desc->title, desc->summary);
    _dirty = TRUE;

    if(firstCommand < argc){ // command line arguments run as commands
        char* line = toolArgs_join(argc - firstCommand, argv + firstCommand);
        if(line != NULL){
            printf("%s> %s\n", desc->name, line);
            tool_execLine(line);
            free(line);
        }
    }
    flush();

    if(batch){
        if(!_saved && !_quit){
            char* saveArgs[1] = { "save" };
            cmd_save(1, saveArgs);
        }
    }
    else if(!_quit){
        if(_headless || !runWindow()){
            if(!_headless) printf("No display available, running without a preview window (use save to write images)\n");
            runConsole();
        }
    }

    if(desc->cleanup != NULL) desc->cleanup();
    while(_historyCount > 0) historyDrop();
    baseRelease(_toolBase);
    _toolBase = NULL;
    if(_toolOutput != NULL) RASTERON_DEALLOC(_toolOutput);
    _toolOutput = NULL;
    return 0;
}
