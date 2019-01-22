#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "blur_detect.h"


dl_matrix3du_t *bgr2gray(dl_matrix3du_t *image_matrix)
{
	assert(image_matrix->c == 3);
	dl_matrix3du_t *gray_image = dl_matrix3du_alloc(1, image_matrix->w, image_matrix->h, 1);
	uint8_t *g = gray_image->item;
	uint8_t *bgr = image_matrix->item;
	int r = 0;
	int count = (image_matrix->w)*(image_matrix->h); 
	for(int i=0;i<count;i++){
		//printf("b:%d,g:%d,r:%d",*bgr,*(bgr+1),*(bgr+2));
		r = (int)(((*bgr)*114 + (*(bgr+1))*587+(*(bgr+2))*299+500)/1000);
		*g++ = r;
		bgr += 3;
		//printf(" result:%d,gray:%d\n",r,*(g-1));
	}
	return gray_image;
}

float tenengrad_score(dl_matrix3du_t *gray_image)
{
	int tw = (gray_image->w)-2;
	int th = (gray_image->h)-2;
	float gx = 0;
	float gy = 0;
	
	uint8_t *g0 = (gray_image->item);
	uint8_t *g1 = (gray_image->item)+gray_image->h;
	uint8_t *g2 = (gray_image->item)+2*gray_image->h;

	float score = 0;
	for(int i=0;i<tw;i++){
		for(int j=0;j<th;j++){
			gx = (-(*g0)-2*(*(g0+1))-(*(g0+2)))+((*g2)+2*(*(g2+1))+(*(g2+2)));
			gy = ((*g0)-(*(g0+2)))+(2*(*g1)-2*(*(g1+2)))+((*g2)-(*(g2+2)));
			score += sqrt(gx*gx+gy*gy);
			g0++;
			g1++;
			g2++;
		}
		g0 = g0+2;
		g1 = g1+2;
		g2 = g2+2;
	}
	score = score/(tw*th);
	return score;
}


float laplace_score(dl_matrix3du_t *gray_image)
{
	int lw = (gray_image->w)-2;
	int lh = (gray_image->h)-2;
	float r = 0;
	dl_matrix3d_t *laplace_image = dl_matrix3d_alloc(1,lw,lh,1);

	float *l = laplace_image->item;
	uint8_t *g0 = (gray_image->item);
	uint8_t *g1 = (gray_image->item)+gray_image->h;
	uint8_t *g2 = (gray_image->item)+2*gray_image->h;

	float e = 0;
	float score = 0;
	for(int i=0;i<lw;i++){
		for(int j=0;j<lh;j++){
			r = (*(g0+1))+(*(g1)-4*(*(g1+1))+*(g1+2))+(*(g2+1));
			e += r;
			*l++ = r;
			g0++;
			g1++;
			g2++;
		}
		g0 = g0+2;
		g1 = g1+2;
		g2 = g2+2;
	}
	e = e/(lw*lh);
	l = laplace_image->item;
	for(int i=0;i<lw;i++){
		for(int j=0;j<lh;j++){
			score += ((*l)-e)*((*l)-e);
			l++;
		}
	}
	dl_matrix3d_free(laplace_image);
	score = score/(lw*lh);
	return score;
}