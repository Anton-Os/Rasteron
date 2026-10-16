#include "Tool.h"

#include <stdlib.h>
#include <errno.h>

// --------------------------------   Shared Choice Lists    -------------------------------- //

const char* const _toolSideNames[] = { "none", "top", "bottom", "left", "right", "radial", NULL }; // index - 1 == SIDE_Type
const char* const _toolFlipNames[] = { "none", "clock", "counter", "upside", NULL }; // index - 1 == FLIP_Type

static const char* const _typeNames[] = { "int", "float", "color", "choice", "on|off", "text" };

typedef struct { const char* name; unsigned color; } Tool_NamedColor;

static const Tool_NamedColor _namedColors[] = {
    { "black", 0xFF000000 }, { "white", 0xFFFFFFFF }, { "grey", 0xFF888888 }, { "gray", 0xFF888888 },
    { "dark", 0xFF333333 }, { "light", 0xFFCCCCCC }, { "red", 0xFFFF0000 }, { "green", 0xFF00FF00 },
    { "blue", 0xFF0000FF }, { "yellow", 0xFFFFFF00 }, { "cyan", 0xFF00FFFF }, { "magenta", 0xFFFF00FF },
    { "orange", 0xFFFF8800 }, { "purple", 0xFF8800FF }, { "pink", 0xFFFF88CC }, { "brown", 0xFF7A4A21 },
    { "olive", 0xFF6B8E23 }, { "navy", 0xFF001F5B }, { "teal", 0xFF008080 }, { "gold", 0xFFFFC107 },
    { "sand", 0xFFD8C08A }, { "rust", 0xFFA0452A }, { "lime", 0xFF9EFF3C }, { "none", 0x00000000 },
};

// --------------------------------   Tokenizing    -------------------------------- //

unsigned toolArgs_split(char* line, char** argv, unsigned maxArgs){
    unsigned argc = 0;
    char* src = line;

    while(*src != '\0' && argc < maxArgs){
        while(isspace((unsigned char)*src)) src++; // skip leading space
        if(*src == '\0' || *src == '#') break; // end of input or comment

        char quote = (*src == '"' || *src == '\'')? *src : '\0';
        if(quote != '\0') src++;

        char* dst = src; // compacts quoted text in place
        argv[argc++] = dst;
        while(*src != '\0'){
            if(quote != '\0' && *src == quote){ src++; break; }
            if(quote == '\0' && isspace((unsigned char)*src)) break;
            *dst++ = *src++;
        }
        if(*src != '\0') src++; // step over separator
        *dst = '\0';
    }
    return argc;
}

char* toolArgs_join(int argc, char** argv){
    size_t length = 1;
    for(int a = 0; a < argc; a++) length += strlen(argv[a]) + 3;

    char* line = (char*)malloc(length);
    if(line == NULL) return NULL;
    line[0] = '\0';

    for(int a = 0; a < argc; a++){
        int quoted = (strchr(argv[a], ' ') != NULL || strchr(argv[a], '\t') != NULL) && strchr(argv[a], ';') == NULL;
        if(a > 0) strcat(line, " ");
        if(quoted) strcat(line, "\"");
        strcat(line, argv[a]);
        if(quoted) strcat(line, "\"");
    }
    return line;
}

int toolArgs_equals(const char* str1, const char* str2){
    while(*str1 != '\0' && *str2 != '\0'){
        if(tolower((unsigned char)*str1) != tolower((unsigned char)*str2)) return FALSE;
        str1++; str2++;
    }
    return *str1 == *str2;
}

static int startsWith(const char* str, const char* prefix){
    while(*prefix != '\0'){
        if(tolower((unsigned char)*str) != tolower((unsigned char)*prefix)) return FALSE;
        str++; prefix++;
    }
    return TRUE;
}

static int isRandomWord(const char* str){ return toolArgs_equals(str, "rand") || toolArgs_equals(str, "random"); }

// --------------------------------   Value Parsing    -------------------------------- //

int toolArgs_int(const char* str, int* value){
    if(str == NULL || *str == '\0') return FALSE;
    char* end = NULL;
    errno = 0;
    int isHex = (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) || (str[0] == '-' && str[1] == '0' && (str[2] == 'x' || str[2] == 'X'));
    long result = strtol(str, &end, isHex? 16 : 10);
    if(errno != 0 || *end != '\0') return FALSE;
    *value = (int)result;
    return TRUE;
}

int toolArgs_float(const char* str, double* value){
    if(str == NULL || *str == '\0') return FALSE;
    if(toolArgs_equals(str, "pi")){ *value = 3.14159265358979; return TRUE; }
    if(toolArgs_equals(str, "-pi")){ *value = -3.14159265358979; return TRUE; }

    char* end = NULL;
    errno = 0;
    double result = strtod(str, &end);
    if(errno != 0 || end == str) return FALSE;
    if(*end == '%' && *(end + 1) == '\0') result /= 100.0; // percentages such as 50%
    else if(*end != '\0') return FALSE;
    *value = result;
    return TRUE;
}

