#include "Rasteron.h"

Rasteron_Image* seededImgOp(ref_image_t refImage, uint32_t color, double prob){
	assert(refImage != NULL);

	Rasteron_Image* seedImage = copyedImgOp(refImage);
	
	if(prob <= 0.0) return seedImage;
	else if(prob >= 1.0){
		for (unsigned i = 0; i < seedImage->height * seedImage->width; i++)
			*(seedImage->data + i) = color;
		return seedImage;
	} else {
		double chance = 0.0;
		for (unsigned p = 0; p < seedImage->width * seedImage->height; p++) {
			chance = (double)rand() / (double)RAND_MAX;
			if (chance <= prob) *(seedImage->data + p) = color; // if chance is less than probability change to target color
			else *(seedImage->data + p) = *(refImage->data + p); // otherwise copy ref contents
		}
	}

	return seedImage;
}

Rasteron_Image* seedweightedImgOp(ref_image_t refImage, const ColorSeedTable* seedTable){
	assert(refImage != NULL);

	Rasteron_Image* seedImage = alloc_image("palette", refImage->height, refImage->width);
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
		chance = ((double)rand() / (double)RAND_MAX) * seedWeightTotal;
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