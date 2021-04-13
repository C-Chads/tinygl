#include "zgl.h"

GLint glRenderMode(GLint mode) {
	GLContext* c = gl_get_context();
	GLint result = 0;
#if TGL_FEATURE_ALT_RENDERMODES == 1
	switch (c->render_mode) {
	case GL_RENDER:
		break;
	case GL_SELECT:
		if (c->select_overflow) {
			result = -c->select_hits;
		} else {
			result = c->select_hits;
		}
		c->select_overflow = 0;
		c->select_ptr = c->select_buffer;
		c->name_stack_size = 0;
		break;
	case GL_FEEDBACK:
		if (c->feedback_overflow)
			result = -c->feedback_hits;
		else
			result = c->feedback_hits;
		c->feedback_overflow = 0;
		c->feedback_hits = 0;
		c->feedback_ptr = c->feedback_buffer;
		break;
	default:
		gl_fatal_error("GLContext's Rendermode was somehow erroneously set.");
	}
	switch (mode) {
	case GL_RENDER:
		c->render_mode = GL_RENDER;
		break;
	case GL_SELECT:

#if TGL_FEATURE_ERROR_CHECK == 1
		if (c->select_buffer == NULL)
#define ERROR_FLAG GL_INVALID_OPERATION
#define RETVAL 0
#include "error_check.h"
#else
		if (c->select_buffer == NULL)
			return 0;
#endif
			c->render_mode = GL_SELECT;
		c->select_ptr = c->select_buffer;
		c->select_hits = 0;
		c->select_overflow = 0;
		c->select_hit = NULL;
		break;
	case GL_FEEDBACK:
#if TGL_FEATURE_ERROR_CHECK == 1
		if (c->feedback_buffer == NULL)
#define ERROR_FLAG GL_INVALID_OPERATION
#define RETVAL 0
#include "error_check.h"
#else
		if (c->feedback_buffer == NULL)
			return 0;
#endif
			c->render_mode = GL_FEEDBACK;
		c->feedback_hits = 0;
		c->feedback_ptr = c->feedback_buffer;
		c->feedback_overflow = 0;
		break;
	default:
#if TGL_FEATURE_ERROR_CHECK == 1
#define RETVAL 0
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"

#else
		return 0;
#endif
	}
	return result;
#else
	
	return 0;
#endif
}

void glSelectBuffer(GLint size, GLuint* buf) {

#if TGL_FEATURE_ALT_RENDERMODES == 1
	GLContext* c = gl_get_context();
#if TGL_FEATURE_ERROR_CHECK == 1
	if (c->render_mode == GL_SELECT)
#define ERROR_FLAG GL_INVALID_OPERATION
#include "error_check.h"
#else
	if (c->render_mode == GL_SELECT)
		return;
#endif

		c->select_buffer = buf;
	c->select_size = size;
#else
	return;
#endif
}

void glFeedbackBuffer(GLint size, GLenum type, GLfloat* buf) {
#if TGL_FEATURE_ALT_RENDERMODES == 1
	GLContext* c = gl_get_context();
#if TGL_FEATURE_ERROR_CHECK == 1
	if (c->render_mode == GL_FEEDBACK || !(type == GL_2D || type == GL_3D || type == GL_3D_COLOR || type == GL_3D_COLOR_TEXTURE || type == GL_4D_COLOR_TEXTURE))
#define ERROR_FLAG GL_INVALID_OPERATION
#include "error_check.h"
#else
	if (c->render_mode == GL_FEEDBACK || !(type == GL_2D || type == GL_3D || type == GL_3D_COLOR || type == GL_3D_COLOR_TEXTURE || type == GL_4D_COLOR_TEXTURE))
		return;
#endif
		c->feedback_buffer = buf;
	c->feedback_size = size;
	c->feedback_type = type;
#else
	return;
#endif
}

