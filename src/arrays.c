#include "zgl.h"

#include "msghandling.h"



static GLint free_buffer(GLint handle) {
	GLContext* c = gl_get_context();
	GLSharedState* s = &(c->shared_state);
	if (handle == 0 || handle > MAX_BUFFERS)
		return 1; 

	handle--;
	if (s->buffers[handle]) { 
		if (c->boundarraybuffer == (handle + 1))
			c->boundarraybuffer = 0;
		if (s->buffers[handle]->data) 
		{
			void* d = s->buffers[handle]->data;
			gl_free(s->buffers[handle]->data); 
			
			if (c->vertex_array == d) {
				c->vertex_array = NULL;
				c->client_states &= ~VERTEX_ARRAY;
			}
			if (c->color_array == d) {
				c->color_array = NULL;
				c->client_states &= ~COLOR_ARRAY;
			}
			if (c->normal_array == d) {
				c->normal_array = NULL;
				c->client_states &= ~NORMAL_ARRAY;
			}
			if (c->texcoord_array == d) {
				c->texcoord_array = NULL;
				c->client_states &= ~TEXCOORD_ARRAY;
			}
		}
		gl_free(s->buffers[handle]); 
		s->buffers[handle] = NULL;   
		return 0;
	} else {
		return 0;
	}
}
static GLint check_buffer(GLint handle) { 
	GLContext* c = gl_get_context();
	GLSharedState* s = &(c->shared_state);
	if (handle == 0 || handle > MAX_BUFFERS)
		return 2; 
	handle--;
	if (s->buffers[handle])
		return 1;
	return 0;
}
GLboolean glIsBuffer(GLuint buffer) {
	if (check_buffer(buffer) == 1)
		return GL_TRUE;
	return GL_FALSE;
}

static GLBuffer* get_buffer(GLint handle) {
	GLContext* c;
	GLSharedState* s;
	c = gl_get_context();
	s = &(c->shared_state);
	if (handle == 0 || handle > MAX_BUFFERS)
		return NULL;
	handle--;
	return s->buffers[handle];
}
static GLint create_buffer(GLint handle) {
	GLContext* c = gl_get_context();
	GLSharedState* s = &(c->shared_state);
	if (handle == 0 || handle > MAX_BUFFERS)
		return 1; 
	handle--;	 
	if (s->buffers[handle])
		free_buffer(handle + 1); 
	
	s->buffers[handle] = gl_zalloc(sizeof(GLBuffer));

	if (!(s->buffers[handle])) {
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_OUT_OF_MEMORY
#define RETVAL 1
#include "error_check.h"
#else
		gl_fatal_error("GL_OUT_OF_MEMORY");
#endif
	}
	s->buffers[handle]->data = NULL;
	s->buffers[handle]->size = 0;
	return 0;
}

void glGenBuffers(GLsizei n, GLuint* buffers) {
	GLint i;
	GLContext* c = gl_get_context();
#include "error_check.h"
	if (n > MAX_BUFFERS)
		goto error;

	{
		GLint n_left = n;
		GLuint names[MAX_BUFFERS];
		for (i = 1; i <= MAX_BUFFERS && n_left > 0; i++)
			if (!check_buffer(i))
				names[(n_left--) - 1] = i;

		if (n_left)
			goto error; 
		for (i = 0; i < n; i++) {
			create_buffer(names[i]);


#include "error_check.h"
			buffers[i] = names[i];
		}
	}
	return;
error:
	for (i = 0; i < n; i++)
		buffers[i] = 0;
	return;
}
void glDeleteBuffers(GLsizei n, const GLuint* buffers) {
	GLint i;
	GLContext* c = gl_get_context();
#include "error_check.h"
	for (i = 0; i < n; i++)
		free_buffer(buffers[i]);
}


void glBindBuffer(GLenum target, GLuint buffer) {
	GLContext* c = gl_get_context();
#include "error_check.h"
	if (buffer == 0 || check_buffer(buffer) == 1) {
		if (target == GL_ARRAY_BUFFER)
			c->boundarraybuffer = buffer;
	}
	return;
}

