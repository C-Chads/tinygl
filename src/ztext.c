#include "../include/GL/gl.h"
#include "../include/zbuffer.h"
#include "font8x8_basic.h"
#include "zgl.h"
#include <assert.h>
//#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
GLTEXTSIZE textsize = 1;

void glTextSize(GLTEXTSIZE mode) {
	GLParam p[2];
	p[0].op = OP_TextSize;
	p[1].ui = mode;
	gl_add_op(p);
}
void glopTextSize(GLContext* c, GLParam* p) { textsize = p[1].ui; } // Set text size
void renderchar(GLbyte* bitmap, GLint _x, GLint _y, GLuint p) {
	GLint x, y;
	GLint set;
	// int mask;
	GLint mult = textsize;
	for (x = 0; x < 8; x++) {
		for (y = 0; y < 8; y++) {
			set = bitmap[x] & 1 << y;
			if (set)
				for (GLint i = 0; i < mult; i++)
					for (GLint j = 0; j < mult; j++)
						glPlotPixel(y * mult + i + _x, x * mult + j + _y, p);
		}
	}
}

void glopPlotPixel(GLContext* c, GLParam* p) {
	GLint x = p[1].i;
	PIXEL pix = p[2].ui;
	// PIXEL* pbuf = c->zb->pbuf;
	c->zb->pbuf[x] = pix;
}

void glPlotPixel(GLint x, GLint y, GLuint pix) {
	GLParam p[3];

	// PIXEL* pbuf = gl_get_context()->zb->pbuf;
	GLint w = gl_get_context()->zb->xsize;
	GLint h = gl_get_context()->zb->ysize;
	p[0].op = OP_PlotPixel;

	if (x > 0 && x < w && y > 0 && y < h) {
#if TGL_FEATURE_RENDER_BITS == 16
		pix = RGB_TO_PIXEL(((pix & 255) << 8), (pix & 65280), ((pix >> 16) << 8));
#endif
		p[1].i = x + y * w;
		p[2].ui = pix;
		gl_add_op(p);
	}
}
void glDrawText(const GLubyte* text, GLint x, GLint y, GLuint p) {
	if (!text)
		return;
	// PIXEL* pbuf = gl_get_context()->zb->pbuf;
	GLint w = gl_get_context()->zb->xsize;
	GLint h = gl_get_context()->zb->ysize;
	GLint xoff = 0;
	GLint yoff = 0;
	GLint mult = textsize;
	for (GLint i = 0; text[i] != '\0' && y + 7 < h; i++) {
		if (text[i] != '\n' && text[i] < 127 && xoff + x < w) {
			renderchar(font8x8_basic[text[i]], x + xoff, y + yoff, p);
			xoff += 8 * mult;
		} else if (text[i] == '\n') {
			xoff = 0;
			yoff += 8 * mult;
		}
	}
}
