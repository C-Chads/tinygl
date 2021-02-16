#include "../include/zbuffer.h"
#include <stdlib.h>

#define ZCMP(z, zpix) ((z) >= (zpix))

void ZB_plot(ZBuffer* zb, ZBufferPoint* p) {
	GLushort* pz;
	PIXEL* pp;
	GLint zz;
#if TGL_FEATURE_NO_DRAW_COLOR == 1
	PIXEL col;
#endif
	pz = zb->zbuf + (p->y * zb->xsize + p->x);
	pp = (PIXEL*)((GLbyte*)zb->pbuf + zb->linesize * p->y + p->x * PSZB);
	zz = p->z >> ZB_POINT_Z_FRAC_BITS;
	if (ZCMP(zz, *pz)) {
//#if TGL_FEATURE_RENDER_BITS == 24
// pp[0]=p->r>>8;
// pp[1]=p->g>>8;
// pp[2]=p->b>>8;
//#else
#if TGL_FEATURE_NO_DRAW_COLOR == 1
#define NODRAWTEST(color) ((color & TGL_COLOR_MASK) != TGL_NO_DRAW_COLOR)
		col = RGB_TO_PIXEL(p->r, p->g, p->b);
		if (NODRAWTEST(col))
			*pp = RGB_TO_PIXEL(p->r, p->g, p->b);
#else
		*pp = RGB_TO_PIXEL(p->r, p->g, p->b);
#endif
		//#endif
		*pz = zz;
	}
}

#define INTERP_Z
static void ZB_line_flat_z(ZBuffer* zb, ZBufferPoint* p1, ZBufferPoint* p2, GLint color) {
#include "zline.h"
}

/* line with color GLinterpolation */
#define INTERP_Z
#define INTERP_RGB
static void ZB_line_interp_z(ZBuffer* zb, ZBufferPoint* p1, ZBufferPoint* p2) {
#include "zline.h"
}

/* no Z GLinterpolation */

static void ZB_line_flat(ZBuffer* zb, ZBufferPoint* p1, ZBufferPoint* p2, GLint color) {
#include "zline.h"
}

#define INTERP_RGB
static void ZB_line_interp(ZBuffer* zb, ZBufferPoint* p1, ZBufferPoint* p2) {
#include "zline.h"
}

void ZB_line_z(ZBuffer* zb, ZBufferPoint* p1, ZBufferPoint* p2) {
	GLint color1, color2;

	color1 = RGB_TO_PIXEL(p1->r, p1->g, p1->b);
	color2 = RGB_TO_PIXEL(p2->r, p2->g, p2->b);

	/* choose if the line should have its color GLinterpolated or not */
	if (color1 == color2) {
		ZB_line_flat_z(zb, p1, p2, color1);
	} else {
		ZB_line_interp_z(zb, p1, p2);
	}
}

void ZB_line(ZBuffer* zb, ZBufferPoint* p1, ZBufferPoint* p2) {
	GLint color1, color2;

	color1 = RGB_TO_PIXEL(p1->r, p1->g, p1->b);
	color2 = RGB_TO_PIXEL(p2->r, p2->g, p2->b);

	/* choose if the line should have its color GLinterpolated or not */
	if (color1 == color2) {
		ZB_line_flat(zb, p1, p2, color1);
	} else {
		ZB_line_interp(zb, p1, p2);
	}
}
