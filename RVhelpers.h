#ifndef RV_HELPERS_H
#define RV_HELPERS_H

#include "pmsis.h"

#define NON_MAX_THRES 10

typedef struct{
	uint32_t x;
	uint32_t y;
	uint32_t w;
	uint32_t h;
	int16_t score;
	uint16_t class;
	uint8_t alive;
}bbox_t;

typedef struct{
	bbox_t * bbs;
	int16_t num_bb;
}bboxs_t;


int rect_intersect_area( short a_x, short a_y, short a_w, short a_h,
                         short b_x, short b_y, short b_w, short b_h );


void non_max_suppress(bboxs_t * boundbxs);

void drawBboxes(bboxs_t *boundbxs, uint8_t *img);


#endif
