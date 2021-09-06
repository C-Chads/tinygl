#include "../include/GL/gl.h"
#include "../include/zbuffer.h"
#include "msghandling.h"
#include "zgl.h"

static void gl_vertex_transform_raster(GLVertex* v) {
	GLContext* c = gl_get_context();

	{
		/* no eye coordinates needed, no normal */
		/* NOTE: W = 1 is assumed */
		GLfloat* m = &c->matrix_model_projection.m[0][0];

		v->pc.X = (v->coord.X * m[0] + v->coord.Y * m[1] + v->coord.Z * m[2] + m[3]);
		v->pc.Y = (v->coord.X * m[4] + v->coord.Y * m[5] + v->coord.Z * m[6] + m[7]);
		v->pc.Z = (v->coord.X * m[8] + v->coord.Y * m[9] + v->coord.Z * m[10] + m[11]);

		if (c->matrix_model_projection_no_w_transform) {
			v->pc.W = m[15];
		} else {
			v->pc.W = (v->coord.X * m[12] + v->coord.Y * m[13] + v->coord.Z * m[14] + m[15]);
		}
		m = &c->matrix_stack_ptr[0]->m[0][0];
		v->ec.X = (v->coord.X * m[0] + v->coord.Y * m[1] + v->coord.Z * m[2] + m[3]);
		v->ec.Y = (v->coord.X * m[4] + v->coord.Y * m[5] + v->coord.Z * m[6] + m[7]);
		v->ec.Z = (v->coord.X * m[8] + v->coord.Y * m[9] + v->coord.Z * m[10] + m[11]);
		v->ec.W = (v->coord.X * m[12] + v->coord.Y * m[13] + v->coord.Z * m[14] + m[15]);
	}

	v->clip_code = gl_clipcode(v->pc.X, v->pc.Y, v->pc.Z, v->pc.W);
}

void glRasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
	GLParam p[5];
	p[0].op = OP_RasterPos;
	p[1].f = x;
	p[2].f = y;
	p[3].f = z;
	p[4].f = w;
	gl_add_op(p);
}
void glopRasterPos(GLParam* p) {
	GLContext* c = gl_get_context();
	GLVertex v;
	v.coord.X = p[1].f;
	v.coord.Y = p[2].f;
	v.coord.Z = p[3].f;
	v.coord.W = p[4].f;
	gl_vertex_transform_raster(&v);
	if (v.clip_code == 0) {
		{
			GLfloat winv = 1.0 / v.pc.W;
			v.zp.x = (GLint)(v.pc.X * winv * c->viewport.scale.X + c->viewport.trans.X);
			v.zp.y = (GLint)(v.pc.Y * winv * c->viewport.scale.Y + c->viewport.trans.Y);
			v.zp.z = (GLint)(v.pc.Z * winv * c->viewport.scale.Z + c->viewport.trans.Z);
		}
		c->rasterpos.v[0] = v.zp.x;
		c->rasterpos.v[1] = v.zp.y;
		c->rastervertex = v;
		/* c->rasterpos.v[2] = v.zp.z;*/
		c->rasterpos_zz = v.zp.z >> ZB_POINT_Z_FRAC_BITS; 
		c->rasterposvalid = 1;
	} else
		c->rasterposvalid = 0;
}

void glRasterPos2f(GLfloat x, GLfloat y) { glRasterPos4f(x, y, 0, 1); }
void glRasterPos3f(GLfloat x, GLfloat y, GLfloat z) { glRasterPos4f(x, y, z, 1); }

void glRasterPos2fv(GLfloat* v) { glRasterPos2f(v[0], v[1]); }
void glRasterPos3fv(GLfloat* v) { glRasterPos3f(v[0], v[1], v[2]); }
void glRasterPos4fv(GLfloat* v) { glRasterPos4f(v[0], v[1], v[2], v[3]); }

void glDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, void* data) {
	/* TODO: Come up with a clever scheme for storing the data to avoid pointer dependency. */
#if TGL_FEATURE_RENDER_BITS == 32
	if (type != GL_UNSIGNED_INT && type != GL_UNSIGNED_INT_8_8_8_8) {
		tgl_warning("\nERROR: Incorrect type for glDrawPixels. It MUST be GL_UNSIGNED_INT or GL_UNSIGNED_INT_8_8_8_8, A R G B!");
		return;
	}
#elif TGL_FEATURE_RENDER_BITS == 16
	if (type != GL_UNSIGNED_SHORT && type != GL_UNSIGNED_SHORT_5_6_5) {
		tgl_warning("\nERROR: Incorrect type for glDrawPixels. it MUST be GL_UNSIGNED_SHORT or GL_UNSIGNED_SHORT_5_6_5, R5 G6 B5!");
		return;
	}
#else
#error "Bad TGL_FEATURE_RENDER_BITS"
#endif
	if (format != GL_RGB) {
		tgl_warning("\nERROR: Incorrect format for glDrawPixels.");
		return;
	}
	GLParam p[6];
	p[0].op = OP_DrawPixels;
	p[1].i = width;
	p[2].i = height;
	p[3].p = data;
	gl_add_op(p);
}
#define ZCMP(z, zpix) (!(zbdt) || z >= (zpix))
#define CLIPTEST(_x, _y, _w, _h) ((0 <= _x) && (_w > _x) && (0 <= _y) && (_h > _y))
void glopDrawPixels(GLParam* p) {
	GLContext* c = gl_get_context();
	GLint sy, sx, ty, tx;
	
	GLint w = p[1].i;
	GLint h = p[2].i;
	V4 rastpos = c->rasterpos;
	ZBuffer* zb = c->zb;
	PIXEL* d = p[3].p;
	PIXEL* pbuf = zb->pbuf;
	GLushort* zbuf = zb->zbuf;

	GLubyte zbdw = zb->depth_write;
	GLubyte zbdt = zb->depth_test;
	GLint tw = zb->xsize;
	GLint th = zb->ysize;
	GLfloat pzoomx = c->pzoomx;
	GLfloat pzoomy = c->pzoomy;

	GLint zz = c->rasterpos_zz;
#if TGL_FEATURE_BLEND_DRAW_PIXELS == 1
	TGL_BLEND_VARS
#endif
#if TGL_FEATURE_BLEND == 1
#if TGL_FEATURE_BLEND_DRAW_PIXELS == 1
	GLuint zbeb = zb->enable_blend;
#endif
#endif
	if (!c->rasterposvalid)return;
	
#if TGL_FEATURE_ALT_RENDERMODES == 1
	if (c->render_mode == GL_SELECT) {
		gl_add_select(zz, zz);
		return;
	} else if (c->render_mode == GL_FEEDBACK) {
		gl_add_feedback(GL_DRAW_PIXEL_TOKEN, &(c->rastervertex), NULL, NULL, 0);
		return;
	}
#endif

#if TGL_FEATURE_MULTITHREADED_DRAWPIXELS == 1

#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (sy = 0; sy < h; sy++)
		for (sx = 0; sx < w; sx++) {
			PIXEL col = d[sy * w + sx];
			V4 rastoffset;
			rastoffset.v[0] = rastpos.v[0] + (GLfloat)sx * pzoomx;
			rastoffset.v[1] = rastpos.v[1] - ((GLfloat)(h - sy) * pzoomy);
			rastoffset.v[2] = rastoffset.v[0] + pzoomx;
			rastoffset.v[3] = rastoffset.v[1] - pzoomy;

			for (ty = rastoffset.v[1]; (GLfloat)ty > rastoffset.v[3]; ty--)
				for (tx = rastoffset.v[0]; (GLfloat)tx < rastoffset.v[2]; tx++)
					if (CLIPTEST(tx, ty, tw, th)) {
						GLushort* pz = zbuf + (ty * tw + tx);

						if (ZCMP(zz, *pz)) {

#if TGL_FEATURE_BLEND == 1
#if TGL_FEATURE_BLEND_DRAW_PIXELS == 1
							if (!zbeb)
								pbuf[tx + ty * tw] = col;
							else
								TGL_BLEND_FUNC(col, pbuf[tx + ty * tw])
#else
							pbuf[tx + ty * tw] = col;
#endif
#else
							pbuf[tx + ty * tw] = col;
#endif
							if (zbdw)
								*pz = zz;
						}
					}
		}
#else
	for (sy = 0; sy < h; sy++)
		for (sx = 0; sx < w; sx++) {
			PIXEL col = d[sy * w + sx];
			V4 rastoffset;
			rastoffset.v[0] = rastpos.v[0] + (GLfloat)sx * pzoomx;
			rastoffset.v[1] = rastpos.v[1] - ((GLfloat)(h - sy) * pzoomy);
			rastoffset.v[2] = rastoffset.v[0] + pzoomx;
			rastoffset.v[3] = rastoffset.v[1] - pzoomy;

			for (ty = rastoffset.v[1]; (GLfloat)ty > rastoffset.v[3]; ty--)
				for (tx = rastoffset.v[0]; (GLfloat)tx < rastoffset.v[2]; tx++)
					if (CLIPTEST(tx, ty, tw, th)) {
						GLushort* pz = zbuf + (ty * tw + tx);

						if (ZCMP(zz, *pz)) {

#if TGL_FEATURE_BLEND == 1
#if TGL_FEATURE_BLEND_DRAW_PIXELS == 1
							if (!zbeb)
								pbuf[tx + ty * tw] = col;
							else
								TGL_BLEND_FUNC(col, pbuf[tx + ty * tw])
#else
							pbuf[tx + ty * tw] = col;
#endif
#else
							pbuf[tx + ty * tw] = col;
#endif
							if (zbdw)
								*pz = zz;
						}
					}
		}
#endif
}

void glPixelZoom(GLfloat x, GLfloat y) {
	GLParam p[3];
	p[0].op = OP_PixelZoom;
	p[1].f = x;
	p[2].f = y;
	gl_add_op(p);
}

void glopPixelZoom(GLParam* p) {
	GLContext* c = gl_get_context();
	c->pzoomx = p[1].f;
	c->pzoomy = p[2].f;
}
