/*
 * Memory allocator for TinyGL
 */

static inline void required_for_compilation_(){
	return;
}

#if TGL_FEATURE_CUSTOM_MALLOC == 1
#include "zgl.h"

/* modify these functions so that they suit your needs */

#include <string.h>
void gl_free(void* p) { free(p); }

void* gl_malloc(GLint size) { return malloc(size); }

void* gl_zalloc(GLint size) { return calloc(1, size); }
#endif
