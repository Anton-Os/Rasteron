#include "Rasteron.h"

Rasteron_Image* seededImgOp(ref_image_t refImage, ColorSeed seed){
	assert(refImage != NULL);

	Rasteron_Image* seedImage = copyImgOp(refImage);
	
	if(seed.weight <= 0.0) return seedImage;
	else if(seed.weight >= 1.0){
		for (unsigned i = 0; i < seedImage->height * seedImage->width; i++)
			*(seedImage->data + i) = seed.color;
		return seedImage;
	} else {
		double chance = 0.0;
		for (unsigned p = 0; p < seedImage->width * seedImage->height; p++) {
			chance = (double)rand() / (double)RAND_MAX;
			if (chance <= seed.weight) *(seedImage->data + p) = seed.color; // if chance is less than seed.weightability change to target color
			else *(seedImage->data + p) = *(refImage->data + p); // otherwise copy ref contents
		}
	}

	return seedImage;
}

Rasteron_Image* seededImgOp_tabled(ref_image_t refImage, const ColorSeedTable* seedTable){
	assert(refImage != NULL);

	Rasteron_Image* seedImage = copyImgOp(refImage);
	const float defaultSeedWeight = 1.0f / (seedTable->seedCount + 1); // leaves less covered as seed count increases

	double chance = 0.0f;
	double seedRanges[MAX_SEEDS][2];
	double seedWeightTotal = 0.0f;

	// calculating weights
	for(unsigned s = 0; s < seedTable->seedCount; s++){
		double currentWeight = (seedTable->seeds[s].weight == DEFAULT_SEED_WEIGHT) ? defaultSeedWeight : seedTable->seeds[s].weight;
		seedRanges[s][0] = seedWeightTotal; // lower bound is the running weight total
		seedRanges[s][1] = seedWeightTotal + currentWeight; // upper bound is the added weight
		seedWeightTotal += currentWeight;
	}

	// setting proper colors
	for (unsigned p = 0; p < seedImage->width * seedImage->height; p++) {
		chance = (seedWeightTotal > 1.0)? ((double)rand() / (double)RAND_MAX) * seedWeightTotal : ((double)rand() / (double)RAND_MAX);
		unsigned color = *(refImage->data + p);
		for(unsigned s = 0; s < seedTable->seedCount; s++)
			if(chance > seedRanges[s][0] && chance < seedRanges[s][1]){ // checks if rand number is within bounds
				color = seedTable->seeds[s].color; // sets new color if range is satisfied
				break;
			}
		*(seedImage->data + p) = color;
	}

	return seedImage;
}