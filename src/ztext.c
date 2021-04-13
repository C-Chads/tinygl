#include "../include/GL/gl.h"
#include "../include/zbuffer.h"
#include "font8x8_basic.h"
#include "zgl.h"


#include <stdlib.h>


void glTextSize(GLTEXTSIZE mode) {
#define NEED_CONTEXT
#include "error_check_no_context.h"
#if TGL_FEATURE_ERROR_CHECK == 1
	if (mode < 1 || GL_MAX_TEXT_SIZE < mode)
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"
#endif
		GLParam p[2];
	p[0].op = OP_TextSize;
	p[1].ui = mode;
	gl_add_op(p);
}
void glopTextSize(GLParam* p) {
	GLContext* c = gl_get_context();
	c->textsize = p[1].ui;
} 
static void renderchar(GLbyte* bitmap, GLint _x, GLint _y, GLuint p) {
	GLint x, y, i, j;
	GLint set;
	GLContext* c = gl_get_context();
	GLint mult = c->textsize;
	for (x = 0; x < 8; x++) {
		for (y = 0; y < 8; y++) {
			set = bitmap[x] & (1 << y);
			if (set)
				for (i = 0; i < mult; i++)
					for (j = 0; j < mult; j++)
						glPlotPixel(y * mult + i + _x, x * mult + j + _y, p);
		}
	}
}

void glopPlotPixel(GLParam* p) {
	GLContext* c = gl_get_context();
	GLint x = p[1].i;
	PIXEL pix = p[2].ui;
	c->zb->pbuf[x] = pix;
	
}

void glPlotPixel(GLint x, GLint y, GLuint pix) {
	GLParam p[3];
	GLContext* c = gl_get_context();
#include "error_check.h"
	
	GLint w = c->zb->xsize;
	GLint h = c->zb->ysize;
	p[0].op = OP_PlotPixel;

	if (x > -1 && x < w && y > -1 && y < h) {
#if TGL_FEATURE_RENDER_BITS == 16
		pix = RGB_TO_PIXEL((pix & COLOR_MULT_MASK), ((pix & 0xFF00) << (COLOR_SHIFT - 8)), ((pix & 255) << COLOR_SHIFT));
#endif
		p[1].i = x + y * w;
		p[2].ui = pix;
		gl_add_op(p);
	}
}
void glDrawText(const GLubyte* text, GLint x, GLint y, GLuint p) {
	GLContext* c = gl_get_context();
	GLint i = 0;
#include "error_check.h"

#if TGL_FEATURE_ERROR_CHECK == 1
	if (!text)
#define ERROR_FLAG GL_INVALID_VALUE
#include "error_check.h"
#endif
		
		GLint w = c->zb->xsize;
	GLint h = c->zb->ysize;
	GLint xoff = 0;
	GLint yoff = 0;
	GLint mult = c->textsize;
	for (; text[i] != '\0' && y + 7 < h; i++) {
		if (text[i] != '\n' && xoff + x < w) {
			renderchar(font8x8_basic[text[i]], x + xoff, y + yoff, p);
			xoff += 8 * mult;
		} else if (text[i] == '\n') {
			xoff = 0;
			yoff += 8 * mult;
		}
	}
}
