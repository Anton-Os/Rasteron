#include "Cellwise.h"

static void setFlagBit(nebrCheckFlags* target, enum NBR_CellFlags flagBit){
    *(target) = (*(target) | (1 << (flagBit)));
    return;
}

static void clearFlagBit(nebrCheckFlags* target, enum NBR_CellFlags flagBit){
    *(target) = (*(target) & (~(1 << (flagBit))));
    return;
}

// nebrCheckFlags check_colorRangeNebrs_BW(PixelCRange_BW pixCRange, uint8_t greyRef, enum NBR_ColorRange_Mode mode){
/*    nebrCheckFlags flags = 0x00;
    
    switch(mode){

    }
}*/

nebrCheckFlags check_existNebrs(uint32_t index, uint32_t imgWidth, uint32_t imgHeight){
    nebrCheckFlags flags = 0xFF;

    if(index < imgWidth){
        clearFlagBit(&flags, NBR_Top_Left);
        clearFlagBit(&flags, NBR_Top);
        clearFlagBit(&flags, NBR_Top_Right);
    } else if((imgWidth * imgHeight) - index < imgWidth){
        clearFlagBit(&flags, NBR_Bot_Left);
        clearFlagBit(&flags, NBR_Bot);
        clearFlagBit(&flags, NBR_Bot_Right);
    }

    if(index % imgWidth == 0){
        clearFlagBit(&flags, NBR_Top_Left);
        clearFlagBit(&flags, NBR_Left);
        clearFlagBit(&flags, NBR_Bot_Left);
    } else if(index % imgWidth == imgWidth - 1){
        clearFlagBit(&flags, NBR_Top_Right);
        clearFlagBit(&flags, NBR_Right);
        clearFlagBit(&flags, NBR_Bot_Right);
    }

    return flags;
}

static uint32_t* findNeighbor(uint32_t* raster, uint32_t index, 
                              uint32_t imgWidth, uint32_t imgHeight,
                              enum NBR_CellFlags whichNebr){
    const uint32_t* rasterPixCurrent = raster + index;

	switch (whichNebr) {
		case NBR_Bot_Right:
			return rasterPixCurrent + imgWidth + 1;
		case NBR_Bot:
			return rasterPixCurrent + imgWidth;
		case NBR_Bot_Left:
			return rasterPixCurrent + imgWidth - 1;
		case NBR_Right:
			return rasterPixCurrent + 1;
		case NBR_Left:
			return rasterPixCurrent - 1;
		case NBR_Top_Right:
			return rasterPixCurrent - imgWidth + 1;
		case NBR_Top:
			return rasterPixCurrent - imgWidth;
		case NBR_Top_Left:
			return rasterPixCurrent - imgWidth - 1;

	}
    return 0;
}

/* NebrTable* gen_nebrTables(const uint32_t* raster, uint32_t imgWidth, uint32_t imgHeight){
    NebrTable* nebrTable = (uint32_t*)malloc(imgHeight * imgWidth);
    
    NebrTable* nebrTableCurrent = nebrTable; // Points to first member of the neighbor tables
    const uint32_t* rasterPixCurrent = raster; // Points to the first pixel

    for(unsigned i = 0; i < imgHeight * imgWidth; i++){
        nebrTableCurrent = nebrTable + i; // Points to i'th member in the neighbor tables
        rasterPixCurrent = raster + i;

        nebrTableCurrent->nebrExistFlags = check_existNebrs(i, imgWidth, imgHeight);
        nebrTableCurrent->target = rasterPixCurrent;

		unsigned short nebrCount = 0;
		for (unsigned short n = 0; n < 8; n++)
			if (nebrTableCurrent->nebrExistFlags & (1 << (n))) // Traverse through all flags and increment if present
				nebrCount++;
		// Get the number of bit flags in nebrExistFlags

		nebrTableCurrent->nebrs = (malloc(nebrCount);

		for(unsigned 
			switch (n) { // Each element n corresponds to a neighbor enum index
				case NBR_Bot_Right:
					if(nebrTableCurrent->nebrExistFlags &(1 << (NBR_Bot_Right)))

			}

        // unsigned short nebrCount = 0;
		//nebrCheckFlags checkFlags = check_existNebrs(i, imgWidth, imgHeight);
		//for(unsigned short n = 0; n < 8; n++) // Loops through members of checkFlags and updates current NebrTable structure 
			//if(*)
    }

    return nebrTable;
} */

/* void del_nebrTables(NebrTable* nebrTable){
    free(nebrTable);
} */


NebrTable_List* gen_nebrTables(const uint32_t* raster, uint32_t imgWidth, uint32_t imgHeight) {
	NebrTable_List* list = (NebrTable_List*)malloc(sizeof(NebrTable_List));
}

void del_nebrTables(NebrTable_List* nebrTables) {
	free(nebrTables);
}