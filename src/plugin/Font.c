#include "Font.h"

static void drawFontOffset(Rasteron_Image* image, FT_Bitmap* ftBmap, uint32_t color, int x, int y){
	uint32_t imageOff = x + (image->width * y); // starting pixel for drawing
	if(ftBmap->rows > 0 && ftBmap->width > 0)
		for (unsigned r = 0; r < ftBmap->rows; r++) {
			for (unsigned w = 0; w < ftBmap->width; w++) {
				uint32_t bmapColor = ftBmap->buffer[(w * ftBmap->width) + r];
				if(bmapColor > 0)
					*(image->data + imageOff) = color;
				imageOff++; // move to adjascent pixel
			}
			// imageOff += image->width - ftBmap->width; // sw
			imageOff += image->width - ftBmap->width; // move to start of next row
		}
	else return; // space detected, perform processing here
}

void initFreeType(FT_Library* library){
    int error = FT_Init_FreeType(library);
    if(error) puts("Error occured initializing freetype library!");
}

Rasteron_Image* bakeImgText(const Rasteron_FormatText* textObj, FT_Library* library, uint32_t height, uint32_t width){
    FT_Face face;
    int error = FT_New_Face(*library, textObj->fontFileName, 0, &face);
	error = FT_Set_Char_Size(face, 0, 16 * 64, FONT_RES, FONT_RES);
    // error = FT_Set_Char_Size(face, 50 * 64, 0, 100, 0); // scale size based on FreeType example
    if(error) puts("Error occured baking text");

    Rasteron_Image* textImage = createImgBlank(width, height, textObj->bkColor);
    textImage->name = "text";
    
	int pen_x = FONT_PEN_OFF;
	int pen_y = 0; // int pen_y = FONT_PEN_OFF;

    for(unsigned t = 0; t < strlen(textObj->text); t++){
		char glyphRef = textObj->text[t]; // for viewing current character being processed
        error = FT_Load_Char(face, (FT_ULong)glyphRef, FT_LOAD_RENDER);
        if(error) puts("error loading glyph!");

		drawFontOffset(
			textImage,
			&face->glyph->bitmap,
			textObj->fgColor,
			pen_x + face->glyph->bitmap_left,
			(pen_y + (height / 2)) - face->glyph->bitmap_top // making text centered
		);
		pen_x += face->glyph->advance.x >> 6; // increment pen to next position
    }

    FT_Done_Face(face);
    return textImage;
}

void cleanupFreeType(FT_Library* library){
    FT_Done_FreeType(*library);
}