void gl_add_feedback(GLfloat token, GLVertex* v1, GLVertex* v2, GLVertex* v3, GLfloat passthrough_token_value) {
#if TGL_FEATURE_ALT_RENDERMODES == 1
	GLContext* c = gl_get_context();
	if (c->feedback_overflow)
		return;
	GLuint feedback_hits_needed = 2;
	GLuint vertex_feedback_hits_needed = 0;
	GLuint vertex_pos_hits_needed = 2;
	GLuint vertex_color_hits_needed = 4;
	GLuint vertex_texture_hits_needed = 4;
#if TGL_FEATURE_ERROR_CHECK == 1
	GLuint done = 0;
#endif
	switch (c->feedback_type) {
	case GL_2D:
		vertex_feedback_hits_needed = 2;
		vertex_pos_hits_needed = 2;
		vertex_color_hits_needed = 0;
		vertex_texture_hits_needed = 0;
		break;
	case GL_3D:
		vertex_feedback_hits_needed = 3;
		vertex_pos_hits_needed = 3;
		vertex_color_hits_needed = 0;
		vertex_texture_hits_needed = 0;
		break;
	case GL_3D_COLOR:
		vertex_feedback_hits_needed = 3 + 4;
		vertex_pos_hits_needed = 3;
		vertex_color_hits_needed = 4;
		vertex_texture_hits_needed = 0;
		break;
	case GL_3D_COLOR_TEXTURE:
		vertex_feedback_hits_needed = 3 + 4 + 4;
		vertex_pos_hits_needed = 3;
		vertex_color_hits_needed = 4;
		vertex_texture_hits_needed = 4;
		break;
	case GL_4D_COLOR_TEXTURE:
		vertex_feedback_hits_needed = 4 + 4 + 4;
		vertex_pos_hits_needed = 4;
		vertex_color_hits_needed = 4;
		vertex_texture_hits_needed = 4;
		break;
	default:
		vertex_feedback_hits_needed = 0;
		vertex_pos_hits_needed = 0;
		vertex_color_hits_needed = 0;
		vertex_texture_hits_needed = 0;
		break;
	}
	if (token == GL_PASS_THROUGH_TOKEN)
		feedback_hits_needed = 1 + 1; 
	else if (token == GL_POINT_TOKEN)
		feedback_hits_needed = 1 + 1 * vertex_feedback_hits_needed; 
	else if (token == GL_LINE_TOKEN || token == GL_LINE_RESET_TOKEN)
		feedback_hits_needed = 1 + 2 * vertex_feedback_hits_needed; 
	else if (token == GL_POLYGON_TOKEN)
		feedback_hits_needed = 1 + 1 + 3 * vertex_feedback_hits_needed; 
	else if (token == GL_BITMAP_TOKEN)
		feedback_hits_needed = 1 + 1 * vertex_feedback_hits_needed; 
	else if (token == GL_DRAW_PIXEL_TOKEN || token == GL_COPY_PIXEL_TOKEN)
		feedback_hits_needed = 1 + 1 * vertex_feedback_hits_needed; 
	else
		return;
	c->feedback_hits += feedback_hits_needed;
	if (c->feedback_hits > (GLint)c->feedback_size) {
		c->feedback_overflow = 1;
		c->feedback_hits -= feedback_hits_needed;
		return;
	}
	
#if TGL_FEATURE_ERROR_CHECK == 1
#define DONE_ERROR_CHECK                                                                                                                                       \
	{                                                                                                                                                          \
		if (++done > feedback_hits_needed) {                                                                                                                   \
			gl_fatal_error("\nBAD FEEDBACK BUFFER WRITE DETECTED\n");                                                                                          \
		}                                                                                                                                                      \
	}
#else
#define DONE_ERROR_CHECK /* a comment*/
#endif
#define WRITE_FLOAT(f)                                                                                                                                         \
	{                                                                                                                                                          \
		DONE_ERROR_CHECK;                                                                                                                                      \
		*(c->feedback_ptr++) = f;                                                                                                                              \
	}
#define WRITE_UINT(u)                                                                                                                                          \
	{                                                                                                                                                          \
		DONE_ERROR_CHECK;                                                                                                                                      \
		*(((GLuint*)c->feedback_ptr++)) = u;                                                                                                                   \
	}
#define WRITE_VERTEX(v)                                                                                                                                        \
	{                                                                                                                                                          \
		if (vertex_pos_hits_needed == 2) {                                                                                                                     \
			WRITE_FLOAT(v->zp.x)                                                                                                                               \
			WRITE_FLOAT(v->zp.y)                                                                                                                               \
		}                                                                                                                                                      \
		if (vertex_pos_hits_needed == 3) {                                                                                                                     \
			WRITE_FLOAT(v->zp.x)                                                                                                                               \
			WRITE_FLOAT(v->zp.y)                                                                                                                               \
			WRITE_FLOAT(v->zp.z)                                                                                                                               \
		}                                                                                                                                                      \
		if (vertex_pos_hits_needed == 4) {                                                                                                                     \
			WRITE_FLOAT(v->zp.x)                                                                                                                               \
			WRITE_FLOAT(v->zp.y)                                                                                                                               \
			WRITE_FLOAT(v->zp.z)                                                                                                                               \
			WRITE_FLOAT(v->pc.W)                                                                                                                               \
		}                                                                                                                                                      \
		if (vertex_color_hits_needed == 4) {                                                                                                                   \
			WRITE_FLOAT(v->color.X)                                                                                                                            \
			WRITE_FLOAT(v->color.Y)                                                                                                                            \
			WRITE_FLOAT(v->color.Z)                                                                                                                            \
			WRITE_FLOAT(v->color.W)                                                                                                                            \
		}                                                                                                                                                      \
		if (vertex_texture_hits_needed == 4) {                                                                                                                 \
			WRITE_FLOAT(v->tex_coord.X)                                                                                                                        \
			WRITE_FLOAT(v->tex_coord.Y)                                                                                                                        \
			WRITE_FLOAT(v->tex_coord.Z)                                                                                                                        \
			WRITE_FLOAT(v->tex_coord.W)                                                                                                                        \
		}                                                                                                                                                      \
	}
	if (token == GL_PASS_THROUGH_TOKEN) {
		WRITE_UINT(GL_PASS_THROUGH_TOKEN);
		WRITE_FLOAT(passthrough_token_value);
	} else if (token == GL_LINE_TOKEN || token == GL_LINE_RESET_TOKEN) {
		WRITE_UINT(token);
		WRITE_VERTEX(v1);
		WRITE_VERTEX(v2);
	} else if (token == GL_POLYGON_TOKEN) {
		WRITE_UINT(GL_POLYGON_TOKEN);
		WRITE_UINT(3);
		WRITE_VERTEX(v1);
		WRITE_VERTEX(v2);
		WRITE_VERTEX(v3);
	} else if (token == GL_BITMAP_TOKEN) {
		WRITE_UINT(GL_BITMAP_TOKEN);
		WRITE_VERTEX(v1);
	} else if (token == GL_DRAW_PIXEL_TOKEN || token == GL_COPY_PIXEL_TOKEN) {
		WRITE_UINT(token);
		WRITE_VERTEX(v1);
	}
#if TGL_FEATURE_ERROR_CHECK == 1
	if (done != feedback_hits_needed)
		gl_fatal_error("Failed to write enough information to the buffer.");
#endif
/* End of gl_add_feedback*/
#endif
	return;
}
void glPassThrough(GLfloat token) {

#include "error_check_no_context.h"
	gl_add_feedback(GL_PASS_THROUGH_TOKEN, NULL, NULL, NULL, token);
}
void glopInitNames(GLParam* p) {
#if TGL_FEATURE_ALT_RENDERMODES == 1
	GLContext* c = gl_get_context();
	if (c->render_mode == GL_SELECT) {
		c->name_stack_size = 0;
		c->select_hit = NULL;
	}
#endif
}

