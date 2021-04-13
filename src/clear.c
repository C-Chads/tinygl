#include "zgl.h"

void glopClearColor(GLParam* p) {
	GLContext* c = gl_get_context();
	c->clear_color.v[0] = p[1].f;
	c->clear_color.v[1] = p[2].f;
	c->clear_color.v[2] = p[3].f;
	c->clear_color.v[3] = p[4].f;
}
void glopClearDepth(GLParam* p) {
	GLContext* c = gl_get_context();
	c->clear_depth = p[1].f;
}

void glopClear(GLParam* p) {
	GLContext* c = gl_get_context();
	GLint mask = p[1].i;
	GLint z = 0;
	GLint r = (GLint)(c->clear_color.v[0] * COLOR_MULT_MASK);
	GLint g = (GLint)(c->clear_color.v[1] * COLOR_MULT_MASK);
	GLint b = (GLint)(c->clear_color.v[2] * COLOR_MULT_MASK);

	/* TODO : correct value of Z */

	ZB_clear(c->zb, mask & GL_DEPTH_BUFFER_BIT, z, mask & GL_COLOR_BUFFER_BIT, r, g, b);
}
