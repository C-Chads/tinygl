#ifndef RETVAL
#define RETVAL /* a comment*/
#endif

#if TGL_FEATURE_ERROR_CHECK == 1


#if TGL_FEATURE_STRICT_OOM_CHECKS == 1
GLContext* c = gl_get_context();
if (c->error_flag == GL_OUT_OF_MEMORY)
	return RETVAL;
#elif defined(NEED_CONTEXT)
GLContext* c = gl_get_context();
#endif

#endif

#undef RETVAL
#undef NEED_CONTEXT
