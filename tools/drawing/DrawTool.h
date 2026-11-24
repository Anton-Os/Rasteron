#include "../Tool.h"

#define DRAW_TAU 6.28318530717958647692
#define DRAW_LAYERS_MAX 32 // layers in one drawing, apply flattens them to make room
#define DRAW_POINTS_MAX 64 // points in one layer
#define DRAW_CANVAS_COLOR 0xFF111111 // matches DrawTest

// --------------------------------   Drawing State    -------------------------------- //

enum DRAW_Kind { DRAW_Points = 0, DRAW_Path = 1, DRAW_Brush = 2 };
enum DRAW_Dot { DOT_Disc = 0, DOT_Ring = 1, DOT_Soft = 2, DOT_Square = 3, DOT_Diamond = 4, DOT_Cross = 5, DOT_Star = 6, DOT_Half = 7, DOT_Spray = 8 };
enum DRAW_Line { LINE_Straight = 0, LINE_Curve = 1, LINE_Closed = 2, LINE_Loop = 3 };
enum DRAW_Brush { BRUSH_Round = 0, BRUSH_Flat = 1, BRUSH_Power = 2, BRUSH_Bulge = 3, BRUSH_Bristle = 4, BRUSH_Chalk = 5, BRUSH_Spray = 6, BRUSH_Stamp = 7 };
enum DRAW_Gradient { GRADIENT_None = 0, GRADIENT_Along = 1, GRADIENT_Across = 2 };
enum DRAW_Blend { BLEND_Normal = 0, BLEND_Add = 1, BLEND_Multiply = 2, BLEND_Screen = 3, BLEND_Diff = 4, BLEND_Erase = 5 };
enum DRAW_Backdrop { BACKDROP_Solid = 0, BACKDROP_Checker = 1, BACKDROP_Gradient = 2 };

typedef struct {
    int kind; // points, path, or brush
    unsigned colors[2]; // main color and gradient end color
    int gradient; // how the second color is used
    double width; // stroke width and dot diameter as a fraction of the image height
    double opacity;
    double softness; // feathering of edges
    int blend; // how the layer combines with the layers below
    int dot; // dot shape of points layers
    int line; // how points are connected by paths and brushes
    double wave; // sideways wobble in stroke widths
    double frequency; // repetitions along the stroke for wave, bulge, and stamp
    double cut; // dash length of paths in stroke widths, 0 is solid
    int fill; // fills closed and loop paths
    int brush; // brush texture
} Draw_Style;

typedef struct {
    Draw_Style style;
    int visible;
    int pointCount;
    double points[DRAW_POINTS_MAX][2]; // x y fractions of the image, origin at the top left
} Draw_Layer;

typedef struct {
    unsigned canvas[2]; // backdrop colors
    int backdrop;
    int grid; // checker squares across
    double progress; // fraction of the drawing that is shown, animated by frames/encode
    Draw_Style pen; // settings for new layers, or the live settings of the selected layer
    int selected; // index of the layer being edited, -1 when the pen is free
    int layerCount;
    Draw_Layer layers[DRAW_LAYERS_MAX]; // bottom layer first
} Draw_State;

extern Draw_State _draw;

// --------------------------------   Image Operations (see Draw.c)    -------------------------------- //

const Draw_Style* drawLayerStyle(const Draw_State* state, int index); // style in effect, the selected layer uses the pen
Rasteron_Image* backdropImgOp(ImageSize size, const Draw_State* state, ref_image_t baseImg); // canvas below all layers
void drawLayer(Rasteron_Image* canvasImg, ref_image_t backdropImg, const Draw_Layer* layer, const Draw_Style* style, double shown, unsigned seed); // shown from 0 to 1
Rasteron_Image* drawImgOp(ImageSize size, const Draw_State* state, ref_image_t baseImg); // backdrop with every visible layer on top
