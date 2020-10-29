#include "Rasteron.h"

Rasteron_Image* rstnCreate_ImgBase(const Image* image){
	if (image == NULL) {
		puts("Cannot create base image! Null pointer provided!");
		return NULL;
	}

    Rasteron_Image* rstn_image = (Rasteron_Image*)malloc(sizeof(Rasteron_Image));
    rstn_image->name = "base";

    switch(image->fileFormat){

    case(IMG_Tiff):
#ifdef USE_IMG_TIFF
		rstn_image->width = image->imageData.tiff.width;
        rstn_image->height = image->imageData.tiff.length;
        rstn_image->data = (uint32_t*)malloc(rstn_image->width * rstn_image->height * sizeof(uint32_t));
        
		revrsColorBits_RB(image->imageData.tiff.raster, image->imageData.tiff.width * image->imageData.tiff.length);
		for(unsigned i = 0; i < rstn_image->width * rstn_image->height; i++)
		   *(rstn_image->data + i) = *(image->imageData.tiff.raster + i); // Fix the color inversion!!!
#endif
	break;

	case(IMG_Bmp):
#ifdef USE_IMG_BMP
		rstn_image->width = abs(image->imageData.bmp.width);
        rstn_image->height = abs(image->imageData.bmp.height);
        rstn_image->data = (uint32_t*)malloc(rstn_image->width * rstn_image->height * sizeof(uint32_t));

		for (unsigned i = 0; i < rstn_image->width * rstn_image->height; i++)
			*(rstn_image->data + i) = *(image->imageData.bmp.data + i);
#endif
		break;
	
    case(IMG_Png):
#ifdef USE_IMG_PNG
		rstn_image->width = image->imageData.png.width;
        rstn_image->height = image->imageData.png.height;
        rstn_image->data = (uint32_t*)malloc(rstn_image->width * rstn_image->height * sizeof(uint32_t));
		
		for (unsigned i = 0; i < rstn_image->width * rstn_image->height; i++)
			*(rstn_image->data + i) = *(image->imageData.png.rgbaData + i);
#endif
		break;

	default:
		puts("Image Format not yet supported!!!");
		break;
	}

	return rstn_image;
}

Rasteron_Image* rstnCreate_ImgGrey(const Rasteron_Image* ref) {
	if (ref == NULL) {
		puts("Cannot create grey image! Null pointer provided!");
		return NULL;
	}

	Rasteron_Image* rstn_image = (Rasteron_Image*)malloc(sizeof(Rasteron_Image));
	rstn_image->name = "grey";

	rstn_image->height = ref->height;
	rstn_image->width = ref->width;
	
	uint32_t grey;
	rstn_image->data = (uint32_t*)malloc(rstn_image->width * rstn_image->height * sizeof(uint32_t));
	for (unsigned p = 0; p < rstn_image->width * rstn_image->height; p++) {
		unsigned pixColorDbg = *(ref->data + p); // For testing

		grey = (uint32_t)grayify_32(*(ref->data + p));
		*(rstn_image->data + p) = grey;
	}

	return rstn_image;
}

void rstnDel_Img(Rasteron_Image* rstn_image) {
    free(rstn_image->data);
    free(rstn_image);
}