void glBindBufferAsArray(GLenum target, GLuint buffer,
						 GLenum type,	
						 GLint size,	 
						 GLint stride) { 
	GLContext* c = gl_get_context();
#include "error_check.h"
	if (target != GL_VERTEX_BUFFER && target != GL_NORMAL_BUFFER && target != GL_COLOR_BUFFER && target != GL_TEXTURE_COORD_BUFFER) {
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"
#else
		return;
#endif
	}
	if (buffer == 0)
		switch (target) {
		case GL_VERTEX_BUFFER:
			glDisableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(size, type, stride, NULL);
			c->boundvertexbuffer = buffer;
			return;
			break;
		case GL_NORMAL_BUFFER:
			glDisableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(type, stride, NULL);
			c->boundnormalbuffer = buffer;
			return;
			break;
		case GL_COLOR_BUFFER:
			glDisableClientState(GL_COLOR_ARRAY);
			glColorPointer(size, type, stride, NULL);
			c->boundcolorbuffer = buffer;
			return;
			break;
		case GL_TEXTURE_COORD_BUFFER:
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(size, type, stride, NULL);
			c->boundtexcoordbuffer = buffer;
			return;
			break;
		default:
			return;
		}
	if (check_buffer(buffer) != 1 || type != GL_FLOAT) {
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"
#else
		tgl_warning("\ncheck_buffer failed on buffer, or incorrect type\n");
		return;
#endif
	}
	GLBuffer* buf = c->shared_state.buffers[buffer - 1];
	if (!buf || (buf->data == NULL) || (buf->size == 0)) {
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_INVALID_OPERATION
#include "error_check.h"
#else
		tgl_warning("\nbuffer was null, buffer data was null, or buffer size was 0\n");
		return;
#endif
	}
	switch (target) {
	case GL_VERTEX_BUFFER:
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(size, type, stride, buf->data);
		c->boundvertexbuffer = buffer;
		break;
	case GL_NORMAL_BUFFER:
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(type, stride, buf->data);
		c->boundnormalbuffer = buffer;
		break;
	case GL_COLOR_BUFFER:
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(size, type, stride, buf->data);
		c->boundcolorbuffer = buffer;
		break;
	case GL_TEXTURE_COORD_BUFFER:
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(size, type, stride, buf->data);
		c->boundtexcoordbuffer = buffer;
		break;

	default:
		return;
	}
	return;
}

void* glMapBuffer(GLenum target, GLenum access) {
	GLContext* c = gl_get_context();
#define RETVAL NULL
#include "error_check.h"
	GLint handle = 0;
	if (target == GL_ARRAY_BUFFER)
		handle = c->boundarraybuffer;
	if (target == GL_VERTEX_BUFFER)
		handle = c->boundvertexbuffer;
	if (target == GL_TEXTURE_COORD_BUFFER)
		handle = c->boundtexcoordbuffer;
	if (target == GL_NORMAL_BUFFER)
		handle = c->boundnormalbuffer;
	if (target == GL_COLOR_BUFFER)
		handle = c->boundcolorbuffer;
	{
		if (check_buffer(handle) == 1)
			return c->shared_state.buffers[handle - 1]->data;
	}
#if TGL_FEATURE_ERROR_CHECK == 1
#define RETVAL NULL
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"
#else
	return NULL;
#endif
}
void glBufferData(GLenum target, GLsizei size, const void* data,
				  GLenum usage) 
{
	GLContext* c = gl_get_context();
#include "error_check.h"
	GLint handle = 0;
	GLBuffer* buf = NULL;
	if (target == GL_ARRAY_BUFFER)
		handle = c->boundarraybuffer;
	if (target == GL_VERTEX_BUFFER)
		handle = c->boundvertexbuffer;
	if (target == GL_TEXTURE_COORD_BUFFER)
		handle = c->boundtexcoordbuffer;
	if (target == GL_NORMAL_BUFFER)
		handle = c->boundnormalbuffer;
	if (target == GL_COLOR_BUFFER)
		handle = c->boundcolorbuffer;
	if (check_buffer(handle) == 1)
		buf = c->shared_state.buffers[handle - 1];
	else {
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"
#else
		return;
#endif
	}
	if (buf->data)
		gl_free(buf->data);
	buf->data = NULL;
	buf->size = 0;
	if (size == 0)
		return; 
	buf->data = gl_malloc(size);
	buf->size = size;
	if (!(buf->data)) {
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_OUT_OF_MEMORY
#include "error_check.h"
#else
		gl_fatal_error("GL_OUT_OF_MEMORY");
#endif
	}
	if (data != NULL)
		memcpy(buf->data, data, size);
}

void glopArrayElement(GLParam* param) {
	GLint i;
	GLContext* c = gl_get_context();
	GLint states = c->client_states;
	GLint idx = param[1].i;

	if (states & COLOR_ARRAY) {
		GLParam p[5];
		GLint size = c->color_array_size;
		i = idx * (size + c->color_array_stride);
		p[1].f = c->color_array[i];
		p[2].f = c->color_array[i + 1];
		p[3].f = c->color_array[i + 2];
		p[4].f = (size > 3) ? c->color_array[i + 3] : 1.0f;
		glopColor(p);
	}
	if (states & NORMAL_ARRAY) {
		i = idx * (3 + c->normal_array_stride);
		c->current_normal.X = c->normal_array[i];
		c->current_normal.Y = c->normal_array[i + 1];
		c->current_normal.Z = c->normal_array[i + 2];
		
	}
	if (states & TEXCOORD_ARRAY) {
		GLint size = c->texcoord_array_size;
		i = idx * (size + c->texcoord_array_stride);
		c->current_tex_coord.X = c->texcoord_array[i];
		c->current_tex_coord.Y = c->texcoord_array[i + 1];
		c->current_tex_coord.Z = (size > 2) ? c->texcoord_array[i + 2] : 0.0f;
		c->current_tex_coord.W = (size > 3) ? c->texcoord_array[i + 3] : 1.0f;
	}
	if (states & VERTEX_ARRAY) {
		GLParam p[5];
		GLint size = c->vertex_array_size;
		i = idx * (size + c->vertex_array_stride);
		p[1].f = c->vertex_array[i];
		p[2].f = c->vertex_array[i + 1];
		p[3].f = (size > 2) ? c->vertex_array[i + 2] : 0.0f;
		p[4].f = (size > 3) ? c->vertex_array[i + 3] : 1.0f;
		glopVertex(p);
	}
}

