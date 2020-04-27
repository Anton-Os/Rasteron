#include "Cellwise.h"

#include "stdio.h" // For debug purposes

void print_nebrTables(const NebrTable_List* nebrTables) {
	for (NebrTable* nebrTable_current = nebrTables->tables;
		nebrTable_current != nebrTables->tables + nebrTables->count;
		nebrTable_current++) {

		unsigned short nebrCount = 0;
		for (unsigned short n = 0; n < 8; n++) // Determine number of neighbors based on nebrExistFlags
			if (nebrTable_current->nebrExistFlags & (1 << (n))) // Traverse through all flags and increment if present
				nebrCount++;

		if (nebrCount < 8) printf("N:%d ", nebrCount);
		if (nebrCount == 3) puts("\n");
	}	
}

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
    } else if((imgWidth * imgHeight) - index - 1 < imgWidth){
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

static uint32_t* findNeighbor(const uint32_t* raster, uint32_t index, 
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


NebrTable_List* gen_nebrTables(const uint32_t* raster, uint32_t imgWidth, uint32_t imgHeight) {
	NebrTable_List* list = (NebrTable_List*)malloc(sizeof(NebrTable_List));
	list->count = imgWidth * imgHeight;
	list->tables = NULL;
	list->tables = (NebrTable*)malloc(imgWidth * imgHeight * sizeof(NebrTable));
	
	const uint32_t* rasterPix_current = raster; // start with the first raster pixel
	uint32_t rasterPix_index = 0;
	for (NebrTable* nebrTable_current = list->tables; // start with the first table
		nebrTable_current != list->tables + (imgWidth * imgHeight); // pointer to end on in the mem space
		nebrTable_current++){ // move to next table pointer

		nebrTable_current->target = rasterPix_current;
		nebrTable_current->nebrExistFlags = check_existNebrs(rasterPix_index, imgWidth, imgHeight);

		unsigned short nebrCount = 0;
		for (unsigned short n = 0; n < 8; n++) // Determine number of neighbors based on nebrExistFlags
			if (nebrTable_current->nebrExistFlags & (1 << (n))) // Traverse through all flags and increment if present
				nebrCount++; 

		nebrTable_current->nebrs = (uint32_t**)malloc(nebrCount * sizeof(uint32_t*));
		unsigned short nIndex = 0; // maps n (neighbor) to the correct location in the nebrs structure
		for (unsigned short n = 0; n < 8; n++) // Determine number of neighbors based on nebrExistFlags
			if (nebrTable_current->nebrExistFlags & (1 << (n))) // If neighbor exists we determine which neighbor it is
				switch (n) {
				case NBR_Bot_Right:
					*(nebrTable_current->nebrs + nIndex) = findNeighbor(raster, rasterPix_index, imgWidth, imgHeight, NBR_Bot_Right);
					nIndex++;
					break;
				case NBR_Bot:
					*(nebrTable_current->nebrs + nIndex) = findNeighbor(raster, rasterPix_index, imgWidth, imgHeight, NBR_Bot);
					nIndex++;
					break;
				case NBR_Bot_Left:
					*(nebrTable_current->nebrs + nIndex) = findNeighbor(raster, rasterPix_index, imgWidth, imgHeight, NBR_Bot_Left);
					nIndex++;
					break;
				case NBR_Right:
					*(nebrTable_current->nebrs + nIndex) = findNeighbor(raster, rasterPix_index, imgWidth, imgHeight, NBR_Right);
					nIndex++;
					break;
				case NBR_Left :
					*(nebrTable_current->nebrs + nIndex) = findNeighbor(raster, rasterPix_index, imgWidth, imgHeight, NBR_Left);
					nIndex++;
					break;
				case NBR_Top_Right:
					*(nebrTable_current->nebrs + nIndex) = findNeighbor(raster, rasterPix_index, imgWidth, imgHeight, NBR_Top_Right);
					nIndex++;
					break;
				case NBR_Top:
					*(nebrTable_current->nebrs + nIndex) = findNeighbor(raster, rasterPix_index, imgWidth, imgHeight, NBR_Top);
					nIndex++;
					break;
				case NBR_Top_Left:
					*(nebrTable_current->nebrs + nIndex) = findNeighbor(raster, rasterPix_index, imgWidth, imgHeight, NBR_Top_Left);
					nIndex++;
					break;
				default:
					break;
				}
			else
				continue;

		rasterPix_index++;
		rasterPix_current++; // move to next pixel pointer
	}
	return list; // Return the structure that we generated
}

void del_nebrTables(NebrTable_List* nebrTables) {
	NebrTable* nebrTable_current = nebrTables->tables;
	for (NebrTable* nebrTable_current = nebrTables->tables;
		nebrTable_current != nebrTables->tables + nebrTables->count; 
		nebrTable_current++) {
		// free(nebrTables->tables->nebrs); // Free all neighbors
		free(nebrTable_current->nebrs);
	}
	free(nebrTables->tables);
	free(nebrTables);
}