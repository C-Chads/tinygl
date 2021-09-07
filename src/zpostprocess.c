#include "../include/GL/gl.h"
#include "../include/zbuffer.h"
#include "zgl.h"

void glPostProcess(GLuint (*postprocess)(GLint x, GLint y, GLuint pixel, GLushort z)) {
	GLint i, j;
	GLContext* c = gl_get_context();
#ifdef _OPENMP
#pragma omp parallel for collapse(2)
#endif
	for (j = 0; j < c->zb->ysize; j++)
		for (i = 0; i < c->zb->xsize; i++)
			c->zb->pbuf[i + j * (c->zb->xsize)] = postprocess(i, j, c->zb->pbuf[i + j * (c->zb->xsize)], c->zb->zbuf[i + j * (c->zb->xsize)]);
}
