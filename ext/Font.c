#include "Rasteron.h"

#include "Font.h"

extern FT_Library _freetypeLib = NULL;

static unsigned short _topPad = 0;
static unsigned short _botPad = 0;
static unsigned short _leftPad = 0;
static unsigned short _rightPad = 0;

// Internal Functions

static void drawGlyphToImg(Rasteron_Image* image, FT_Bitmap* ftBmap, uint32_t color, int x, int y){
	uint32_t imageOff = x + (image->width * y); // starting pixel for drawing
	if(ftBmap->rows > 0 && ftBmap->width > 0)
		for (unsigned r = 0; r < ftBmap->rows; r++) {
			for (unsigned w = 0; w < ftBmap->width; w++) {
				uint32_t bmapColor = ftBmap->buffer[(r * ftBmap->width) + w];
				if(bmapColor > 0) *(image->data + imageOff) = color;
				imageOff++; // move to adjascent pixel
			}
			imageOff += image->width - ftBmap->width; // move to start of next row
		}
	else return; // skips space
}

static TextSize getImageTextParams(Rasteron_Image* refImage, uint32_t color) {
	TextSize sizeParams = { refImage->width, 0, refImage->width, 0 };

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

static void cropTextImgToSize(Rasteron_Image* refImage, Rasteron_Image* textImg, TextSize sizeParams) {
	unsigned writeOffset = 0;

	for (unsigned r = 0; r < refImage->height; r++)
		for (unsigned c = 0; c < refImage->width; c++)
			if (r >= sizeParams.yMin && r < sizeParams.yMax && c >= sizeParams.xMin && c < sizeParams.xMax && writeOffset < (textImg->width * textImg->height)) {
				*(textImg->data + writeOffset) = *(refImage->data + (r * refImage->width) + c);
				writeOffset++;
			}
}

// Library Functions

void initFreeType(){
    int error = FT_Init_FreeType(&_freetypeLib);
    if(error) perror("Error occured initializing freetype library!");
}

void cleanupFreeType(){ FT_Done_FreeType(_freetypeLib); }

static Rasteron_Image* _textImgOp(const Rasteron_Text* textObj, unsigned size){
	if(_freetypeLib == NULL){ initFreeType(); }
#ifdef _WIN32
	replaceFwdSlash(textObj->fontFile);
    if(_access(textObj->fontFile, 0)){
#elif defined(__linux__)
    if(access(textObj->fontFile, F_OK) != 0){
#endif
        return errorImgOp("Invalid font file");
    }

	FT_Face face;
    int error = FT_New_Face(_freetypeLib, textObj->fontFile, 0, &face);
	if(error) perror("Error occured loading face");
	error = FT_Set_Char_Size(face, 0, size, FONT_RESOLUTION, FONT_RESOLUTION);
    if(error) perror("Error occured setting character size");

	unsigned totalWidth = FONT_PEN_OFFSET; unsigned totalHeight = FONT_PEN_OFFSET;
	for(unsigned t = 0; t < strlen(textObj->text); t++){
		FT_UInt charIndex = FT_Get_Char_Index(face, (FT_ULong)textObj->text[t]);
		FT_Load_Glyph(face, charIndex, FT_LOAD_RENDER);
		totalWidth += face->glyph->bitmap.width + face->glyph->bitmap_left + (face->glyph->advance.x >> 6);
		if(face->glyph->bitmap.rows + FONT_PEN_OFFSET > totalHeight) totalHeight = face->glyph->bitmap.rows + FONT_PEN_OFFSET;
    }

	Rasteron_Image* textCanvasImg = (!_invertImage)
		? (Rasteron_Image*)solidImgOp((ImageSize){ totalHeight, totalWidth }, textObj->bkColor)
		: (Rasteron_Image*)solidImgOp((ImageSize){ totalWidth, totalHeight }, textObj->bkColor);
	int pen_x = FONT_PEN_OFFSET; int pen_y = FONT_PEN_OFFSET;

	// TODO: Include error handling
    for(unsigned t = 0; t < strlen(textObj->text); t++){
		FT_UInt charIndex = FT_Get_Char_Index(face, (FT_ULong)textObj->text[t]);
		FT_Load_Glyph(face, charIndex, FT_LOAD_RENDER);

		drawGlyphToImg(
			textCanvasImg,
			&face->glyph->bitmap,
			textObj->fgColor,
			pen_x + face->glyph->bitmap_left,
			pen_y - face->glyph->bitmap_top // making text centered
		);
		pen_x += face->glyph->advance.x >> 6; // increment pen to next position
    }

	// Crop from large canvas onto final image

	TextSize sizeParams = getImageTextParams(textCanvasImg, textObj->fgColor);

	Rasteron_Image* textImg = solidImgOp(
		(ImageSize){ sizeParams.yMax - sizeParams.yMin, sizeParams.xMax - sizeParams.xMin }, 
		textObj->bkColor
	);
	
	cropTextImgToSize(textCanvasImg, textImg, sizeParams); // TODO: Test for appropriate size
	
	RASTERON_DEALLOC(textCanvasImg); 
    FT_Done_Face(face);

	if(_topPad == 0 && _leftPad == 0 && _botPad == 0 && _rightPad == 0) return textImg; // padding not required
	else { // padding required
		Rasteron_Image* backgroundImg = solidImgOp(
			(ImageSize){ textImg->height + _botPad + _topPad, textImg->width + _rightPad + _leftPad },
			textObj->bkColor
		);

		double widthExtent = (_rightPad > _leftPad)? _rightPad : _leftPad;
		double heightExtent = (_topPad > _botPad)? _topPad : _botPad;
		Rasteron_Image* textPaddedImg = insertImgOp(textImg, backgroundImg, 
			(_rightPad != _leftPad)? (_leftPad - _rightPad) / widthExtent : 0.0, 
			(_topPad != _botPad)? (_topPad - _botPad) / heightExtent : 0.0
		);

		RASTERON_DEALLOC(textImg);

		return textPaddedImg;
	}
}

Rasteron_Image* textImgOp(const Rasteron_Text* textObj, unsigned size){
	_leftPad = 0;
	_rightPad = 0;
	_topPad = 0;
	_botPad = 0;

	return _textImgOp(textObj, size);
}

Rasteron_Image* textPadImgOp(const Rasteron_Text* textObj, unsigned size, unsigned short padding[4]){
	_leftPad = padding[0];
	_rightPad = padding[1];
	_topPad = padding[2];
	_botPad = padding[3];

	return _textImgOp(textObj, size);
}

Rasteron_Image* messageImgOp(const Rasteron_Message* messageObj, unsigned size){
	unsigned totalHeight = 0;
	unsigned maxWidth = 0;
	
	Rasteron_Image** textImages = (Rasteron_Image**)malloc(messageObj->messageCount * sizeof(Rasteron_Image*));
	for(unsigned t = 0; t < messageObj->messageCount; t++){
		Rasteron_Text textObj = { messageObj->fontFile, messageObj->messages[t], messageObj->bkColor, messageObj->fgColor };
		unsigned short padding[4] = { _leftPad, _rightPad, _topPad, _botPad };
		*(textImages + t) = textPadImgOp(&textObj, size, padding); // textImgOp(&textObj, size);
		totalHeight += (*(textImages + t))->height;
		if((*(textImages + t))->width > maxWidth) maxWidth = (*(textImages + t))->width;
	}

	Rasteron_Image* messageImg = solidImgOp((ImageSize){ totalHeight, maxWidth }, messageObj->bkColor);
	Rasteron_Image* stagingImgOp = NULL;
	for(unsigned t = 0; t < messageObj->messageCount; t++){ // TODO: Test this logic
		if(stagingImgOp != NULL) RASTERON_DEALLOC(stagingImgOp);
		double yOffset = (((double)t / ((double)messageObj->messageCount - 1.0)) * 2.0) - 1.0;
		stagingImgOp = insertImgOp(*(textImages + t), messageImg, messageObj->alignment, yOffset);
		RASTERON_DEALLOC(messageImg);
		messageImg = copyImgOp(stagingImgOp);
	}


	for(unsigned t = 0; t < messageObj->messageCount; t++) RASTERON_DEALLOC(*(textImages + t));
	free(textImages);

	RASTERON_DEALLOC(stagingImgOp);
	return messageImg;
}
