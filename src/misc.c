#include "msghandling.h"
#include "zgl.h"

void glPolygonStipple(void* a) {
#if TGL_FEATURE_POLYGON_STIPPLE == 1
	GLContext* c = gl_get_context();
	ZBuffer* zb = c->zb;

	gl_memcpy(zb->stipplepattern, a, TGL_POLYGON_STIPPLE_BYTES);
	for (GLint i = 0; i < TGL_POLYGON_STIPPLE_BYTES; i++) {
		zb->stipplepattern[i] = ((GLubyte*)a)[i];
	}
#endif
}

void glopViewport(GLContext* c, GLParam* p) {
	GLint xsize, ysize, xmin, ymin, xsize_req, ysize_req;

	xmin = p[1].i;
	ymin = p[2].i;
	xsize = p[3].i;
	ysize = p[4].i;

	/* we may need to resize the zbuffer */

	if (c->viewport.xmin != xmin || c->viewport.ymin != ymin || c->viewport.xsize != xsize || c->viewport.ysize != ysize) {

		xsize_req = xmin + xsize;
		ysize_req = ymin + ysize;

		if (c->gl_resize_viewport && c->gl_resize_viewport(c, &xsize_req, &ysize_req) != 0) {
			gl_fatal_error("glViewport: error while resizing display");
		}

		xsize = xsize_req - xmin;
		ysize = ysize_req - ymin;
		if (xsize <= 0 || ysize <= 0) {
			gl_fatal_error("glViewport: size too small");
		}

		tgl_trace("glViewport: %d %d %d %d\n", xmin, ymin, xsize, ysize);
		c->viewport.xmin = xmin;
		c->viewport.ymin = ymin;
		c->viewport.xsize = xsize;
		c->viewport.ysize = ysize;

		c->viewport.updated = 1;
	}
}
void glBlendFunc(GLenum sfactor, GLenum dfactor) {
	GLParam p[3];
	p[0].op = OP_BlendFunc;
	p[1].i = sfactor;
	p[2].i = dfactor;
	gl_add_op(p);
	return;
}
void glopBlendFunc(GLContext* c, GLParam* p) {
	c->zb->sfactor = p[1].i;
	c->zb->dfactor = p[2].i;
}


void glBlendEquation(GLenum mode) {
	GLParam p[2];
	p[0].op = OP_BlendEquation;
	p[1].i = mode;
	gl_add_op(p);
}
void glopBlendEquation(GLContext* c, GLParam* p) { c->zb->blendeq = p[1].i; }

void glopPointSize(GLContext* c, GLParam* p){
	c->zb->pointsize = p[1].f;
}
void glPointSize(GLfloat f){
	GLParam p[2]; p[0].op = OP_PointSize;
	p[1].f = f;
	gl_add_op(p);
}

void glopEnableDisable(GLContext* c, GLParam* p) {
	GLint code = p[1].i;
	GLint v = p[2].i;

	switch (code) {
	case GL_CULL_FACE:
		c->cull_face_enabled = v;
		break;
	case GL_LIGHTING:
		c->lighting_enabled = v;
		break;
	case GL_COLOR_MATERIAL:
		c->color_material_enabled = v;
		break;
	case GL_TEXTURE_2D:
		c->texture_2d_enabled = v;
		break;
	case GL_BLEND:
		c->zb->enable_blend = v;
		break;
	case GL_NORMALIZE:
		c->normalize_enabled = v;
		break;
	case GL_DEPTH_TEST:
		c->zb->depth_test = v;
		break;
	case GL_POLYGON_OFFSET_FILL:
		if (v)
			c->offset_states |= TGL_OFFSET_FILL;
		else
			c->offset_states &= ~TGL_OFFSET_FILL;
		break;
	case GL_POLYGON_STIPPLE:
#if TGL_FEATURE_POLYGON_STIPPLE == 1
		c->zb->dostipple = v;
#endif
		break;
	case GL_POLYGON_OFFSET_POINT:
		if (v)
			c->offset_states |= TGL_OFFSET_POINT;
		else
			c->offset_states &= ~TGL_OFFSET_POINT;
		break;
	case GL_POLYGON_OFFSET_LINE:
		if (v)
			c->offset_states |= TGL_OFFSET_LINE;
		else
			c->offset_states &= ~TGL_OFFSET_LINE;
		break;
	default:
		if (code >= GL_LIGHT0 && code < GL_LIGHT0 + MAX_LIGHTS) {
			gl_enable_disable_light(c, code - GL_LIGHT0, v);
		} else {
			/*
			fpr_ntf(stderr,"glEnableDisable: 0x%X not supported.\n",code);
			*/
		}
		break;
	}
}

void glopShadeModel(GLContext* c, GLParam* p) {
	GLint code = p[1].i;
	c->current_shade_model = code;
}

void glopCullFace(GLContext* c, GLParam* p) {
	GLint code = p[1].i;
	c->current_cull_face = code;
}

void glopFrontFace(GLContext* c, GLParam* p) {
	GLint code = p[1].i;
	c->current_front_face = code;
}

void glopPolygonMode(GLContext* c, GLParam* p) {
	GLint face = p[1].i;
	GLint mode = p[2].i;

	switch (face) {
	case GL_BACK:
		c->polygon_mode_back = mode;
		break;
	case GL_FRONT:
		c->polygon_mode_front = mode;
		break;
	case GL_FRONT_AND_BACK:
		c->polygon_mode_front = mode;
		c->polygon_mode_back = mode;
		break;
	default:
		assert(0);
	}
}

void glopHint(GLContext* c, GLParam* p) {
#if 0
  GLint target=p[1].i;
  GLint mode=p[2].i;

  /* do nothing */
#endif
}

void glopPolygonOffset(GLContext* c, GLParam* p) {
	c->offset_factor = p[1].f;
	c->offset_units = p[2].f;
}
