#include "Rasteron.h"

Rasteron_Image* blendsImg(ref_image_t image1, ref_image_t image2){
	assert(image1 != NULL && image2 != NULL && image1->width == image2->width && image1->height == image2->height);

	Rasteron_Image* blendImage = alloc_image("blend", image1->height, image1->width);
	for(unsigned p = 0; p < image1->width * image1->height; p++)
		*(blendImage->data + p) = blend(*(image1->data + p), *(image2->data + p), 0.5); // blends halfway

	return blendImage;
}

Rasteron_Image* fusionsImg(ref_image_t image1, ref_image_t image2){
	assert(image1 != NULL && image2 != NULL && image1->width == image2->width && image1->height == image2->height);

	Rasteron_Image* fuseImage = alloc_image("fuse", image1->height, image1->width);
	for(unsigned p = 0; p < image1->width * image1->height; p++)
		*(fuseImage->data + p) = fuse(*(image1->data + p), *(image2->data + p), 0.5); // fuses halfway

	return fuseImage;
}

Rasteron_Image* embedsImg(ref_image_t image1, ref_image_t image2, float scale){
	assert(image1 != NULL && image2 != NULL && image1->width == image2->width && image1->height == image2->height);

	unsigned xBounds[2] = { floor(image1->width * scale), ceil(image1->width - (image1->width * scale)) };
	unsigned yBounds[2] = { floor(image1->height * scale), ceil(image1->height - (image1->height * scale)) };

	Rasteron_Image* embedImg = alloc_image("embed", image1->height, image1->width);
	for(unsigned p = 0; p < image1->width * image1->height; p++){
		unsigned xOffset = p % embedImg->width; // absolute X pixel offset
        unsigned yOffset = p / embedImg->width; // absolute Y pixel offset

		if((xOffset > xBounds[0] && xOffset < xBounds[1]) && (yOffset > yBounds[0] && yOffset < yBounds[1])) 
			*(embedImg->data + p) = *(image1->data + p); // copies from first image if inside
		else *(embedImg->data + p) = *(image2->data + p); // copies from second image if outside
	}

	return embedImg;
}