/*
 * Memory allocator for TinyGL
 */
#include "zgl.h"

/* modify these functions so that they suit your needs */
#include<string.h>
void gl_free(void* p) { free(p); }

void* gl_malloc(GLint size) { return malloc(size); }

void* gl_zalloc(GLint size) { return calloc(1, size); }

void gl_memcpy(void* dest, void* src, GLuint size){
	memcpy(dest,src,size);
}
