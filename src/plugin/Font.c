#include "Font.h"

static void drawFontOffset(Rasteron_Image* image, FT_Face* face, const Rasteron_TextSizeParams* sizeParams){
    // TODO: Implement copying code here
    return;
}

void initFreeType(FT_Library* library){
    int error = FT_Init_FreeType(library);
    if(error) puts("Error occured initializing freetype library!");
}

Rasteron_Image* bakeImgText(const Rasteron_FormatText* textObj, FT_Library* library, uint32_t width, uint32_t height){
    FT_Face face;
    int error = FT_New_Face(*library, textObj->fontFileName, 0, &face);
    if(error) puts("Error occured baking text");
    // TODO: get size parameters here
    // error = FT_Set_Char_Size() // scale size based on width and height of new image

    Rasteron_Image* textImage = createImgBlank(width, height, textObj->bkColor);
    textImage->name = "text";
    // TODO: populate image data with bitmap text

    int charmapsCount = face->num_charmaps; // for testing
    Rasteron_TextSizeParams sizeParams;
    sizeParams.height = height;
    sizeParams.width = width / strlen(textObj->text);
    for(unsigned t = 0; t < strlen(textObj->text); t++){
        /* int charcode = (int)(*(textObj->text + t));
        unsigned glyphIndex = FT_Get_Char_Index(face, charcode);
        error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
        error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL); // converts to anti-aliased bitmap */
        error = FT_Load_Char(face, textObj->text[t], FT_LOAD_RENDER);
        if(error) puts("error loading glyph!");

        sizeParams.xOffset = sizeParams.width * t; // increment to next postition
        drawFontOffset(textImage, face, &sizeParams);
    }

    FT_Done_Face(face);
    return textImage;
}

void cleanupFreeType(FT_Library* library){
    FT_Done_FreeType(*library);
}