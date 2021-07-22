#include "Loader.h"

// Start from zero to index arrays easily
enum NBR_CellFlags {
	NBR_Bot_Right = 0,
	NBR_Bot = 1,
	NBR_Bot_Left = 2,
	NBR_Right = 3,
	NBR_Left = 4,
	NBR_Top_Right = 5,
	NBR_Top = 6,
	NBR_Top_Left = 7,
};

typedef uint8_t nebrCheckFlags; // A number with the bitflags set above

typedef struct {
    const uint32_t* target;
    uint32_t** nebrs;
    nebrCheckFlags nebrExistFlags;
    // unsigned short nebrCount;
} NebrTable;

typedef struct {
	uint32_t count;
	NebrTable* tables;
} NebrTable_List;

nebrCheckFlags checkExistNebrs(uint32_t index, uint32_t imgWidth, uint32_t imgHeight); // checks which cell neighbors exist
NebrTable_List* genNebrTables(const uint32_t* raster, uint32_t imgWidth, uint32_t imgHeight);
void printNebrTables(const NebrTable_List* nebrTables); // For debug purposes
void delNebrTables(NebrTable_List* nebrTables);