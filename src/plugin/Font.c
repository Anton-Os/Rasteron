#include "Font.h"

static void getImageTextParams(Rasteron_Image* refImage, Rasteron_TextSizeProperties* sizeParams, uint32_t color, int isInvert) {
	// setting all paramteres to initial values
	sizeParams->xMin = (!isInvert)? refImage->width : refImage->height;
	sizeParams->yMin = (!isInvert)? refImage->width : refImage->width;
	sizeParams->xMax = 0; sizeParams->yMax = 0;

	for(unsigned r = 0; r < refImage->height; r++)
		for (unsigned c = 0; c < refImage->width; c++)
			if (*(refImage->data + (r * refImage->width) + c) == color) {
				if (c < sizeParams->xMin) sizeParams->xMin = c;
				else if (c > sizeParams->xMax) sizeParams->xMax = c;

				if (r < sizeParams->yMin) sizeParams->yMin = r;
				else if (r > sizeParams->yMax) sizeParams->yMax = r;
			}
}

static void cropTextImage(Rasteron_Image* refImage, Rasteron_Image* fontImage, Rasteron_TextSizeProperties sizeParams) {
	unsigned writeOffset = 0;

	for (unsigned r = 0; r < refImage->height; r++)
		for (unsigned c = 0; c < refImage->width; c++)
			if (r >= sizeParams.yMin && r < sizeParams.yMax && c >= sizeParams.xMin && c < sizeParams.xMax && writeOffset < (fontImage->width * fontImage->height)) {
				*(fontImage->data + writeOffset) = *(refImage->data + (r * refImage->width) + c);
				writeOffset++;
			}
}

static void drawGlyph(Rasteron_Image* image, FT_Bitmap* ftBmap, uint32_t color, int x, int y){
	uint32_t imageOff = x + (image->width * y); // starting pixel for drawing
	if(ftBmap->rows > 0 && ftBmap->width > 0)
		for (unsigned r = 0; r < ftBmap->rows; r++) {
			for (unsigned w = 0; w < ftBmap->width; w++) {
				uint32_t bmapColor = ftBmap->buffer[(r * ftBmap->width) + w];
				if(bmapColor > 0)
					*(image->data + imageOff) = color;
				imageOff++; // move to adjascent pixel
			}
			imageOff += image->width - ftBmap->width; // move to start of next row
		}
	else return; // skips space
}

static Rasteron_Image* bakeImgTextCustom(FT_Library* library, const Rasteron_FormatText* textObj, unsigned scale, int isInvert){
    FT_Face face;
    int error = FT_New_Face(*library, textObj->fileName, 0, &face);

	int bboxWidth = face->bbox.xMax - face->bbox.xMin;
	int bboxHeight = face->bbox.yMax - face->bbox.yMin;
	error = FT_Set_Char_Size(face, 0, scale, FONT_RESOLUTION, FONT_RESOLUTION);
    if(error) perror("Error occured baking text");

	Rasteron_Image* canvasImage = createImgBlank(FONT_CANVAS_HEIGHT, FONT_CANVAS_WIDTH, textObj->bkColor);
	int pen_x = FONT_PEN_OFFSET; int pen_y = FONT_PEN_OFFSET;

    for(unsigned t = 0; t < strlen(textObj->text); t++){
		char glyphRef = textObj->text[t]; // for viewing current character being processed
        error = FT_Load_Char(face, (FT_ULong)glyphRef, FT_LOAD_RENDER);
		// error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL); // render glyph to fix issues
        if(error) perror("error loading glyph!");

		drawGlyph(
			canvasImage,
			&face->glyph->bitmap,
			textObj->fgColor,
			pen_x + face->glyph->bitmap_left,
			pen_y - face->glyph->bitmap_top // making text centered
		);
		pen_x += face->glyph->advance.x >> 6; // increment pen to next position
    }

	Rasteron_TextSizeProperties sizeProps;
	getImageTextParams(canvasImage, &sizeProps, textObj->fgColor, isInvert);

	// Copy from large canvas to real image

	Rasteron_Image* fontImage = (!isInvert)
		? createImgBlank(sizeProps.yMax - sizeProps.yMin, sizeProps.xMax - sizeProps.xMin, textObj->bkColor) // regular
		: createImgBlank(sizeProps.xMax - sizeProps.xMin, sizeProps.yMax - sizeProps.yMin, textObj->bkColor); // inverted
	cropTextImage(canvasImage, fontImage, sizeProps);
	
	deleteImg(canvasImage);
    FT_Done_Face(face);
	return fontImage; // return fontImage;
}

void initFreeType(FT_Library* library){
    int error = FT_Init_FreeType(library);
    if(error) perror("Error occured initializing freetype library!");
}

Rasteron_Image* bakeImgTextRegScaled(FT_Library* library, const Rasteron_FormatText* textObj, unsigned scale){
	return bakeImgTextCustom(library, textObj, scale, FONT_INVERT_FALSE);
}

Rasteron_Image* bakeImgTextReg(FT_Library* library, const Rasteron_FormatText* textObj){
	return bakeImgTextRegScaled(library, textObj, FONT_SIZE_MED);
}

Rasteron_Image* bakeImgTextInvertScaled(FT_Library* library, const Rasteron_FormatText* textObj, unsigned scale){
	return bakeImgTextCustom(library, textObj, scale, FONT_INVERT_TRUE);
}

Rasteron_Image* bakeImgTextInvert(FT_Library* library, const Rasteron_FormatText* textObj){
	return bakeImgTextInvertScaled(library, textObj, FONT_SIZE_MED);
}

void cleanupFreeType(FT_Library* library){
    FT_Done_FreeType(*library);
}