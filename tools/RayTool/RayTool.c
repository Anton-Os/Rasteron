static double xArg = 0.0;
static double yArg = 0.0;

#include "../_Tool.h"

// Overriden Functions

unsigned rayColor1 = 0xFF000000;
unsigned rayColor2 = 0xFFFFFFFF;

static double xArg = 0.0;
static double yArg = 0.0;

static unsigned vectorFunc(double x, double y, double z) {
    unsigned color = blend_colors(rayColor1, rayColor2, pow(x + y + z, x * y * z) * 0.25);
    return mult_rgb(color, color);
}

Rasteron_Image* raycastImgOp(float* points, unsigned pointCount, double dist){ 
    Rasteron_Image* raycastImg = RASTERON_ALLOC("raycast", 1024, 1024);

    rayColor1 = RAND_COLOR();
    rayColor2 = color_invert(rayColor1);

    for (unsigned p = 0; p < raycastImg->width * raycastImg->height; p++) {
        double x = (1.0 / (double)1024) * (p % 1024) - 0.5;
        double y = (1.0 / (double)1024) * (p / 1024) - 0.5;

        double length = sqrt(pow(x, 2.0) + pow(y, 2.0) + pow(dist, 2.0));
        
        if(pointCount > 0 && points != NULL)
            for (unsigned c = 0; c < pointCount; c++) {
                unsigned color = vectorFunc(
                  fabs(x / length) + *(points + (c * 3) + 0), 
                  fabs(y / length) + *(points + (c * 3) + 1),
                  fabs(dist / length) + *(points + (c * 3) + 2)
                );
                *(raycastImg->data + p) = (c == 0)? color : bit_colors_xor(color, *(raycastImg->data + p));
                // *(raycast->data + p) = (c == 0)? color : fuse_colors(color, *(raycast->data + p), (1.0 / pointCount) * (c + 1));
            }
        else *(raycastImg->data + p) = vectorFunc(fabs(x / length), fabs(y / length), fabs(dist / length));
    }

    return raycastImg;
}

void _onKeyEvent(char key){
    float r = (((float)rand() / (float)RAND_MAX) - 0.5) * 2 * 2;
    float pointData[12] = { xArg, -yArg, (rand() % 2 == 0) ? r : -r, -xArg, yArg, r, xArg, yArg, -r, -xArg, -yArg, 0.0F };
    // TODO: Parse key events
}
void _onPressEvent(double x, double y){ }
void _onTickEvent(unsigned secs){}

int main(int argc, char** argv) {
    srand(time(NULL));
    
    rayColor1 = RAND_COLOR();
    rayColor2 = RAND_COLOR();

    _outputImg = vectorImgOp((ImageSize){ 1024, 1024 }, 1.0, vectorFunc);

    _run(argc, argv, NULL); // system specific initialization and continuous loop

    RASTERON_DEALLOC(_outputImg); // cleanup
    return 0;
}
