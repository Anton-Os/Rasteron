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

nebrCheckFlags check_existNebrs(uint32_t imgWidth, uint32_t imgHeight, uint32_t index){
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