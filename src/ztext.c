#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../include/zbuffer.h"
#include "../include/GL/gl.h"
#include "zgl.h"
#include "font8x8_basic.h"
GLTEXTSIZE textsize = 1;

void glTextSize(GLTEXTSIZE mode){textsize = mode;}//Set text size
void renderGLbyte(GLbyte *bitmap, GLint _x, GLint _y, GLuint p) {
    GLint x,y;
    GLint set;
    //int mask;
    GLint mult = textsize;
    for (x=0; x < 8; x++) {
        for (y=0; y < 8; y++) {
            set = bitmap[x] & 1 << y;
			if(set)
				for(GLint i = 0; i < mult; i++)
				for(GLint j = 0; j < mult; j++)
				glPlotPixel(y*mult + i + _x, x*mult + j + _y, p);
        }
    }
}


void glPlotPixel(GLint x, GLint y, GLuint p){
//	int x = p[1].i;
//	int y = p[2].i;
//	GLuint p = p[3].ui;
#if TGL_FEATURE_RENDER_BITS == 16
	p = RGB_TO_PIXEL( ( (p & 255) << 8) , ( p & 65280) , ( (p >>16)<<8 ) );
#endif
	PIXEL* pbuf = gl_get_context()->zb->pbuf;
	int w = gl_get_context()->zb->xsize;
	int h = gl_get_context()->zb->ysize;
	
	if(x>0 && x<w && y>0 && y < h)
		pbuf[x+y*w] = p;
}
void glDrawText(const GLubyte* text, GLint x, GLint y, GLuint p){
	if(!text)return;
	//PIXEL* pbuf = gl_get_context()->zb->pbuf;
	int w = gl_get_context()->zb->xsize;
	int h = gl_get_context()->zb->ysize;
	int xoff = 0;
	int yoff = 0;
	int mult = textsize;
	for(GLint i = 0; text[i] != '\0' && y+7 < h; i++){
		if(text[i] != '\n' && text[i] < 127 && xoff+x < w)
		{
			renderGLbyte(font8x8_basic[text[i]],x+xoff,y+yoff, p);
			xoff+=8*mult;
		}else if(text[i] == '\n'){
			xoff=0;
			yoff+=8*mult;
		}
	}
}
