#include "Font.h"

static void drawFontOffset(Rasteron_Image* image, FT_Bitmap* ftBitmap, int x, int y){
    int x_max = x + ftBitmap->width;
    int y_max = y + ftBitmap->rows;

	// Implement drawing call

    return;
}

void initFreeType(FT_Library* library){
    int error = FT_Init_FreeType(library);
    if(error) puts("Error occured initializing freetype library!");
}

Rasteron_Image* bakeImgText(const Rasteron_FormatText* textObj, FT_Library* library, uint32_t width, uint32_t height){
    FT_Face face;
    int error = FT_New_Face(*library, textObj->fontFileName, 0, &face);
	error = FT_Set_Char_Size(face, 0, 16 * 64, FONT_RES, FONT_RES);
    // error = FT_Set_Char_Size(face, 50 * 64, 0, 100, 0); // scale size based on FreeType example
    if(error) puts("Error occured baking text");

    Rasteron_Image* textImage = createImgBlank(width, height, textObj->bkColor);
    textImage->name = "text";
    
    FT_Matrix matrix = {0, 0, 0, 0}; // no transform
    FT_Vector pen = {0, 0}; // no offset
    pen.x = 20; pen.y = 20; 

    for(unsigned t = 0; t < strlen(textObj->text); t++){
        FT_Set_Transform(face, &matrix, &pen);

        error = FT_Load_Char(face, textObj->text[t], FT_LOAD_RENDER);
        if(error) puts("error loading glyph!");

        drawFontOffset(textImage, &face->glyph->bitmap, face->glyph->bitmap_left, height - face->glyph->bitmap_top);
        
        pen.x += face->glyph->advance.x;
        pen.y += face->glyph->advance.y;
    }

    FT_Done_Face(face);
    return textImage;
}

void cleanupFreeType(FT_Library* library){
    FT_Done_FreeType(*library);
}