int toolArgs_color(const char* str, unsigned* value){
    if(str == NULL || *str == '\0') return FALSE;
    if(isRandomWord(str)){ *value = RAND_COLOR(); return TRUE; }

    for(unsigned c = 0; c < TOOL_COUNT(_namedColors); c++)
        if(toolArgs_equals(str, _namedColors[c].name)){ *value = _namedColors[c].color; return TRUE; }

    const char* hex = str;
    if(*hex == '#') hex++;
    else if(hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) hex += 2;

    size_t len = strlen(hex);
    if(len != 3 && len != 6 && len != 8) return FALSE;
    for(size_t l = 0; l < len; l++) if(!isxdigit((unsigned char)hex[l])) return FALSE;

    unsigned long result = strtoul(hex, NULL, 16);
    if(len == 3) // short form such as f80
        result = ((result & 0xF00) << 12) | ((result & 0xF00) << 8) | ((result & 0x0F0) << 8) | ((result & 0x0F0) << 4) | ((result & 0x00F) << 4) | (result & 0x00F);
    if(len != 8) result |= 0xFF000000; // opaque unless alpha given
    *value = (unsigned)result;
    return TRUE;
}

int toolArgs_bool(const char* str, int current, int* value){
    if(str == NULL) return FALSE;
    if(toolArgs_equals(str, "on") || toolArgs_equals(str, "true") || toolArgs_equals(str, "yes") || toolArgs_equals(str, "1")){ *value = TRUE; return TRUE; }
    if(toolArgs_equals(str, "off") || toolArgs_equals(str, "false") || toolArgs_equals(str, "no") || toolArgs_equals(str, "0")){ *value = FALSE; return TRUE; }
    if(toolArgs_equals(str, "toggle") || toolArgs_equals(str, "flip")){ *value = !current; return TRUE; }
    return FALSE;
}

unsigned toolArgs_choiceCount(const char* const* choices){
    unsigned count = 0;
    if(choices != NULL) while(choices[count] != NULL) count++;
    return count;
}

int toolArgs_choice(const char* str, const char* const* choices){
    if(str == NULL || choices == NULL) return -1;
    unsigned count = toolArgs_choiceCount(choices);

    for(unsigned c = 0; c < count; c++) if(toolArgs_equals(str, choices[c])) return (int)c; // exact match

    int index;
    if(toolArgs_int(str, &index) && index >= 0 && (unsigned)index < count) return index; // numeric index

    int match = -1;
    for(unsigned c = 0; c < count; c++) // unique prefix
        if(startsWith(choices[c], str)){
            if(match >= 0) return -1; // ambiguous
            match = (int)c;
        }
    return match;
}

// --------------------------------   Lookup    -------------------------------- //

const Tool_Param* toolArgs_findParam(const Tool_Param* params, unsigned count, const char* name){
    if(params == NULL || name == NULL) return NULL;
    for(unsigned p = 0; p < count; p++) if(toolArgs_equals(name, params[p].name)) return &params[p];

    const Tool_Param* match = NULL;
    if(strlen(name) < 3) return NULL; // prefixes need at least 3 letters
    for(unsigned p = 0; p < count; p++)
        if(startsWith(params[p].name, name)){
            if(match != NULL) return NULL; // ambiguous
            match = &params[p];
        }
    return match;
}

const Tool_Command* toolArgs_findCommand(const Tool_Command* commands, unsigned count, const char* name){
    if(commands == NULL || name == NULL) return NULL;
    for(unsigned c = 0; c < count; c++) if(toolArgs_equals(name, commands[c].name)) return &commands[c];
    return NULL;
}

// --------------------------------   Setting Values    -------------------------------- //

static void printExpected(const Tool_Param* param){
    printf("  expected: %s ", param->name);
    if(param->type == PARAM_Enum){
        unsigned count = toolArgs_choiceCount(param->choices);
        putchar('<');
        for(unsigned c = 0; c < count; c++) printf("%s%s", (c > 0)? "|" : "", param->choices[c]);
        putchar('>');
    }
    else printf("<%s>", _typeNames[param->type]);
    if(param->count > 1) printf(" x%d (one value per axis or item)", param->count);
    if((param->type == PARAM_Int || param->type == PARAM_Float) && param->min != param->max)
        printf("  range %.10g to %.10g", param->min, param->max);
    if(param->type == PARAM_Color) printf("  e.g. ff8800, #ff8800, 0xFFFF8800, red, rand");
    putchar('\n');
}