void glArrayElement(GLint i) {
	GLParam p[2];
#include "error_check_no_context.h"
	p[0].op = OP_ArrayElement;
	p[1].i = i;
	gl_add_op(p);
}

void glDrawArrays(GLenum mode, GLint first, GLsizei count) {
	GLint i;
	GLint end;
	
#include "error_check_no_context.h"
	end = first + count;
	glBegin(mode);
	for (i = first; i < end; i++)
		glArrayElement(i);
	glEnd();
}

void glopEnableClientState(GLParam* p) { gl_get_context()->client_states |= p[1].i; }

void glEnableClientState(GLenum array) {
	GLParam p[2];
#include "error_check_no_context.h"
	p[0].op = OP_EnableClientState;

	switch (array) {
	case GL_VERTEX_ARRAY:
		p[1].i = VERTEX_ARRAY;
		break;
	case GL_NORMAL_ARRAY:
		p[1].i = NORMAL_ARRAY;
		break;
	case GL_COLOR_ARRAY:
		p[1].i = COLOR_ARRAY;
		break;
	case GL_TEXTURE_COORD_ARRAY:
		p[1].i = TEXCOORD_ARRAY;
		break;
	default:
		break;
	}
	gl_add_op(p);
}

void glopDisableClientState(GLParam* p) {
	GLContext* c = gl_get_context();
	c->client_states &= p[1].i;
}

void glDisableClientState(GLenum array) {
	GLParam p[2];
#include "error_check_no_context.h"
	p[0].op = OP_DisableClientState;

	switch (array) {
	case GL_VERTEX_ARRAY:
		p[1].i = ~VERTEX_ARRAY;
		break;
	case GL_NORMAL_ARRAY:
		p[1].i = ~NORMAL_ARRAY;
		break;
	case GL_COLOR_ARRAY:
		p[1].i = ~COLOR_ARRAY;
		break;
	case GL_TEXTURE_COORD_ARRAY:
		p[1].i = ~TEXCOORD_ARRAY;
		break;
	default:
		break;
	}
	gl_add_op(p);
}

void glopVertexPointer(GLParam* p) {
	GLContext* c = gl_get_context();
	c->vertex_array_size = p[1].i;
	c->vertex_array_stride = p[2].i;
	c->vertex_array = p[3].p;
}

void glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) {
	GLParam p[4];
#define NEED_CONTEXT
#include "error_check_no_context.h"
#if TGL_FEATURE_ERROR_CHECK == 1
	if (type != GL_FLOAT)
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"
#else
	if (type != GL_FLOAT)
		return;
#endif
		p[0].op = OP_VertexPointer;
	p[1].i = size;
	p[2].i = stride;
	p[3].p = (void*)pointer;
	gl_add_op(p);
}

void glopColorPointer(GLParam* p) {
	GLContext* c = gl_get_context();
	c->color_array_size = p[1].i;
	c->color_array_stride = p[2].i;
	c->color_array = p[3].p;
}

void glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) {
	GLParam p[4];
#define NEED_CONTEXT
#include "error_check_no_context.h"
#if TGL_FEATURE_ERROR_CHECK == 1
	if (type != GL_FLOAT)
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"
#else
	/* assert(type == GL_FLOAT);*/
#endif
		p[0].op = OP_ColorPointer;
	p[1].i = size;
	p[2].i = stride;
	p[3].p = (void*)pointer;
	gl_add_op(p);
}

void glopNormalPointer(GLParam* p) {
	GLContext* c = gl_get_context();
	c->normal_array_stride = p[1].i;
	c->normal_array = p[2].p;
}

void glNormalPointer(GLenum type, GLsizei stride, const GLvoid* pointer) {
	GLParam p[3];
#define NEED_CONTEXT
#include "error_check_no_context.h"
#if TGL_FEATURE_ERROR_CHECK == 1
	if (type != GL_FLOAT)
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"
#else

#endif
		p[0].op = OP_NormalPointer;
	p[1].i = stride;
	p[2].p = (void*)pointer;
	gl_add_op(p);
}

void glopTexCoordPointer(GLParam* p) {
	GLContext* c = gl_get_context();
	c->texcoord_array_size = p[1].i;
	c->texcoord_array_stride = p[2].i;
	c->texcoord_array = p[3].p;
}

void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) {
	GLParam p[4];
#define NEED_CONTEXT
#include "error_check_no_context.h"
#if TGL_FEATURE_ERROR_CHECK == 1
	if (type != GL_FLOAT)
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"
#else

#endif
		p[0].op = OP_TexCoordPointer;
	p[1].i = size;
	p[2].i = stride;
	p[3].p = (void*)pointer;
	gl_add_op(p);
}
