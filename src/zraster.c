#include "../include/GL/gl.h"
#include "../include/zbuffer.h"
#include "zgl.h"
#include "msghandling.h"

static inline void gl_vertex_transform_raster(GLContext* c, GLVertex* v) {
	
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
	}

	v->clip_code = gl_clipcode(v->pc.X, v->pc.Y, v->pc.Z, v->pc.W);
}


void glRasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w){
	GLParam p[5];
	p[0].op = OP_RasterPos;
	p[1].f = x;
	p[2].f = y;
	p[3].f = z;
	p[4].f = w;
	gl_add_op(p);
}
void glopRasterPos(GLContext* c, GLParam* p){
	GLVertex v;
	v.coord.X = p[1].f;
	v.coord.Y = p[2].f;
	v.coord.Z = p[3].f;
	v.coord.W = p[4].f;
	gl_vertex_transform_raster(c, &v);
//	if (v.clip_code == 0)
		{
			{
				GLfloat winv = 1.0 / v.pc.W;
				v.zp.x = (GLint)(v.pc.X * winv * c->viewport.scale.X + c->viewport.trans.X);
				v.zp.y = (GLint)(v.pc.Y * winv * c->viewport.scale.Y + c->viewport.trans.Y);
				v.zp.z = (GLint)(v.pc.Z * winv * c->viewport.scale.Z + c->viewport.trans.Z);
			}
			c->rasterpos.v[0] = v.zp.x;
			c->rasterpos.v[1] = v.zp.y;
			c->rasterpos.v[2] = v.zp.z;
			c->rasterposvalid = 1;
		}
//	else
//		c->rasterposvalid = 0;
}

void glRasterPos2f(GLfloat x, GLfloat y){glRasterPos4f(x,y,0,1);}
void glRasterPos3f(GLfloat x, GLfloat y, GLfloat z){glRasterPos4f(x,y,z,1);}

void glRasterPos2fv(GLfloat* v){glRasterPos2f(v[0],v[1]);}
void glRasterPos3fv(GLfloat* v){glRasterPos3f(v[0],v[1],v[2]);}
void glRasterPos4fv(GLfloat* v){glRasterPos4f(v[0],v[1],v[2],v[3]);}


void glDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, void* data){
	/* TODO: Come up with a clever scheme for storing the data to avoid pointer dependency. */
#if TGL_FEATURE_RENDER_BITS == 32
	if( type != GL_UNSIGNED_INT &&
		type != GL_UNSIGNED_INT_8_8_8_8 )
	{
		tgl_warning("\nERROR: Incorrect type for glDrawPixels. It MUST be GL_UNSIGNED_INT or GL_UNSIGNED_INT_8_8_8_8, A R G B!");
		return;
	}
#elif TGL_FEATURE_RENDER_BITS == 16
	if( type != GL_UNSIGNED_SHORT &&
		type != GL_UNSIGNED_SHORT_5_6_5)
	{
		tgl_warning("\nERROR: Incorrect type for glDrawPixels. it MUST be GL_UNSIGNED_SHORT or GL_UNSIGNED_SHORT_5_6_5, R5 G6 B5!");
		return;
	}
#else 
#error "Bad TGL_FEATURE_RENDER_BITS"
#endif
	if( format != GL_RGB ){
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
#define CLIPTEST(_x,_y,_w,_h)((0<=_x) && (_w>_x) && (0<=_y) && (_h>_y))
void glopDrawPixels(GLContext* c, GLParam* p){
	// p[3]
	if(!c->rasterposvalid) return;
	GLint w = p[1].i;
	GLint h = p[2].i;
	V3 rastpos = c->rasterpos;
	PIXEL* d = p[3].p;
	PIXEL* pbuf = c->zb->pbuf;
	GLint tw = c->zb->xsize;
	GLint th = c->zb->ysize;
	GLfloat pzoomx = c->pzoomx;
	GLfloat pzoomy = c->pzoomy;
	V4 rastoffset;
	rastoffset.v[0] = rastpos.v[0];
	rastoffset.v[1] = rastpos.v[1];
	//Looping over the source pixels.
	for(GLint sx = 0; sx < w; sx++)
	for(GLint sy = 0; sy < h; sy++)
	{
		PIXEL col = d[sy*w+sx];
		rastoffset.v[0] = rastpos.v[0] +  (GLfloat)sx * pzoomx;
		rastoffset.v[1] = rastpos.v[1] - ((GLfloat)(h-sy) * pzoomy);
		rastoffset.v[2] = rastoffset.v[0] + pzoomx;
		rastoffset.v[3] = rastoffset.v[1] - pzoomy;
		for(GLint tx = rastoffset.v[0]; (GLfloat)tx < rastoffset.v[2];tx++)
		for(GLint ty = rastoffset.v[1]; (GLfloat)ty > rastoffset.v[3];ty--)
			if(CLIPTEST(tx,ty,tw,th))
				pbuf[tx+ty*tw] = col;
	}
	/*GLint mult = textsize;
		for (GLint i = 0; i < mult; i++)
		for (GLint j = 0; j < mult; j++)
		glPlotPixel(y * mult + i + _x, x * mult + j + _y, p);
	*/
}




void glPixelZoom(GLfloat x, GLfloat y){
	GLParam p[3];
	p[0].op = OP_PixelZoom;
	p[1].f = x;
	p[2].f = y;
	gl_add_op(p);
}

void glopPixelZoom(GLContext* c, GLParam* p){
	c->pzoomx = p[1].f;
	c->pzoomy = p[2].f;
}
