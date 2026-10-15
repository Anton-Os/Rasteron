#include "../Test.h"

#include <cuda_runtime.h> // see if this include works

void _onKeyEvent(char key){}
void _onPressEvent(double x, double y){}
void _onTickEvent(unsigned secs){}

// CUDA

Rasteron_Image* cudaImgOp(ImageSize size, coordCallback callback){
#if USE_CUDA_LIBS
    return solidImgOp(size, 0xFF00FF00); // green image for success
#else
    return solidImgOp(size, 0xFFFF0000); // red image for failure
#endif
}

// Main

int main(int argc, char** argv) {
    auto result = cudaInitDevice(0, 0, 0);
    
    // Running 

    _outputImg = cudaImgOp({ RASTERON_HEIGHT, RASTERON_WIDTH }, NULL);
    _run(argc, argv, NULL); // system specific initialization and continuous loop

    // Deallocation

    // RASTERON_QUEUE_DEALLOC(_mainQueue);
    RASTERON_DEALLOC(_outputImg);

    return 0;
} 