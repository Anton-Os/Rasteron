#include "Rasteron.h"

Rasteron_Image* canvasImg = NULL;

void keyEvent(char key){}
void mouseEvent(double x, double y){}
void (*timerEvent)(unsigned secs) = NULL;

#include "Util_Demo.h"

int main(int argc, char** argv) {
    canvasImg = solidImgOp((ImageSize){ 1024, 1024 }, 0xFFFFFF00); // global canvas for drawing

	inputLoop(NULL);

    return 0;
}