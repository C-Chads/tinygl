
#ifndef RETVAL
#define RETVAL /* a comment*/
#endif

#if TGL_FEATURE_ERROR_CHECK == 1



#ifndef ERROR_FLAG



#if TGL_FEATURE_STRICT_OOM_CHECKS == 1
if (c->error_flag == GL_OUT_OF_MEMORY)
	return RETVAL;
#endif


#elif ERROR_FLAG != GL_OUT_OF_MEMORY
{
	c->error_flag = ERROR_FLAG;
	return RETVAL;
}

#else
{
	c->error_flag = GL_OUT_OF_MEMORY;
	return RETVAL;
}
#endif



#endif



#undef RETVAL
#undef ERROR_FLAG
