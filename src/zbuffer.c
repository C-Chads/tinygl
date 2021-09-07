/*

 * Z buffer: 16 bits Z / 16 bits color
 *
 */

#include <stdlib.h>
#include <string.h>

#include "../include/zbuffer.h"
#include "msghandling.h"
ZBuffer* ZB_open(GLint xsize, GLint ysize, GLint mode,

				 void* frame_buffer) {
	ZBuffer* zb;
	GLint size;

	zb = gl_malloc(sizeof(ZBuffer));
	if (zb == NULL)
		return NULL;

	zb->xsize = xsize & ~3; 
	zb->ysize = ysize;
	
	
	zb->linesize = (xsize * PSZB);

	switch (mode) {
#if TGL_FEATURE_32_BITS == 1
	case ZB_MODE_RGBA:
		break;
#endif
#if TGL_FEATURE_16_BITS == 1
	case ZB_MODE_5R6G5B:
		break;
#endif

	default:
		goto error;
	}

	size = zb->xsize * zb->ysize * sizeof(GLushort);

	zb->zbuf = gl_malloc(size);
	if (zb->zbuf == NULL)
		goto error;

	if (frame_buffer == NULL) {
		zb->pbuf = gl_malloc(zb->ysize * zb->linesize);
		if (zb->pbuf == NULL) {
			gl_free(zb->zbuf);
			goto error;
		}
		zb->frame_buffer_allocated = 1;
	} else {
		zb->frame_buffer_allocated = 0;
		zb->pbuf = frame_buffer;
	}

	zb->current_texture = NULL;

	return zb;
error:
	gl_free(zb);
	return NULL;
}

void ZB_close(ZBuffer* zb) {

	if (zb->frame_buffer_allocated)
		gl_free(zb->pbuf);

	gl_free(zb->zbuf);
	gl_free(zb);
}

void ZB_resize(ZBuffer* zb, void* frame_buffer, GLint xsize, GLint ysize) {
	GLint size;

	/* xsize must be a multiple of 4 */
	xsize = xsize & ~3;

	zb->xsize = xsize;
	zb->ysize = ysize;
	zb->linesize = (xsize * PSZB);

	size = zb->xsize * zb->ysize * sizeof(GLushort);

	gl_free(zb->zbuf);
	zb->zbuf = gl_malloc(size);
	if (zb->zbuf == NULL)
		exit(1);
	if (zb->frame_buffer_allocated)
		gl_free(zb->pbuf);

	if (frame_buffer == NULL) {
		zb->pbuf = gl_malloc(zb->ysize * zb->linesize);
		if (!zb->pbuf)
			exit(1);
		zb->frame_buffer_allocated = 1;
	} else {
		zb->pbuf = frame_buffer;
		zb->frame_buffer_allocated = 0;
	}
}

#if TGL_FEATURE_32_BITS == 1
 PIXEL pxReverse32(PIXEL x) {
	return
		((x & 0xFF000000) >> 24) | /*______AA*/
		((x & 0x00FF0000) >> 8) |  /*____RR__*/
		((x & 0x0000FF00) << 8) |  /*__GG____*/
		((x & 0x000000FF) << 24);  /* BB______*/
}
#endif

static void ZB_copyBuffer(ZBuffer* zb, void* buf, GLint linesize) {
	GLint y, i;
#if TGL_FEATURE_MULTITHREADED_ZB_COPYBUFFER == 1
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (y = 0; y < zb->ysize; y++) {
		PIXEL* q;
		GLubyte* p1;
		q = zb->pbuf + y * zb->xsize;
		p1 = (GLubyte*)buf + y * linesize;
#if TGL_FEATURE_NO_COPY_COLOR == 1
		for (i = 0; i < zb->xsize; i++) {
			if ((*(q + i) & TGL_COLOR_MASK) != TGL_NO_COPY_COLOR)
				*(((PIXEL*)p1) + i) = *(q + i);
		}
#else
		memcpy(p1, q, linesize);
#endif


	}
#else
	for (y = 0; y < zb->ysize; y++) {
		PIXEL* q;
		GLubyte* p1;
		q = zb->pbuf + y * zb->xsize;
		p1 = (GLubyte*)buf + y * linesize;
#if TGL_FEATURE_NO_COPY_COLOR == 1
		for (i = 0; i < zb->xsize; i++) {
			if ((*(q + i) & TGL_COLOR_MASK) != TGL_NO_COPY_COLOR)
				*(((PIXEL*)p1) + i) = *(q + i);
		}
#else
		memcpy(p1, q, linesize);
#endif
	}
#endif
}

