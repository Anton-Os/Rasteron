#ifndef RASTERON_H

#ifdef __cplusplus
extern "C"{
#endif

// Core
#include "pointtype.h"
#include "geotype.h"

#include "Image.h"
#include "Cellwise.h"
#include "Noise.h"

// Plugins
#ifdef RASTERON_ENABLE_PLUGIN
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