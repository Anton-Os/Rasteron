#include "Rasteron.h"

Rasteron_Image* insertImgOp(ref_image_t image1, ref_image_t image2, float scale){
	assert(image1 != NULL && image2 != NULL && image1->width == image2->width && image1->height == image2->height);

	ref_image_t innerImg = (scale < 1.0)? image1 : image2; 
	ref_image_t outerImg = (scale < 1.0)? image2 : image1;
	if(scale > 1.0) scale = 1.0 / scale;

	unsigned xBounds[2] = { floor(innerImg->width * scale), ceil(innerImg->width - (innerImg->width * scale)) };
	unsigned yBounds[2] = { floor(innerImg->height * scale), ceil(innerImg->height - (innerImg->height * scale)) };

	Rasteron_Image* embedImg = alloc_image("insert", innerImg->height, innerImg->width);
	for(unsigned p = 0; p < innerImg->width * innerImg->height; p++){
		unsigned xOffset = p % embedImg->width; // absolute X pixel offset
        unsigned yOffset = p / embedImg->width; // absolute Y pixel offset

		if((xOffset > xBounds[0] && xOffset < xBounds[1]) && (yOffset > yBounds[0] && yOffset < yBounds[1])) 
			*(embedImg->data + p) = *(innerImg->data + p); // copies from inner image if inside
		else *(embedImg->data + p) = *(outerImg->data + p); // copies from outer image if outside
	}

	return embedImg;
}

Rasteron_Image* dualcolorImgOp(ref_image_t image1, ref_image_t image2, dualcolorCallback callback){
	assert(image1 != NULL && image2 != NULL && image1->width == image2->width && image1->height == image2->height);

	Rasteron_Image* dualImage = alloc_image("dual", image1->height, image1->width);
	for(unsigned p = 0; p < image1->width * image1->height; p++)
		*(dualImage->data + p) = callback(*(image1->data + p), *(image2->data + p));

	return dualImage;
}


Rasteron_Image* blendImgOp(ref_image_t image1, ref_image_t image2){
	assert(image1 != NULL && image2 != NULL && image1->width == image2->width && image1->height == image2->height);

	Rasteron_Image* blendImage = alloc_image("blend", image1->height, image1->width);
	for(unsigned p = 0; p < image1->width * image1->height; p++)
		*(blendImage->data + p) = blend(*(image1->data + p), *(image2->data + p), 0.5); // blends halfway

	return blendImage;
}

Rasteron_Image* fusionImgOp(ref_image_t image1, ref_image_t image2){
	assert(image1 != NULL && image2 != NULL && image1->width == image2->width && image1->height == image2->height);

	Rasteron_Image* fuseImage = alloc_image("fuse", image1->height, image1->width);
	for(unsigned p = 0; p < image1->width * image1->height; p++)
		*(fuseImage->data + p) = fuse(*(image1->data + p), *(image2->data + p), 0.5); // fuses halfway

	return fuseImage;
}