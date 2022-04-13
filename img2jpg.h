/*
 * img2jpg.h
 *
 *  Created on: Mar 24, 2020
 *      Author: tp
 */

#ifndef IMG2JPG_H_
#define IMG2JPG_H_

/* Defines */
#ifndef FRAME411_HEIGHT
#define FRAME411_HEIGHT 244
#endif
#ifndef FRAME411_WIDTH
#define FRAME411_WIDTH 324
#endif



uint8_t* Img2jpg(uint8_t *img, int *img_s);
void ImgFree(uint8_t* img);

#endif /* IMG2JPG_H_ */
