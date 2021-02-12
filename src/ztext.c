#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "zbuffer.h"
#include "include/GL/gl.h"
#include "zgl.h"
#include "font8x8_basic.h"
GLTEXTSIZE textsize = 1;

void glTextSize(GLTEXTSIZE mode){textsize = mode;}//Set text size
void renderchar(char *bitmap, int _x, int _y, PIXEL p) {
    int x,y;
    int set;
    //int mask;
    int mult = textsize;
    for (x=0; x < 8; x++) {
        for (y=0; y < 8; y++) {
            set = bitmap[x] & 1 << y;
			if(set)
				for(int i = 0; i < mult; i++)
				for(int j = 0; j < mult; j++)
				glPlotPixel(y*mult + i + _x, x*mult + j + _y, p);
        }
    }
}
void glPlotPixel(int x, int y, PIXEL p){
	PIXEL* pbuf = gl_get_context()->zb->pbuf;
	int w = gl_get_context()->zb->xsize;
	int h = gl_get_context()->zb->ysize;
	
	if(x>0 && x<w && y>0 && y < h)
		pbuf[x+y*w] = p;
}
void glDrawText(const unsigned char* text, int x, int y, PIXEL p){
	if(!text)return;
	//PIXEL* pbuf = gl_get_context()->zb->pbuf;
	int w = gl_get_context()->zb->xsize;
	int h = gl_get_context()->zb->ysize;
	int xoff = 0;
	int yoff = 0;
	int mult = textsize;
	for(int i = 0; text[i] != '\0' && y+7 < h; i++){
		if(text[i] != '\n' && text[i] < 127 && xoff+x < w)
		{
			renderchar(font8x8_basic[text[i]],x+xoff,y+yoff, p);
			xoff+=8*mult;
		}else if(text[i] == '\n'){
			xoff=0;
			yoff+=8*mult;
		}
	}
}
