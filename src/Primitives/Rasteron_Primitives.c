#include "Rasteron.h"

Rasteron_ImageSq* createImageSq_Rstn(const Image* image){
    	Rasteron_ImageSq* rstn_imageSq = (Rasteron_ImageSq*)malloc(sizeof(Rasteron_ImageSq));
        rstn_imageSq->count = 1;
        // rstn_imageSq->labels[0] = "base"; // Furst label
        rstn_imageSq->data = (uint32_t**)malloc(sizeof(uint32_t*)); // Just using a base imaage for now!

        switch(image->fileFormat){

        case(IMG_Tiff):
#ifdef USE_IMG_TIFF
		rstn_imageSq->width = image->imageData.tiff.width;
        rstn_imageSq->height = image->imageData.tiff.length;
        *(rstn_imageSq->data) = (uint32_t*)malloc(rstn_imageSq->width * rstn_imageSq->height * sizeof(uint32_t));
        *(*(rstn_imageSq->data)) = *(image->imageData.tiff.raster); // One member so far, we are copying the image to "base"
#endif
		break;

	case(IMG_Bmp):
#ifdef USE_IMG_BMP
		rstn_imageSq->width = image->imageData.bmp.width;
        rstn_imageSq->height = image->imageData.bmp.height;
        *(rstn_imageSq->data) = (uint32_t*)malloc(rstn_imageSq->width * rstn_imageSq->height * sizeof(uint32_t));
        *(rstn_imageSq->data) = *(image->imageData.bmp.data);
#endif
		break;
	
    case(IMG_Png):
#ifdef USE_IMG_PNG
		rstn_imageSq->width = image->imageData.png.width;
        rstn_imageSq->height = image->imageData.png.height;
        *(rstn_imageSq->data) = (uint32_t*)malloc(rstn_imageSq->width * rstn_imageSq->height * sizeof(uint32_t));
        *(rstn_imageSq->data) = *(image->imageData.png.rgbaData);
#endif
		break;

	default:
		puts("Image Format not yet supported!!!");
		break;
	}

	return rstn_imageSq;
}

void delImageSq_Rstn(Rasteron_ImageSq* rstn_image){
    for(unsigned short i = 0; i < rstn_image->count; i++)
        free(*(rstn_image->data) + i);
    free(rstn_image->data);
    free(rstn_image);
}