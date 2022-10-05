#include "mixtype.h"

// Seed operations

void addSeed(Rasteron_SeedTable* table, unsigned color){ 
	addWeightedSeed(table, color, 0.0); 
}

void addWeightedSeed(Rasteron_SeedTable* table, unsigned color, double weight){
	table->seeds[table->seedCount].color = color;
	table->seeds[table->seedCount].weight = weight;
	table->seedCount++;
}

// Swatch operations

Rasteron_Swatch createSwatch(unsigned color, uint8_t deviation){
	Rasteron_Swatch swatch;
	
	swatch.base = color;
	swatch.deviation = deviation;

	uint8_t redBit = (color & RED_CHANNEL) >> 16;
	uint8_t greenBit = (color & GREEN_CHANNEL) >> 8;
	uint8_t blueBit = color & BLUE_CHANNEL;

	// Clamps Additive Values Below 0xFF
	uint8_t redAdd = (deviation < 0xFF - redBit)? deviation : 0xFF - redBit;
	uint8_t greenAdd = (deviation < 0xFF - greenBit)? deviation : 0xFF - greenBit;
	uint8_t blueAdd = (deviation < 0xFF - blueBit)? deviation : 0xFF - blueBit;

	// Clamps Substractive Values Above 0x00
	uint8_t redSub = (deviation < redBit)? deviation : redBit;
	uint8_t greenSub = (deviation < greenBit)? deviation : greenBit;
	uint8_t blueSub = (deviation < greenBit)? deviation : blueBit;

	// TODO: Add calculation here!
	swatch.colors[SWATCH_Yellow_Add] = color + (redAdd << 16) | (greenAdd << 8);
	swatch.colors[SWATCH_Cyan_Add] = color + (greenAdd << 8) | blueAdd;
	swatch.colors[SWATCH_Magenta_Add] = color + (redAdd << 16) | (blueBit + blueAdd);
	swatch.colors[SWATCH_Light] = color + (redAdd << 16) | (greenAdd << 8) | blueAdd;
	swatch.colors[SWATCH_Dark] = color - ((redSub << 16) | (greenSub << 8) | (blueSub));
	swatch.colors[SWATCH_Yellow_Sub] = color - ((redSub << 16) | (greenSub << 8));
	swatch.colors[SWATCH_Cyan_Sub] = color - ((greenSub << 8) | blueSub);
	swatch.colors[SWATCH_Magenta_Sub] = color - ((redSub << 16) | blueSub);

	return swatch;
}

Rasteron_SeedTable createSeedTable(const Rasteron_Swatch* swatch) {
	Rasteron_SeedTable seedTable = { 0 };
	addSeed(&seedTable, swatch->colors[SWATCH_Yellow_Add]);
	addSeed(&seedTable, swatch->colors[SWATCH_Cyan_Add]);
	addSeed(&seedTable, swatch->colors[SWATCH_Magenta_Add]);
	addSeed(&seedTable, swatch->colors[SWATCH_Light]);
	addSeed(&seedTable, swatch->colors[SWATCH_Dark]);
	addSeed(&seedTable, swatch->colors[SWATCH_Yellow_Sub]);
	addSeed(&seedTable, swatch->colors[SWATCH_Cyan_Sub]);
	addSeed(&seedTable, swatch->colors[SWATCH_Magenta_Sub]);

	return seedTable;
}