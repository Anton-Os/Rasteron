#ifndef RASTERON_TOOL_H

#include <stddef.h>

#include "Util_OS.h"

#include "Loader.h"

// --------------------------------   Directives for Tools    -------------------------------- //
//
//      Tools are command-driven counterparts of the keypress-driven tests. Each tool owns a plain
//      state struct described by a parameter table, and the shared layer in Tool.c / ToolArgs.c
//      handles the console, the preview window, undo history, saving, and encoding.

#ifndef TOOL_WIDTH
#define TOOL_WIDTH 1300 // default output width, matches tests
#endif
#ifndef TOOL_HEIGHT
#define TOOL_HEIGHT 1300 // default output height, matches tests
#endif

#define TOOL_LINE_MAX 2048 // longest command line accepted
#define TOOL_ARGS_MAX 64 // most tokens in a single command
#define TOOL_TEXT_MAX 256 // storage for PARAM_Text values
#define TOOL_HISTORY_MAX 48 // undo depth for revert
#define TOOL_FRAMES_DEFAULT 60 // default frame count for encode and frames
#define TOOL_FRAMES_MAX 600 // upper bound for encode and frames
#define TOOL_SIZE_MAX 8192 // upper bound for generated image dimensions

#if defined(_WIN32) && USE_CUDA_LIBS
#define TOOL_ENCODE_ENABLED 1 // encode command available on Windows CUDA builds
#else
#define TOOL_ENCODE_ENABLED 0
#endif

// --------------------------------   Parameters    -------------------------------- //

typedef enum {
    PARAM_Int = 0, // stored as int
    PARAM_Float = 1, // stored as double
    PARAM_Color = 2, // stored as unsigned, accepts hex, #hex, 0xAARRGGBB, names, and rand
    PARAM_Enum = 3, // stored as int index into choices
    PARAM_Bool = 4, // stored as int, accepts on/off/true/false/yes/no/1/0/toggle
    PARAM_Text = 5 // stored as char[TOOL_TEXT_MAX]
} PARAM_Type;

typedef struct {
    const char* name; // name typed after set
    PARAM_Type type;
    size_t offset; // offsetof() inside the tool state struct
    unsigned short count; // number of consecutive values, 1 for scalars
    double min; double max; // accepted numeric range, ignored when min == max
    const char* const* choices; // NULL terminated names for PARAM_Enum
    const char* desc; // shown in help
} Tool_Param;

#define TOOL_PARAM(name, type, stateType, field, count, min, max, choices, desc) \
    { name, type, offsetof(stateType, field), count, min, max, choices, desc }

// --------------------------------   Commands    -------------------------------- //

enum TOOL_Result {
    TOOL_Fail = -1, // bad input, help is printed for the command
    TOOL_Ok = 0, // nothing needs to change on screen
    TOOL_Render = 1, // state changed, output is regenerated
    TOOL_Quit = 2 // leave the tool
};

typedef int (*toolCmdCallback)(int argc, char** argv); // argv[0] is the command name

typedef struct {
    const char* name;
    const char* usage; // argument usage shown in help
    const char* desc;
    toolCmdCallback callback;
} Tool_Command;

// --------------------------------   Tool Description    -------------------------------- //

typedef Rasteron_Image* (*toolGenCallback)(ImageSize size, const Rasteron_Image* baseImg); // baseImg is the applied image or NULL
typedef void (*toolAnimCallback)(unsigned frame, unsigned frameCount); // mutates state for an animation frame
typedef int (*toolApplyCallback)(void); // return TRUE when output must be regenerated after apply

typedef struct {
    const char* name; // short name used for prompt and file names, e.g. "radial"
    const char* title; // window title
    const char* summary; // one line shown at the top of help

    void* state; // pointer to the tool state struct
    const void* defaults; // pointer to a const copy of default state
    size_t stateSize;

    const Tool_Param* params; unsigned paramCount;
    const Tool_Command* commands; unsigned commandCount;

    toolGenCallback generate; // required
    toolAnimCallback animate; // optional, used by encode and frames
    toolApplyCallback onApply; // optional
    void (*init)(int argc, char** argv); // optional, called once before startup commands
    void (*cleanup)(void); // optional
} Tool_Desc;

#define TOOL_COUNT(array) (unsigned)(sizeof(array) / sizeof(array[0]))

// --------------------------------   Shared Tool Layer (Tool.c)    -------------------------------- //

typedef struct {
    int size[2]; // output width and height
    int seed; // random seed applied before each render
} Tool_Common;

extern Tool_Common _toolCommon;

int tool_run(const Tool_Desc* desc, int argc, char** argv); // entry point used by each tool main
int tool_execLine(const char* line); // executes one line of commands separated by ';'

void tool_printHelp(const char* topic); // full help, or help for a command or parameter

ImageSize tool_size(void); // current output size
const Rasteron_Image* tool_getBase(void); // applied image, may be NULL
const Rasteron_Image* tool_getOutput(void); // currently displayed image, may be NULL
void tool_setBase(Rasteron_Image* image); // takes ownership, records history
void tool_render(void); // regenerates output now

unsigned tool_rand(void); // random value from the render seed
double tool_randf(void); // random value in [0, 1]

// --------------------------------   Argument Helpers (ToolArgs.c)    -------------------------------- //

unsigned toolArgs_split(char* line, char** argv, unsigned maxArgs); // tokenizes in place, honours quotes
char* toolArgs_join(int argc, char** argv); // joins process arguments into one command line, caller frees
int toolArgs_equals(const char* str1, const char* str2); // case insensitive comparison

int toolArgs_int(const char* str, int* value);
int toolArgs_float(const char* str, double* value);
int toolArgs_color(const char* str, unsigned* value);
int toolArgs_bool(const char* str, int current, int* value);
int toolArgs_choice(const char* str, const char* const* choices); // index, or -1 when no match
unsigned toolArgs_choiceCount(const char* const* choices);

const Tool_Param* toolArgs_findParam(const Tool_Param* params, unsigned count, const char* name);
const Tool_Command* toolArgs_findCommand(const Tool_Command* commands, unsigned count, const char* name);

int toolArgs_setParam(void* state, const Tool_Param* param, int argc, char** argv); // TRUE on success, prints error otherwise
void toolArgs_printValue(const void* state, const Tool_Param* param, FILE* stream);
void toolArgs_printParam(const void* state, const Tool_Param* param); // one help line with current value
void toolArgs_printCommand(const Tool_Command* command); // one help line

extern const char* const _toolSideNames[]; // none top bottom left right radial, matches SIDE_Type + 1
extern const char* const _toolFlipNames[]; // none clock counter upside, matches FLIP_Type + 1

#define RASTERON_TOOL_H
#endif