int toolArgs_setParam(void* state, const Tool_Param* param, int argc, char** argv){
    char* field = (char*)state + param->offset;

    if(argc < 1){
        printf("Missing value for '%s'\n", param->name);
        printExpected(param);
        return FALSE;
    }

    if(param->type == PARAM_Text){
        char* text = field;
        text[0] = '\0';
        for(int a = 0; a < argc; a++){
            if(strlen(text) + strlen(argv[a]) + 2 >= TOOL_TEXT_MAX) break;
            if(a > 0) strcat(text, " ");
            strcat(text, argv[a]);
        }
        return TRUE;
    }

    if(argc > param->count){
        printf("Too many values for '%s' (takes %d)\n", param->name, param->count);
        printExpected(param);
        return FALSE;
    }

    int ranged = param->min != param->max;
    for(int a = 0; a < argc; a++){
        const char* arg = argv[a];
        int ok = TRUE;

        switch(param->type){
        case PARAM_Int: {
            int value;
            if(isRandomWord(arg)) value = ranged? (int)(param->min + floor(((double)rand() / ((double)RAND_MAX + 1.0)) * (param->max - param->min + 1.0))) : rand();
            else if(!toolArgs_int(arg, &value)) { ok = FALSE; break; }
            if(ranged && (value < param->min || value > param->max)) { ok = FALSE; break; }
            *((int*)field + a) = value;
        } break;
        case PARAM_Float: {
            double value;
            if(isRandomWord(arg)) value = ranged? param->min + ((double)rand() / RAND_MAX) * (param->max - param->min) : (double)rand() / RAND_MAX;
            else if(!toolArgs_float(arg, &value)) { ok = FALSE; break; }
            if(ranged && (value < param->min || value > param->max)) { ok = FALSE; break; }
            *((double*)field + a) = value;
        } break;
        case PARAM_Color: {
            unsigned value;
            if(!toolArgs_color(arg, &value)) { ok = FALSE; break; }
            *((unsigned*)field + a) = value;
        } break;
        case PARAM_Enum: {
            int value = toolArgs_choice(arg, param->choices); // a choice literally named random wins
            if(value < 0 && isRandomWord(arg)) value = (int)(rand() % toolArgs_choiceCount(param->choices));
            if(value < 0) { ok = FALSE; break; }
            *((int*)field + a) = value;
        } break;
        case PARAM_Bool: {
            int value;
            if(isRandomWord(arg)) value = rand() % 2;
            else if(!toolArgs_bool(arg, *((int*)field + a), &value)) { ok = FALSE; break; }
            *((int*)field + a) = value;
        } break;
        default: ok = FALSE; break;
        }

        if(!ok){
            printf("Invalid value '%s' for '%s'\n", arg, param->name);
            printExpected(param);
            return FALSE;
        }
    }
    return TRUE;
}

// --------------------------------   Printing    -------------------------------- //

void toolArgs_printValue(const void* state, const Tool_Param* param, FILE* stream){
    const char* field = (const char*)state + param->offset;

    if(param->type == PARAM_Text){ fprintf(stream, "\"%s\"", field); return; }

    for(unsigned short v = 0; v < param->count; v++){
        if(v > 0) fputc(' ', stream);
        switch(param->type){
        case PARAM_Int: fprintf(stream, "%d", *((const int*)field + v)); break;
        case PARAM_Float: fprintf(stream, "%g", *((const double*)field + v)); break;
        case PARAM_Color: fprintf(stream, "%08X", *((const unsigned*)field + v)); break;
        case PARAM_Bool: fprintf(stream, "%s", *((const int*)field + v)? "on" : "off"); break;
        case PARAM_Enum: {
            int index = *((const int*)field + v);
            unsigned count = toolArgs_choiceCount(param->choices);
            fprintf(stream, "%s", (index >= 0 && (unsigned)index < count)? param->choices[index] : "?");
        } break;
        default: break;
        }
    }
}

void toolArgs_printParam(const void* state, const Tool_Param* param){
    printf("  %-11s = ", param->name);
    toolArgs_printValue(state, param, stdout);

    printf("\n%15s", "");
    if(param->type == PARAM_Enum){
        unsigned count = toolArgs_choiceCount(param->choices);
        for(unsigned c = 0; c < count; c++) printf("%s%s", (c > 0)? "|" : "", param->choices[c]);
    }
    else {
        printf("%s", _typeNames[param->type]);
        if(param->count > 1 && param->type != PARAM_Text) printf(" x%d", param->count);
        if((param->type == PARAM_Int || param->type == PARAM_Float) && param->min != param->max) printf(" [%.10g, %.10g]", param->min, param->max);
    }
    printf("  %s\n", param->desc);
}

void toolArgs_printCommand(const Tool_Command* command){
    char usage[128];
    snprintf(usage, sizeof(usage), "%s %s", command->name, (command->usage != NULL)? command->usage : "");
    printf("  %-30s %s\n", usage, command->desc);
}
