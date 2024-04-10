#include "Experimental.h"

unsigned startPaint(double x, double y){ return (sin(x * 10.0) > tan(y * 10.0))? 0xFFFFFF00 : 0xFFFF00FF; } 

#include "Util_Demo.h"

void keyEvent(char key){ printf("Key entered: %c", key); }
void mouseEvent(double x, double y){ printf("Cursor data is (%f, %f)", x, y); }
void timerEvent(unsigned secs){}

/* void inputCallback(){
	puts("Enter a command: ");
	scanf("%s", &inputStr);
	
	if(inputStr[0] == '0' && inputStr[1] == 'x') puts("Solid color detected");
	// TODO: Detect if file path was set to png, tiff, or bmp
	// TODO: Detect procedural operations
	else puts("Unknown operation!");
} */

int main(int argc, char** argv){
	if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    _outputImg = mapImgOp((ImageSize){1024, 1024}, startPaint); // global canvas for drawing

	inputLoop(NULL);

    RASTERON_DEALLOC(_outputImg); // cleanup
    return 0;
}