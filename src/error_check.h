//Error check #included into every command which references the current context as "c"
#ifndef RETVAL
#define RETVAL /* a comment*/
#endif


#if TGL_FEATURE_ERROR_CHECK == 1
//LEVEL 1 ERROR_CHECK
//#error should never execute.
#ifndef ERROR_FLAG
//LEVEL 2 ERROR_FLAG

//BEGIN LEVEL 3
#if TGL_FEATURE_STRICT_OOM_CHECKS == 1
	if(c->error_flag == GL_OUT_OF_MEMORY)
		return RETVAL;
#else
	/* a comment */
#endif
//EOF LEVEL 3

#elif ERROR_FLAG != GL_OUT_OF_MEMORY
//LEVEL 2 ERROR_FLAG
//Deal with the smalltime error case.
	{c->error_flag = ERROR_FLAG; return RETVAL;}
//OUT OF MEMORY ELSE
#else
//LEVEL 2 ERROR_FLAG
//Deal with the out-of-memory case
	{c->error_flag = GL_OUT_OF_MEMORY; return RETVAL;}
//^ end of TGL_FEATURE_ERROR_CHECK check, 
#endif
//LEVEL 1 ERROR_CHECK
#endif
//^End of TGL_FEATURE_ERROR_CHECK, level 0

//Cleanup
#undef RETVAL
#undef ERROR_FLAG
