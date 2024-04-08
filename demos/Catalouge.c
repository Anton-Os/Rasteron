#include "Util_OS.h"

#include "Experimental.h"

Rasteron_Image* canvasImg; // global canvas for drawing

#define CANVAS_PRESET_MIN -1
#define CANVAS_PRESET_MAX 2

static int mode = -1;
static double xArg = 0.0;
static double yArg = 0.0;

static char keysave = 'd';

void setupCanvas(char input){
	if(isalnum(input)) {
		if(canvasImg != NULL) RASTERON_DEALLOC(canvasImg);
		if(isalpha(input)) keysave = (char)tolower(input);
	}

	switch(input){
		case '0': xArg = 0.0; yArg = 0.0; mode = 0; break;
		case '1': mode++; break; case '3': mode--; break;
		case '8': yArg += 0.05F; break; case '2': yArg -= 0.05F; break;
		case '6': xArg += 0.05F; break; case '4': xArg -= 0.05F; break;
		case '7': double temp = xArg; xArg = yArg; yArg = temp; break; // flip arguments 1 and 2
		case '9': xArg = ((double)rand() / (RAND_MAX / 2.0)) - 1.0;
				  yArg = ((double)rand() / (RAND_MAX / 2.0)) - 1.0;
	}
	if(mode > CANVAS_PRESET_MAX) mode = CANVAS_PRESET_MIN; // bounds mode to upper limit
	else if(mode < CANVAS_PRESET_MIN) mode = CANVAS_PRESET_MAX;

	switch(keysave){
		case 'a': canvasImg = oragamiImgOp(mode, xArg, yArg); break;
		case 'b': canvasImg = nestboxesImgOp(xArg, yArg); break;
		case 'c': canvasImg = lensesImgOp(mode); break;
		case 'd': canvasImg = hypnosisImgOp(mode, 0xFFFFFF00, 0xFF00FFFF); break;
		case 'e': canvasImg = typographyImgOp(0xFFEEEEEE, 0xFF000000); break;
		case 'f': canvasImg = grassNoiseImgOp(mode, 1024, 1024); break;
		case 'g': canvasImg = gameOfLifeImgOp(mode + 2); break;
		case 'h': canvasImg = patchingImgOp((mode + 2) * 5); break;
		case 'i': canvasImg = mosaicImgOp(10.0 * (xArg + 1.0), 10.0 * (yArg + 1.0)); break;
		case 'j': canvasImg = interferenceImgOp(32, mode + 2); break;
		case 'k': canvasImg = perturbImgOp(xArg + 0.5F, yArg + 0.5); break;
		case 'l': canvasImg = ballingImgOp((double)(mode + 2)); break;
		case 'm': canvasImg = stratifyImgOp(mode + 4); break;
		case 'n': canvasImg = chemicalsImgOp(RAND_COLOR(), RAND_COLOR()); break;
		case 'o': canvasImg = expImgOp1(); break;
		case 'p': canvasImg = expImgOp2(); break;
		case 'q': canvasImg = expImgOp3(); break;
		case 'r': canvasImg = expImgOp4(); break;
		case 's': canvasImg = expImgOp5(); break;
		case 't': canvasImg = expImgOp6(); break;
		case 'u': canvasImg = expImgOp7(); break;
		case 'v': canvasImg = expImgOp8(); break;
		case 'w': canvasImg = expImgOp9(); break;
		case 'x': canvasImg = expImgOp10(); break;
		case 'y': canvasImg = expImgOp11(); break;
		case 'z': canvasImg = expImgOp12(); break;
		default: canvasImg = solidImgOp((ImageSize){ 1024, 1024 }, RAND_COLOR()); break;
	}
}

Rasteron_Image* canvasImg;
void keyEvent(char key){ if(key != 0) setupCanvas(key); }
void mouseEvent(double x, double y){ }
void timerEvent(unsigned secs){}

#include "Util_Demo.h"

int main(int argc, char** argv) {
	srand(time(NULL));
	if(canvasImg != NULL) RASTERON_DEALLOC(canvasImg);
    canvasImg = grassNoiseImgOp(mode, 1024, 1024);

	puts("Please refer to following commands to select images for canvas:");
	puts("\nAlphabetical characters A to H output images dedicated to various Rasteron API functionalities");
	puts("\nPress numbered keys 0-9 to tweak function parameters and modify the image outputs");

	inputLoop(NULL);

	if(canvasImg != NULL)
		writeFileImageRaw("Output", IMG_Png, canvasImg->height, canvasImg->width, canvasImg->data);

    RASTERON_DEALLOC(canvasImg); // cleanup
    return 0;
}