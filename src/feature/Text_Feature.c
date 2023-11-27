#include "Rasteron.h"

#include "Text_Feature.h"

static FT_Library _freetypeLib = NULL;

// Internal Functions

static void drawGlyphToImg(Rasteron_Image* image, FT_Bitmap* ftBmap, uint32_t color, int x, int y){
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

static TextSize getImageTextParams(Rasteron_Image* refImage, uint32_t color, int isInverted) {
	TextSize sizeParams = {
		(!isInverted)? refImage->width : refImage->height, 0,
		(!isInverted)? refImage->width : refImage->width, 0
	};

	for(unsigned r = 0; r < refImage->height; r++)
		for (unsigned c = 0; c < refImage->width; c++)
			if (*(refImage->data + (r * refImage->width) + c) == color) {
				if (c < sizeParams.xMin) sizeParams.xMin = c;
				else if (c > sizeParams.xMax) sizeParams.xMax = c;

				if (r < sizeParams.yMin) sizeParams.yMin = r;
				else if (r > sizeParams.yMax) sizeParams.yMax = r;
			}

	return sizeParams;
}

static void cropTextImgToSize(Rasteron_Image* refImage, Rasteron_Image* fontImage, TextSize sizeParams) {
	unsigned writeOffset = 0;

	for (unsigned r = 0; r < refImage->height; r++)
		for (unsigned c = 0; c < refImage->width; c++)
			if (r >= sizeParams.yMin && r < sizeParams.yMax && c >= sizeParams.xMin && c < sizeParams.xMax && writeOffset < (fontImage->width * fontImage->height)) {
				*(fontImage->data + writeOffset) = *(refImage->data + (r * refImage->width) + c);
				writeOffset++;
			}
}

static Rasteron_Image* bakeText_custom(const Rasteron_Text* textObj, unsigned scale, int isInverted){
    if(_freetypeLib == NULL){ initFreeType(); }

	FT_Face face;
    int error = FT_New_Face(_freetypeLib, textObj->fileName, 0, &face);
	if(error) perror("Error occured loading face");
	error = FT_Set_Char_Size(face, 0, scale, FONT_RESOLUTION, FONT_RESOLUTION);
    if(error) perror("Error occured setting character size");

	Rasteron_Image* textImage = (Rasteron_Image*)solidImgOp((ImageSize){ FONT_CANVAS_HEIGHT, FONT_CANVAS_WIDTH }, textObj->bkColor);
	int pen_x = FONT_PEN_OFFSET; int pen_y = FONT_PEN_OFFSET;

    for(unsigned t = 0; t < strlen(textObj->text); t++){
		char glyphRef = textObj->text[t]; // for viewing current character being processed
		FT_UInt charIndex = FT_Get_Char_Index(face, (FT_ULong)glyphRef);
		error = FT_Load_Glyph(face, charIndex, FT_LOAD_RENDER);
        if(error) perror("Error loading glyph %c at %d\n", glyphRef, t);

		drawGlyphToImg(
			textImage,
			&face->glyph->bitmap,
			textObj->fgColor,
			pen_x + face->glyph->bitmap_left,
			pen_y - face->glyph->bitmap_top // making text centered
		);
		pen_x += face->glyph->advance.x >> 6; // increment pen to next position
    }

	// Crop from large canvas onto final image

	TextSize sizeParams = getImageTextParams(textImage, textObj->fgColor, isInverted);

	Rasteron_Image* fontImage = (!isInverted)
		? solidImgOp((ImageSize){ sizeParams.yMax - sizeParams.yMin, sizeParams.xMax - sizeParams.xMin }, textObj->bkColor) // regular
		: solidImgOp((ImageSize){ sizeParams.xMax - sizeParams.xMin, sizeParams.yMax - sizeParams.yMin }, textObj->bkColor); // inverted

	cropTextImgToSize(textImage, fontImage, sizeParams);
	
	dealloc_image(textImage);
    FT_Done_Face(face);
	return fontImage; // return fontImage;
}

// Library Functions

void initFreeType(){
    int error = FT_Init_FreeType(&_freetypeLib);
    if(error) perror("Error occured initializing freetype library!");
	else puts("Successfully initialized freetype library");
}

void cleanupFreeType(){ FT_Done_FreeType(_freetypeLib); }

/* Rasteron_Image* bakeText_sized(const Rasteron_Text* textObj, unsigned scale){
	return bakeText_custom(textObj, scale, FONT_REGULAR);
} */

Rasteron_Image* bakeText(const Rasteron_Text* textObj, unsigned size){
	return bakeText_custom(textObj, size, FONT_REGULAR);
}

/* Rasteron_Image* bakeTextI_sized(const Rasteron_Text* textObj, unsigned scale){
	return bakeText_custom(textObj, scale, FONT_INVERTED);
} */

Rasteron_Image* bakeTextI(const Rasteron_Text* textObj, unsigned size){
	return bakeText_custom(textObj, size, FONT_INVERTED);
}