/*
 * img2jpg.c
 *
 *  Created on: Mar 24, 2020
 *      Author: tp
 */



/* Includes ------------------------------------------------------------------*/
#include <pmsis.h>
#include "gaplib/jpeg_encoder.h"
#include <bsp/fs.h>
#include <bsp/flash/hyperflash.h>

/* USER CODE BEGIN Includes */

#include "img2jpg.h"


static uint8_t jpeg_image[0xFFFF];

/**
 * @brief  Send Img Raw
 * @param  img: Img pointer
 * @param  img_s: Img size pointer
 * @retval img: Img jpeg pointer
 */


void ImgFree(uint8_t* img){
	int image_size;
   	image_size = FRAME411_WIDTH * FRAME411_HEIGHT;
	//pi_l2_free(img,image_size);
	pmsis_l2_malloc_free(img,image_size);

	return;
}

uint8_t* Img2jpg(uint8_t *img, int *img_s){
	jpeg_encoder_t enc;
	unsigned int width, height;
	int image_size;
	int pgm_header_size;

	unsigned char *image = img;
	width = FRAME411_WIDTH;
	height = FRAME411_HEIGHT;
	image_size = FRAME411_WIDTH * FRAME411_HEIGHT;
	pgm_header_size = 1;
	if (image == NULL)
		return 0;

	// Allocate output jpeg image
	if(jpeg_image == 0){
		//jpeg_image = pi_l2_malloc(image_size);
		//jpeg_image = pi_l2_malloc(0xFFFF);
		//jpeg_image = (unsigned char *) pmsis_l2_malloc(image_size);
		if (jpeg_image == NULL)
			return 0;
	}

	// Open JPEG encoder
	printf("Start JPEG encoding\n\r");

	struct jpeg_encoder_conf enc_conf;

	jpeg_encoder_conf_init(&enc_conf);

	enc_conf.width = width;
	enc_conf.height = height;

	if (jpeg_encoder_open(&enc, &enc_conf))
		return 0;

	printf("Open JPEG encoding\n\r");

	if (jpeg_encoder_start(&enc))
		return 0;

	printf("Start JPEG encoding\n\r");

	// Get the header so that we can produce full JPEG image
	pi_buffer_t bitstream;
	bitstream.data = jpeg_image;
	bitstream.size = image_size;
	uint32_t header_size, footer_size, body_size;

	if (jpeg_encoder_header(&enc, &bitstream, &header_size))
		return 0;

	printf("Header JPEG encoding. Header size: %d\n\r",(int)header_size);

	// Now get the encoded image
	pi_buffer_t buffer;
	buffer.data = &image[pgm_header_size];
	buffer.size = image_size - pgm_header_size;
	buffer.width = width;
	buffer.height = height;
	bitstream.data = &jpeg_image[header_size];

	if (jpeg_encoder_process(&enc, &buffer, &bitstream, &body_size))
		return 0;

	printf("Process JPEG encoding\n\r");

	// An finally get the footer
	bitstream.data = &jpeg_image[body_size + header_size];
	if (jpeg_encoder_footer(&enc, &bitstream, &footer_size))
		return 0;

	int bitstream_size = body_size + header_size + footer_size;

	printf("Encoding done at addr %p size %d\n\r", jpeg_image, bitstream_size);

	jpeg_encoder_stop(&enc);
	jpeg_encoder_close(&enc);

	//return
	*img_s = bitstream_size;
	return jpeg_image;

}