#if TGL_FEATURE_RENDER_BITS == 16

/* 32 bpp copy */
/*

#ifdef TGL_FEATURE_32_BITS

#define RGB16_TO_RGB32(p0,p1,v)\
{\
	GLuint g,b,gb;\
	g = (v & 0x07E007E0) << 5;\
	b = (v & 0x001F001F) << 3;\
	gb = g | b;\
	p0 = (gb & 0x0000FFFF) | ((v & 0x0000F800) << 8);\
	p1 = (gb >> 16) | ((v & 0xF8000000) >> 8);\
}


static void ZB_copyFrameBufferRGB32(ZBuffer * zb,
									void *buf,
									GLint linesize)
{
	GLushort *q;
	GLuint *p, *p1, v, w0, w1;
	GLint y, n;

	q = zb->pbuf;
	p1 = (GLuint *) buf;
	
	for (y = 0; y < zb->ysize; y++) {
	p = p1;
	n = zb->xsize >> 2;
	do {
		v = *(GLuint *) q;
		RGB16_TO_RGB32(w1, w0, v);
		p[0] = w0;
		p[1] = w1;
		v = *(GLuint *) (q + 2);
		RGB16_TO_RGB32(w1, w0, v);
		p[2] = w0;
		p[3] = 0;

		q += 4;
		p += 4;
	} while (--n > 0);

	p1 += linesize;
	}
}
*/
#endif

/* 24 bit packed pixel handling */

#ifdef TGL_FEATURE_24_BITS

/* order: RGBR GBRG BRGB */

/* XXX: packed pixel 24 bit support not tested */
/* XXX: big endian case not optimised */
/*
#if BYTE_ORDER == BIG_ENDIAN

#define RGB16_TO_RGB24(p0,p1,p2,v1,v2)\
{\
	GLuint r1,g1,b1,gb1,g2,b2,gb2;\
	v1 = (v1 << 16) | (v1 >> 16);\
	v2 = (v2 << 16) | (v2 >> 16);\
	r1 = (v1 & 0xF800F800);\
	g1 = (v1 & 0x07E007E0) << 5;\
	b1 = (v1 & 0x001F001F) << 3;\
	gb1 = g1 | b1;\
	p0 = ((gb1 & 0x0000FFFF) << 8) | (r1 << 16) | (r1 >> 24);\
	g2 = (v2 & 0x07E007E0) << 5;\
	b2 = (v2 & 0x001F001F) << 3;\
	gb2 = g2 | b2;\
	p1 = (gb1 & 0xFFFF0000) | (v2 & 0xF800) | ((gb2 >> 8) & 0xff);\
	p2 = (gb2 << 24) | ((v2 & 0xF8000000) >> 8) | (gb2 >> 16);\
}

#else

#define RGB16_TO_RGB24(p0,p1,p2,v1,v2)\
{\
	GLuint r1,g1,b1,gb1,g2,b2,gb2;\
	r1 = (v1 & 0xF800F800);\
	g1 = (v1 & 0x07E007E0) << 5;\
	b1 = (v1 & 0x001F001F) << 3;\
	gb1 = g1 | b1;\
	p0 = ((gb1 & 0x0000FFFF) << 8) | (r1 << 16) | (r1 >> 24);\
	g2 = (v2 & 0x07E007E0) << 5;\
	b2 = (v2 & 0x001F001F) << 3;\
	gb2 = g2 | b2;\
	p1 = (gb1 & 0xFFFF0000) | (v2 & 0xF800) | ((gb2 >> 8) & 0xff);\
	p2 = (gb2 << 24) | ((v2 & 0xF8000000) >> 8) | (gb2 >> 16);\
}

#endif
*/
/*
static void ZB_copyFrameBufferRGB24(ZBuffer * zb,
									void *buf,
									GLint linesize)
{
	GLushort *q;
	GLuint *p, *p1, w0, w1, w2, v0, v1;
	GLint y, n;

	q = zb->pbuf;
	p1 = (GLuint *) buf;
	linesize = linesize * 3;

	for (y = 0; y < zb->ysize; y++) {
	p = p1;
	n = zb->xsize >> 2;
	do {
		v0 = *(GLuint *) q;
		v1 = *(GLuint *) (q + 2);
		RGB16_TO_RGB24(w0, w1, w2, v0, v1);
		p[0] = w0;
		p[1] = w1;
		p[2] = w2;

		q += 4;
		p += 3;
	} while (--n > 0);

	*((GLbyte *) p1) += linesize;
	}
}
*/
#endif

