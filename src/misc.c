#include "msghandling.h"
#include "zgl.h"

void glPolygonStipple(void* a) {
#if TGL_FEATURE_POLYGON_STIPPLE == 1
	GLContext* c = gl_get_context();
#include "error_check.h"
	ZBuffer* zb = c->zb;

	memcpy(zb->stipplepattern, a, TGL_POLYGON_STIPPLE_BYTES);
	for (GLint i = 0; i < TGL_POLYGON_STIPPLE_BYTES; i++) {
		zb->stipplepattern[i] = ((GLubyte*)a)[i];
	}
#endif
}

void glopViewport(GLParam* p) {
	GLContext* c = gl_get_context();
	GLint xsize, ysize, xmin, ymin, xsize_req, ysize_req;

	xmin = p[1].i;
	ymin = p[2].i;
	xsize = p[3].i;
	ysize = p[4].i;

	/* we may need to resize the zbuffer */

	if (c->viewport.xmin != xmin || c->viewport.ymin != ymin || c->viewport.xsize != xsize || c->viewport.ysize != ysize) {

		xsize_req = xmin + xsize;
		ysize_req = ymin + ysize;

		if (c->gl_resize_viewport && c->gl_resize_viewport(&xsize_req, &ysize_req) != 0) {
			gl_fatal_error("glViewport: error while resizing display");
		}
		if (xsize <= 0 || ysize <= 0) {
			gl_fatal_error("glViewport: size too small");
		}

		
		c->viewport.xmin = xmin;
		c->viewport.ymin = ymin;
		c->viewport.xsize = xsize;
		c->viewport.ysize = ysize;

		
		gl_eval_viewport();
		
	}
}
void glBlendFunc(GLenum sfactor, GLenum dfactor) {
	GLParam p[3];
#include "error_check_no_context.h"
	p[0].op = OP_BlendFunc;
	p[1].i = sfactor;
	p[2].i = dfactor;
	gl_add_op(p);
	return;
}
void glopBlendFunc(GLParam* p) {
	GLContext* c = gl_get_context();
	c->zb->sfactor = p[1].i;
	c->zb->dfactor = p[2].i;
}

void glBlendEquation(GLenum mode) {
	GLParam p[2];
#include "error_check_no_context.h"
	p[0].op = OP_BlendEquation;
	p[1].i = mode;
	gl_add_op(p);
}
void glopBlendEquation(GLParam* p) {
	GLContext* c = gl_get_context();
	c->zb->blendeq = p[1].i;
}

void glopPointSize(GLParam* p) {
	GLContext* c = gl_get_context();
	c->zb->pointsize = p[1].f;
}
void glPointSize(GLfloat f) {
	GLParam p[2];
	p[0].op = OP_PointSize;
#include "error_check_no_context.h"
	p[1].f = f;
	gl_add_op(p);
}

void glopEnableDisable(GLParam* p) {
	GLContext* c = gl_get_context();
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
			gl_enable_disable_light(code - GL_LIGHT0, v);
		} else {
			tgl_warning("glEnableDisable: 0x%X not supported.\n", code);
		}
		break;
	}
}

void glopShadeModel(GLParam* p) {
	GLContext* c = gl_get_context();
	GLint code = p[1].i;
	c->current_shade_model = code;
}

void glopCullFace(GLParam* p) {
	GLContext* c = gl_get_context();
	GLint code = p[1].i;
	c->current_cull_face = code;
}

void glopFrontFace(GLParam* p) {
	GLContext* c = gl_get_context();
	GLint code = p[1].i;
	c->current_front_face = code;
}

void glopPolygonMode(GLParam* p) {
	GLContext* c = gl_get_context();
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
		break;
	}
}

void glopPolygonOffset(GLParam* p) {
	GLContext* c = gl_get_context();
	c->offset_factor = p[1].f;
	c->offset_units = p[2].f;
}

GLenum glGetError() {
#if TGL_FEATURE_ERROR_CHECK == 1
	GLContext* c = gl_get_context();
	GLenum eflag = c->error_flag;
	if (eflag != GL_OUT_OF_MEMORY) 
		c->error_flag = GL_NO_ERROR;
	return eflag;
#else
	return GL_NO_ERROR;
#endif
}

void glDrawBuffer(GLenum mode) {
	GLContext* c = gl_get_context();
#include "error_check.h"
	if ((mode != GL_FRONT && mode != GL_NONE) || c->in_begin) {
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_INVALID_OPERATION
#include "error_check.h"
#else
		return;
#endif
	}
	c->drawbuffer = mode;
}

void glReadBuffer(GLenum mode) {
	GLContext* c = gl_get_context();
#include "error_check.h"
	if ((mode != GL_FRONT && mode != GL_NONE) || c->in_begin) {
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_INVALID_OPERATION
#include "error_check.h"
#else
		return;
#endif
	}
	c->readbuffer = mode;
}


void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* data) {
	GLContext* c = gl_get_context();
#include "error_check.h"
	if (c->readbuffer != GL_FRONT || (format != GL_RGBA && format != GL_RGB && format != GL_DEPTH_COMPONENT) ||
#if TGL_FEATURE_RENDER_BITS == 32
		(type != GL_UNSIGNED_INT && type != GL_UNSIGNED_INT_8_8_8_8)
#elif TGL_FEATURE_RENDER_BITS == 16
		(type != GL_UNSIGNED_SHORT && type != GL_UNSIGNED_SHORT_5_6_5)
#else
#error "Unsupported TGL_FEATURE_RENDER_BITS"
#endif

	) {
#if TGL_FEATURE_ERROR_CHECK
#define ERROR_FLAG GL_INVALID_OPERATION
#include "error_check.h"
#else
		return;
#endif
	}
	/* TODO: implement read pixels.*/
}

void glFinish() { return; }
