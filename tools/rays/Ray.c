#include "RayTool.h"

#define RAY_PI 3.14159265358979323846

// --------------------------------   Ray Points    -------------------------------- //

unsigned rayPoints(float* points, unsigned maxRays){
    const Ray_State* s = &_ray;
    unsigned count = (unsigned)((s->arrange == ARRANGE_Custom)? s->customCount : s->count);
    if(count > maxRays) count = maxRays;

    double rotation = s->angle * RAY_PI / 180.0;
    double z = s->radius * sin(s->tilt * RAY_PI / 180.0);
    unsigned side = (unsigned)ceil(sqrt((double)count));

    for(unsigned r = 0; r < count; r++){
        double x = 0.0, y = 0.0, rz = z;
        switch(s->arrange){
        case ARRANGE_Circle: {
            double a = rotation + (s->arc * RAY_PI / 180.0) * r / ((s->arc >= 360.0)? count : ((count > 1)? count - 1 : 1));
            x = s->radius * cos(a); y = s->radius * sin(a);
        } break;
        case ARRANGE_Spiral: {
            double a = rotation + r * 2.39996322972865332;
            double d = s->radius * sqrt((r + 1.0) / count);
            x = d * cos(a); y = d * sin(a);
            rz = z * (1.0 - (double)r / count);
        } break;
        case ARRANGE_Line: {
            double t = (count > 1)? ((double)r / (count - 1)) * 2.0 - 1.0 : 0.0;
            x = t * s->radius * cos(rotation); y = t * s->radius * sin(rotation);
        } break;
        case ARRANGE_Grid: {
            double gx = (side > 1)? ((double)(r % side) / (side - 1)) * 2.0 - 1.0 : 0.0;
            double gy = (side > 1)? ((double)(r / side) / (side - 1)) * 2.0 - 1.0 : 0.0;
            x = s->radius * (gx * cos(rotation) - gy * sin(rotation));
            y = s->radius * (gx * sin(rotation) + gy * cos(rotation));
        } break;
        case ARRANGE_Random:
            x = (((double)rand() / RAND_MAX) * 2.0 - 1.0) * s->radius;
            y = (((double)rand() / RAND_MAX) * 2.0 - 1.0) * s->radius;
            rz = (((double)rand() / RAND_MAX) * 2.0 - 1.0) * z;
            break;
        case ARRANGE_Test: { // RayTest point data: { x, -y, +-r, -x, y, r, x, y, -r, -x, -y, 0 }
            static const double signs[4][3] = { { 1, -1, 1 }, { -1, 1, 1 }, { 1, 1, -1 }, { -1, -1, 0 } };
            double rx = s->radius * cos(rotation), ry = s->radius * sin(rotation);
            x = rx * signs[r % 4][0]; y = ry * signs[r % 4][1]; rz = z * signs[r % 4][2];
        } break;
        case ARRANGE_Custom:
            x = s->custom[r][0]; y = s->custom[r][1]; rz = s->custom[r][2];
            break;
        }
        points[r * 3 + 0] = (float)x;
        points[r * 3 + 1] = (float)y;
        points[r * 3 + 2] = (float)rz;
    }
    return count;
}

// --------------------------------   Mapping    -------------------------------- //

double rayMap(int map, double v){
    switch(map){
        case MAP_Abs: return fabs(v);
        case MAP_Linear: return v;
        case MAP_Sin: return sin(v * RAY_PI);
        case MAP_Cos: return cos(v * RAY_PI);
        case MAP_Tan: return tan(v * RAY_PI * 0.45);
        case MAP_Square: return v * v;
        case MAP_Sqrt: return sqrt(fabs(v));
        case MAP_Exp: return exp(v) - 1.0;
        case MAP_Log: return log(1.0 + fabs(v) * 4.0) * 0.5;
        case MAP_Saw: return v * 4.0 - floor(v * 4.0);
        case MAP_Inverse: return 0.1 / (fabs(v) + 0.1);
        case MAP_Step: return floor(v * 6.0) / 6.0;
        default: return v;
    }
}

// --------------------------------   Ray Image    -------------------------------- //
//
//      Generalizes raycastImgOp and rayblendImgOp from RayTest: each pixel direction is normalized
//      against the eye distance, mapped per axis, offset by every ray, and the ray colors are combined

Rasteron_Image* raymapImgOp(ImageSize size, const float* points, unsigned pointCount, coordCallback3 rayFunc, mixCallback rayMixFunc, mixCallback mixFunc){
    const Ray_State* s = &_ray;
    Rasteron_Image* rayImg = RASTERON_ALLOC("raymap", size.height, size.width);
    double aspect = (double)size.width / (double)size.height;

    for(unsigned p = 0; p < size.width * size.height; p++){
        double x = ((double)(p % size.width) / size.width - 0.5) * aspect;
        double y = (double)(p / size.width) / size.height - 0.5;
        double length = sqrt(x * x + y * y + s->dist * s->dist);

        double mx = rayMap(s->map[0], (x / length) * s->scale[0]) + s->offset[0];
        double my = rayMap(s->map[1], (y / length) * s->scale[1]) + s->offset[1];
        double mz = rayMap(s->map[2], (s->dist / length) * s->scale[2]) + s->offset[2];

        unsigned color;
        if(pointCount == 0 || points == NULL) color = rayFunc(mx, my, mz);
        else {
            color = 0;
            for(unsigned c = 0; c < pointCount; c++){
                unsigned rayColor = rayFunc(mx + points[c * 3 + 0], my + points[c * 3 + 1], mz + points[c * 3 + 2]);
                if(mixFunc != NULL) rayColor = mixFunc(rayColor, rayColor); // rayblendImgOp behavior
                color = (c == 0)? rayColor : rayMixFunc(rayColor, color);
            }
        }
        *(rayImg->data + p) = color | 0xFF000000;
    }
    return rayImg;
}