#if TGL_FEATURE_RENDER_BITS == 16

void ZB_copyFrameBuffer(ZBuffer* zb, void* buf, GLint linesize) {

	ZB_copyBuffer(zb, buf, linesize);
}

#endif 
/*^ TGL_FEATURE_RENDER_BITS == 16 */


#if TGL_FEATURE_RENDER_BITS == 32

#define RGB32_TO_RGB16(v) (((v >> 8) & 0xf800) | (((v) >> 5) & 0x07e0) | (((v)&0xff) >> 3))


void ZB_copyFrameBuffer(ZBuffer* zb, void* buf, GLint linesize) {
	ZB_copyBuffer(zb, buf, linesize);
}

#endif 
/* ^TGL_FEATURE_RENDER_BITS == 32 */

/*
 * adr must be aligned on an 'int'
 */
static void memset_s(void* adr, GLint val, GLint count) {
	GLint i, n, v;
	GLuint* p;
	GLushort* q;

	p = adr;
	v = val | (val << 16);

	n = count >> 3;
	for (i = 0; i < n; i++) {
		p[0] = v;
		p[1] = v;
		p[2] = v;
		p[3] = v;
		p += 4;
	}

	q = (GLushort*)p;
	n = count & 7;
	for (i = 0; i < n; i++)
		*q++ = val;
}

/* Used in 32 bit mode*/
static void memset_l(void* adr, GLint val, GLint count) {
	GLint i, n, v;
	GLuint* p;
	p = adr;
	v = val;
	n = count >> 2;
	for (i = 0; i < n; i++) {
		p[0] = v;
		p[1] = v;
		p[2] = v;
		p[3] = v;
		p += 4;
	}
	n = count & 3;
	for (i = 0; i < n; i++)
		*p++ = val;
}

void ZB_clear(ZBuffer* zb, GLint clear_z, GLint z, GLint clear_color, GLint r, GLint g, GLint b) {
	GLuint color;
	GLint y;
	PIXEL* pp;
	if (clear_z) {
		memset_s(zb->zbuf, z, zb->xsize * zb->ysize);
	}
	if (clear_color) {
		pp = zb->pbuf;
		for (y = 0; y < zb->ysize; y++) {
#if TGL_FEATURE_RENDER_BITS == 15 || TGL_FEATURE_RENDER_BITS == 16
			// color = RGB_TO_PIXEL(r, g, b);
#if TGL_FEATURE_FORCE_CLEAR_NO_COPY_COLOR
			color = TGL_NO_COPY_COLOR;
#else
			color = RGB_TO_PIXEL(r, g, b);
#endif
			memset_s(pp, color, zb->xsize);
#elif TGL_FEATURE_RENDER_BITS == 32
#if TGL_FEATURE_FORCE_CLEAR_NO_COPY_COLOR
			color = TGL_NO_COPY_COLOR;
#else
			color = RGB_TO_PIXEL(r, g, b);
#endif
			memset_l(pp, color, zb->xsize);
#else
#error BADJUJU
#endif
			pp = (PIXEL*)((GLbyte*)pp + zb->linesize);
		}
	}
}