void glopPushName(GLParam* p) {
#if TGL_FEATURE_ALT_RENDERMODES == 1
	GLContext* c = gl_get_context();
	if (c->render_mode == GL_SELECT) {
		
		c->name_stack[c->name_stack_size++] = p[1].i;
		c->select_hit = NULL;
	}
#endif
}

void glopPopName(GLParam* p) {
#if TGL_FEATURE_ALT_RENDERMODES == 1
	GLContext* c = gl_get_context();
	if (c->render_mode == GL_SELECT) {
		
		c->name_stack_size--;
		c->select_hit = NULL;
	}
#endif
}

void glopLoadName(GLParam* p) {
#if TGL_FEATURE_ALT_RENDERMODES == 1
	GLContext* c = gl_get_context();
	if (c->render_mode == GL_SELECT) {
		
		c->name_stack[c->name_stack_size - 1] = p[1].i;
		c->select_hit = NULL;
	}
#endif
}

void gl_add_select(GLuint zmin, GLuint zmax) {

#if TGL_FEATURE_ALT_RENDERMODES == 1
	GLContext* c = gl_get_context();
	GLuint* ptr;
	GLint n, i;
	if (!c->select_overflow) {
		if (c->select_hit == NULL) {
			n = c->name_stack_size;
			if ((c->select_ptr - c->select_buffer + 3 + n) > c->select_size) { 
				c->select_overflow = 1;
			} else {
				ptr = c->select_ptr;
				c->select_hit = ptr;
				*ptr++ = c->name_stack_size;
				*ptr++ = zmin;
				*ptr++ = zmax;
				for (i = 0; i < n; i++)
					*ptr++ = c->name_stack[i];
				c->select_ptr = ptr;
				c->select_hits++;
			}
		} else {
			if (zmin < c->select_hit[1])
				c->select_hit[1] = zmin;
			if (zmax > c->select_hit[2])
				c->select_hit[2] = zmax;
		}
	}
#else
	return;
#endif
}
