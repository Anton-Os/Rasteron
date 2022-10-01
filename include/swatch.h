
#include "support_def.h"

#include "seed.h"

/* enum SPECTRUM_Level {
	SPECTRUM_Dark, // 0% to 20% // BLACK
	SPECTRUM_Low, // 20% to 40%
	SPECTRUM_Medium, // 40% to 60%
	SPECTRUM_High, // 60% to 80%
	SPECTRUM_Bright // 80% to 100% // WHITE
}; */

#define SWATCH_COUNT 8

enum SWATCH_Colors {
    SWATCH_Yellow_Add = 0,
    SWATCH_Cyan_Add = 1,
    SWATCH_Magenta_Add = 2,
    SWATCH_Light = 3,
    SWATCH_Dark = 4,
    SWATCH_Yellow_Sub = 5,
    SWATCH_Cyan_Sub = 6,
    SWATCH_Magenta_Sub = 7,
};

typedef struct {
    unsigned base; // base color to generate swatch
    unsigned colors[SWATCH_COUNT]; // generated swatch colors
    uint8_t deviation;
} Rasteron_Swatch;

Rasteron_Swatch createSwatch(unsigned color, uint8_t deviation);
Rasteron_SeedTable createSeedTable(const Rasteron_Swatch* swatch);
