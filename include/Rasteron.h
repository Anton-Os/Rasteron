#ifndef RASTERON_H

#ifdef __cplusplus
extern "C"{
#endif

// Core
#include "Image.h"
#include "Primitive.h"

// Plugins
#ifdef RASTERON_ENABLE_PLUGIN
#include "Animation.h"
#include "Cellwise.h"
#include "Heightmap.h"
#include "Noise.h"
#endif

#ifdef RASTERON_ENABLE_FONT
#include "Font.h"
#endif

#ifdef __cplusplus
}
#endif

#define RASTERON_H
#endif