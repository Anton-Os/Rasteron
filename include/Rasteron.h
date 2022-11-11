#ifndef RASTERON_H

#ifdef __cplusplus
extern "C"{
#endif

// Core
#include "coord_type.h"
#include "geo_type.h"

#include "Image.h"
#include "Cellwise.h"
#include "Noise.h"

// Plugins
#ifdef RASTERON_ENABLE_ANIM
#include "Animation.h"
#endif

#ifdef RASTERON_ENABLE_FONT
#include "Font.h"
#endif

#ifdef __cplusplus
}
#endif

#define RASTERON_H
#endif