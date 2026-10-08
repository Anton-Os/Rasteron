#include "Rasteron.h"

#include "../_Test.h"

void _onKeyEvent(char key){}
void _onPressEvent(double x, double y){}
void _onTickEvent(unsigned secs){}

Rasteron_Image* testImgOp(ImageSize size, coordCallback callback){
#if USE_CUDA_LIBS
    return solidImgOp(size, 0xFF00FF00); // green image for success
#else
    return solidImgOp(size, 0xFFFF0000); // red image for failure
#endif
}

int main(int argc, char** argv) {
    // Initialize CUDA device
    cudaSetDevice(0);

    // Running 

    _outputImg = testImgOp((ImageSize){512, 512}, NULL);

    _run(argc, argv, NULL); // system specific initialization and continuous loop

    // Deallocation

    // RASTERON_QUEUE_DEALLOC(_mainQueue);
    RASTERON_DEALLOC(_outputImg);

    return 0;
} 