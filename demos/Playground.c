#define RASTERON_ENABLE_FONT
#define RASTERON_ENABLE_ANIM

#include "Experimental.h"

#define CANVAS_PRESET_MIN -1
#define CANVAS_PRESET_MAX 2

static int mode = -1;
static double xArg = 0.0;
static double yArg = 0.0;

static char keysave = 'd';

#include "Util_Demo.h"

void setup(char input){
	if(isalnum(input)) {
		if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
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
		case 'a': _outputImg = oragamiImgOp(mode, xArg, yArg); break;
		case 'b': _outputImg = nestboxesImgOp(xArg, yArg); break;
		case 'c': _outputImg = lensesImgOp(mode); break;
		case 'd': _outputImg = hypnosisImgOp(mode, 0xFFFFFF00, 0xFF00FFFF); break;
		case 'e': _outputImg = typographyImgOp(0xFFEEEEEE, 0xFF000000); break;
		case 'f': _outputImg = fisheyeImgOp((mode + 2) * 5); break;
		case 'g': _outputImg = mozaicImgOp(10.0 * (xArg + 1.0), 10.0 * (yArg + 1.0)); break;
		case 'h': _outputImg = interferenceImgOp(32, mode + 2); break;
		case 'i': _outputImg = perturbImgOp(xArg + 0.5F, yArg + 0.5); break;
		case 'j': _outputImg = ballingImgOp((double)(mode + 2)); break;
		case 'k': _outputImg = stratifyImgOp(mode + 4); break;
		case 'l': _outputImg = barkodeImgOp(mode + 4, RAND_COLOR(), RAND_COLOR()); break;
		case 'm': _outputImg = combImgOp(mode + 6, mode + 5); break;
		case 'n': _outputImg = truschetImgOp(mode + 3, mode + 3); break;
		case 'o': _outputImg = euclidTileImgOp(mode, 10, (xArg == 0.0)? 0.01 : 0.01 + xArg, (yArg == 0.0)? 0.01 : 0.01 + yArg); break;
		case 'p': _outputImg = nuTileImgOp(mode + 2, 10, (xArg == 0.0)? 0.01 : 0.01 + xArg, (yArg == 0.0)? 0.01 : 0.01 + yArg); break;
		case 'q': _outputImg = graterImgOp(RAND_COLOR(), RAND_COLOR()); break;
		case 'r': _outputImg = fuzzlikeImgOp((mode + 2) * 4); break;
		case 's': _outputImg = oozelikeImgOp(mode); break;
		case 't': _outputImg = recurrantImgOp(mode + 3); break;
		case 'u': _outputImg = intersectImgOp(1.0, 0.0); break;
		case 'v': _outputImg = swirlyImgOp((mode + 2) * 5.0); break;
		case 'w': _outputImg = displacerImgOp(mode + 2, 0xFFFF0088, 0xFF00FF88); break;
		case 'x': _outputImg = bilineImgOp(0xFF000000 + (rand() % 0xFF), 0x88 * (mode + 2)); break;
		case 'y': _outputImg = arcaneImgOp(100.0, (mode + 2) * 10); break;
		case 'z': _outputImg = ultImgOp(0, 128, 1.0 + xArg, 1.0 - xArg, 1.0 + yArg, 1.0 - yArg); break;
		default: _outputImg = expImgOp1(); break;
	}
}

void _onKeyEvent(char key){
	static char k = 0;

	// Parsing Step
	if(key == 0x0D && k != 0){
		printf("\nEntering your command\n");
		setup(k); 
	}
	else {
		putchar(key);
		k = key;
	}
}
void _onPressEvent(double x, double y){ }
void _onTickEvent(unsigned secs){}

int main(int argc, char** argv) {
	srand(time(NULL));
	if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    _outputImg = fisheyeImgOp((mode + 2) * 5);

	puts("Please refer to following commands to select images for canvas:");
	puts("\nAlphabetical characters A to H output images dedicated to various Rasteron API functionalities");
	puts("\nPress numbered keys 0-9 to tweak function parameters and modify the image outputs");

	_run(argc, argv, NULL); // system specific initialization and continuous loop

	if(_outputImg != NULL)
		writeFileImageRaw("Output", IMG_Png, _outputImg->height, _outputImg->width, _outputImg->data);

    RASTERON_DEALLOC(_outputImg); // cleanup
    return 0;
}