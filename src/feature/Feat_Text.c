#include "Rasteron.h"

#include "Feat_Text.h"

extern int _invertFont = FONT_INVERT;
extern FT_Library _freetypeLib = NULL;

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

static void cropTextImgToSize(Rasteron_Image* refImage, Rasteron_Image* sizedTextImg, TextSize sizeParams) {
	unsigned writeOffset = 0;

	for (unsigned r = 0; r < refImage->height; r++)
		for (unsigned c = 0; c < refImage->width; c++)
			if (r >= sizeParams.yMin && r < sizeParams.yMax && c >= sizeParams.xMin && c < sizeParams.xMax && writeOffset < (sizedTextImg->width * sizedTextImg->height)) {
				*(sizedTextImg->data + writeOffset) = *(refImage->data + (r * refImage->width) + c);
				writeOffset++;
			}
}

// Library Functions

void initFreeType(){
    int error = FT_Init_FreeType(&_freetypeLib);
    if(error) perror("Error occured initializing freetype library!");
}

void cleanupFreeType(){ FT_Done_FreeType(_freetypeLib); }

Rasteron_Image* textImgOp(const Rasteron_Text* textObj, unsigned size){
	if(_freetypeLib == NULL){ initFreeType(); }

	FT_Face face;
    int error = FT_New_Face(_freetypeLib, textObj->fontFile, 0, &face);
	if(error) perror("Error occured loading face");
	error = FT_Set_Char_Size(face, 0, size, FONT_RESOLUTION, FONT_RESOLUTION);
    if(error) perror("Error occured setting character size");

	unsigned tcanvasHeight = FONT_CANVAS_HEIGHT;
	unsigned tcanvasWidth = FONT_CANVAS_HEIGHT;
	for(unsigned t = 0; t < strlen(textObj->text); t++){
		FT_UInt charIndex = FT_Get_Char_Index(face, (FT_ULong)textObj->text[t]);
		error = FT_Load_Glyph(face, charIndex, FT_LOAD_RENDER);
        if(error) perror("Error loading glyph %c at %d\n", textObj->text[t], t);

		/* printf("Glyph left: %d, Glyph top: %d, Glyph rows: %d, Glyph width: %d \n", 
			face->glyph->bitmap_left, face->glyph->bitmap_top, 
			face->glyph->bitmap.rows, face->glyph->bitmap.width
		); */ // For testing
		// TODO: Calculate height and width
	}

	Rasteron_Image* textImage = (Rasteron_Image*)solidImgOp((ImageSize){ tcanvasHeight, tcanvasWidth }, textObj->bkColor);
	int pen_x = FONT_PEN_OFFSET; int pen_y = FONT_PEN_OFFSET;

    for(unsigned t = 0; t < strlen(textObj->text); t++){
		FT_UInt charIndex = FT_Get_Char_Index(face, (FT_ULong)textObj->text[t]);
		FT_Load_Glyph(face, charIndex, FT_LOAD_RENDER);

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

	TextSize sizeParams = getImageTextParams(textImage, textObj->fgColor, _invertFont);

	Rasteron_Image* sizedTextImg = (!_invertFont)
		? solidImgOp((ImageSize){ sizeParams.yMax - sizeParams.yMin, sizeParams.xMax - sizeParams.xMin }, textObj->bkColor) // regular
		: solidImgOp((ImageSize){ sizeParams.xMax - sizeParams.xMin, sizeParams.yMax - sizeParams.yMin }, textObj->bkColor); // inverted

	cropTextImgToSize(textImage, sizedTextImg, sizeParams);

	Rasteron_Image* antialiasImg = antialiasImgOp(sizedTextImg);
	
	dealloc_image(textImage); 
	dealloc_image(antialiasImg); // dealloc_image(sizedTextImg);
    FT_Done_Face(face);
	return sizedTextImg; // return antialiasImg;
}

Rasteron_Image* messageImgOp(const Rasteron_Message* messageObj, unsigned size){
	unsigned totalHeight = 0;
	unsigned maxWidth = 0;
	
	Rasteron_Image** textImages = (Rasteron_Image**)malloc(messageObj->messageCount * sizeof(Rasteron_Image*));
	for(unsigned t = 0; t < messageObj->messageCount; t++){
		Rasteron_Text textObj = { messageObj->fontFile, messageObj->messages[t], messageObj->bkColor, messageObj->fgColor };
		*(textImages + t) = textImgOp(&textObj, size);
		totalHeight += (*(textImages + t))->height;
		if((*(textImages + t))->width > maxWidth) maxWidth = (*(textImages + t))->width;
	}

	Rasteron_Image* messageImg = solidImgOp((ImageSize){ totalHeight, maxWidth }, messageObj->bkColor);
	Rasteron_Image* stagingImgOp = NULL;
	for(unsigned t = 0; t < messageObj->messageCount; t++){ // TODO: Test this logic
		if(stagingImgOp != NULL) dealloc_image(stagingImgOp);
		double yOffset = (((double)t / (double)messageObj->messageCount) * 2.0) - 1.0;
		stagingImgOp = insertImgOp(*(textImages + t), messageImg, 0.0, 1.0 - yOffset);
		dealloc_image(messageImg);
		messageImg = copyImgOp(stagingImgOp);
	}


	for(unsigned t = 0; t < messageObj->messageCount; t++) dealloc_image(*(textImages + t));
	free(textImages);

	dealloc_image(stagingImgOp);
	return messageImg;
}