#include "seed.h"

void addSeed(Rasteron_SeedTable* table, unsigned color){ 
	addWeightedSeed(table, color, 0.0); 
}

void addWeightedSeed(Rasteron_SeedTable* table, unsigned color, double weight){
	table->seeds[table->seedCount].color = color;
	table->seeds[table->seedCount].weight = weight;
	table->seedCount++;
}