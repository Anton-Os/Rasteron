#include "Catalouge.h"

Rasteron_Image* supernestImgOp(double x, double y){
    Rasteron_Image* nestedImgs[9] = {
        solidImgOp((ImageSize){ 768, 768 }, 0xFFFF00FF),
        solidImgOp((ImageSize){ 512, 512 }, 0xFF00FFFF),
        solidImgOp((ImageSize){ 384, 384 }, 0xFFFF0000),
        solidImgOp((ImageSize){ 256, 256 }, 0xFF00FF00),
        solidImgOp((ImageSize){ 192, 192 }, 0xFF0000FF),
        solidImgOp((ImageSize){ 128, 128 }, 0xFFFC6B03),
        solidImgOp((ImageSize){ 96, 96 }, 0xFFFC03B5),
        solidImgOp((ImageSize){ 64, 64 }, 0xFF7303FC),
        solidImgOp((ImageSize){ 32, 32 }, 0xFFFFFFFF),
    };

    Rasteron_Image* combineImgs[8];
    combineImgs[0] = insertImgOp(nestedImgs[7], nestedImgs[8], x, y);
    for(unsigned i = 1; i < 8; i++) 
        combineImgs[i] = insertImgOp(combineImgs[i - 1], nestedImgs[8 - i - 1], x, y);

    Rasteron_Image* outerImg = solidImgOp((ImageSize){ 1024, 1024 }, 0xFFFFFF00);
    Rasteron_Image* finalImg = insertImgOp(combineImgs[7], outerImg, x, y);

    for(unsigned i = 0; i < 9; i++) dealloc_image(nestedImgs[i]);
    for(unsigned i = 0; i < 8; i++) dealloc_image(combineImgs[i]);  
    dealloc_image(outerImg);

    return finalImg;
}

Rasteron_Image* slicediceImgOp(enum FLIP_Type flip, int isMirrored, enum CROP_Type crop, double factor){ 
    char fullImagePath[1024];
    genFullFilePath("Zero+.png", &fullImagePath);

    Rasteron_Image* loadedImg = loadImgOp(fullImagePath);
    Rasteron_Image* flipImg = flipImgOp(loadedImg, flip);
    Rasteron_Image* mirrorIfImg = (isMirrored)? mirrorImgOp(flipImg) : copyImgOp(flipImg);
    Rasteron_Image* finalImg = cropImgOp(mirrorIfImg, crop, factor);

    dealloc_image(loadedImg);
    dealloc_image(flipImg);
    dealloc_image(mirrorIfImg);

    return finalImg;
}

Rasteron_Image* distillingImgOp(){ 
    // TODO: Add body
    return solidImgOp((ImageSize){ 1024, 1024 }, 0xFFFFFF00); 
}

Rasteron_Image* overlayerImgOp(){ 
    // TODO: Add body
    return solidImgOp((ImageSize){ 1024, 1024 }, 0xFFFFFF00); 
} 

Rasteron_Image* multiNoiseImgOp(){ 
    // TODO: Add body
    return solidImgOp((ImageSize){ 1024, 1024 }, 0xFFFFFF00);
}

Rasteron_Image* cellAutomataImgOp(){
    // TODO: Add body 
    return solidImgOp((ImageSize){ 1024, 1024 }, 0xFFFFFF00); 
}

Rasteron_Image* proxPatternImgOp(){ 
    // TODO: Add body
    return solidImgOp((ImageSize){ 1024, 1024 }, 0xFFFFFF00); 
}

Rasteron_Image* fractalsImgOp(){ 
    // TODO: Add body
    return solidImgOp((ImageSize){ 1024, 1024 }, 0xFFFFFF00); 
}
