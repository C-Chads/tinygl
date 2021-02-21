#include "msghandling.h"
#include "zgl.h"
#define TINYGL_VERSION 0.8
void glGetIntegerv(GLint pname, GLint* params) {
	GLContext* c = gl_get_context();
#include "error_check.h"
	switch (pname) {
	case GL_BLEND:
		*params = c->zb->enable_blend;
		break;
	case GL_BLEND_DST:
		*params = c->zb->dfactor;
		break;
	case GL_BLEND_SRC:
		*params = c->zb->sfactor;
		break;
	case GL_BLEND_EQUATION:
		*params = c->zb->blendeq;
		break;
	case GL_VIEWPORT:
		params[0] = c->viewport.xmin;
		params[1] = c->viewport.ymin;
		params[2] = c->viewport.xsize;
		params[3] = c->viewport.ysize;
		break;
	case GL_MAX_MODELVIEW_STACK_DEPTH:
		*params = MAX_MODELVIEW_STACK_DEPTH;
		break;
	case GL_MAX_PROJECTION_STACK_DEPTH:
		*params = MAX_PROJECTION_STACK_DEPTH;
		break;
	
	case GL_CULL_FACE_MODE:
		*params = c->current_cull_face;
		break;
	case GL_MAX_LIGHTS:
		*params = MAX_LIGHTS;
		break;
	case GL_MAX_TEXTURE_SIZE:
		*params = TGL_FEATURE_TEXTURE_DIM; /* not completely true, but... */
		break;
	case GL_CULL_FACE:
		*params = c->cull_face_enabled;
		break;
	case GL_MAX_TEXTURE_STACK_DEPTH:
		*params = MAX_TEXTURE_STACK_DEPTH;
		break;
	case GL_CURRENT_RASTER_POSITION_VALID:
		*params = c->rasterposvalid;
		break;
	default:
		tgl_warning("glGet: option not implemented");
		break;
	}
}
#define xstr(s) str(s)
#define str(s) #s

/*
const GLubyte* license_string = (const GLubyte*)""
"Copyright notice:\n"
"\n"
" (C) 1997-2021 Fabrice Bellard, Gek (DMHSW), C-Chads\n"
"\n"
" This software is provided 'as-is', without any express or implied\n"
"  warranty.  In no event will the authors be held liable for any damages\n"
"   arising from the use of this software.\n"
"\n"
" Permission is granted to anyone to use this software for any purpose,\n"
" including commercial applications, and to alter it and redistribute it\n"
" freely, subject to the following restrictions:\n"
"\n"
" 1. The origin of this software must not be misrepresented; you must not\n"
"    claim that you wrote the original software. If you use this software\n"
"    in a product, an acknowledgment in the product and its documentation \n"
"    *is* required.\n"
" 2. Altered source versions must be plainly marked as such, and must not be\n"
"    misrepresented as being the original software.\n"
" 3. This notice may not be removed or altered from any source distribution.\n"
"\n"
"If you redistribute modified sources, I would appreciate that you\n"
"include in the files history information documenting your changes.";
*/

const GLubyte* vendor_string = (const GLubyte*)"Fabrice Bellard, Gek, and the C-Chads";
const GLubyte* renderer_string = (const GLubyte*)"TinyGL";
const GLubyte* version_string = (const GLubyte*)"" 
xstr(TINYGL_VERSION)
" TinyGLv"
xstr(TINYGL_VERSION)
" "
#ifdef __GNUC__
"Compiled using GCC "
__VERSION__
#endif
#ifdef __TINYC__
"Compiled using Tiny C Compiler "
xstr(__TINYC__)
#endif
#ifdef _MSVC_VER
"Compiled using the worst compiler on earth, M$VC. Fuck you!"
#endif
#ifdef __clang__
"Compiled Using Clang "
__clang_version__
#endif
;
const GLubyte* extensions_string = (const GLubyte*)"TGL_TEXTURE "
"TGL_SMOOTHSHADING "
"TGL_LIGHTING "
#if TGL_FEATURE_ERROR_CHECK == 1
"TGL_FEATURE_ERROR_CHECK "
#endif 
#if TGL_FEATURE_STRICT_OOM_CHECKS == 1
"TGL_FEATURE_STRICT_OOM_CHECKS "
#endif
#if TGL_FEATURE_FISR == 1
"TGL_FEATURE_FISR "
#endif 
#if TGL_FEATURE_ARRAYS == 1
"TGL_FEATURE_ARRAYS "
#endif
#if TGL_FEATURE_DISPLAYLISTS == 1
"TGL_FEATURE_DISPLAYLISTS "
#endif
#if ZB_POINT_T_FRAC_BITS == (ZB_POINT_S_FRAC_BITS + TGL_FEATURE_TEXTURE_POW2)
"TGL_FEATURE_OPTIMIZED_TEXTURE_ACCESS "
#endif
"TGL_FEATURE_TEXTURE_POW2=" xstr(TGL_FEATURE_TEXTURE_POW2) " "
#if TGL_FEATURE_LIT_TEXTURES == 1
"TGL_FEATURE_LIT_TEXTURES "
#endif

#if TGL_FEATURE_POLYGON_OFFSET == 1
"TGL_FEATURE_POLYGON_OFFSET "
#endif

#if TGL_FEATURE_POLYGON_STIPPLE == 1
"TGL_FEATURE_POLYGON_STIPPLE "
#endif


#if TGL_FEATURE_NO_COPY_COLOR == 1
"TGL_FEATURE_NO_COPY_COLOR "
#endif


#if TGL_FEATURE_BLEND == 1
"TGL_FEATURE_BLEND "
#endif

#if TGL_FEATURE_NO_DRAW_COLOR == 1
"TGL_FEATURE_NO_DRAW_COLOR "
#endif

#if TGL_FEATURE_FORCE_CLEAR_NO_COPY_COLOR == 1
"TGL_FEATURE_FORCE_CLEAR_NO_COPY_COLOR "
#endif

#if TGL_FEATURE_16_BITS == 1
"TGL_FEATURE_16_BITS "
#endif

#if TGL_FEATURE_32_BITS == 1
"TGL_FEATURE_32_BITS "
#endif
#if COMPILETIME_TINYGL_COMPAT_TEST == 1
"TGL_COMPILETIME_TINYGL_COMPAT_TEST "
#endif
#if TGL_FEATURE_TINYGL_RUNTIME_COMPAT_TEST == 1
"TGL_FEATURE_TINYGL_RUNTIME_COMPAT_TEST "
#endif
"TGL_BUFFER_EXT "
"TGL_SOFTWARE_ACCELERATED";
const GLubyte* glGetString(GLenum name){
	switch(name){
		case GL_VENDOR: return vendor_string;
		case GL_RENDERER: return renderer_string;
		case GL_VERSION: return version_string;
		case GL_EXTENSIONS: return extensions_string;
		/*case GL_LICENSE: return license_string;*/
	}
	return (const GLubyte*)"Erroneous input to glGetString";
}

void glGetFloatv(GLint pname, GLfloat* v) {
	GLint i;
	GLint mnr = 0; /* just a trick to return the correct matrix */
	GLContext* c = gl_get_context();
#include "error_check.h"
	switch (pname) {
		case GL_TEXTURE_MATRIX:
			mnr++;
		case GL_PROJECTION_MATRIX:
			mnr++;
		case GL_MODELVIEW_MATRIX: {
			GLfloat* p = &c->matrix_stack_ptr[mnr]->m[0][0];
			;
			for (i = 0; i < 4; i++) {
				*v++ = p[0];
				*v++ = p[4];
				*v++ = p[8];
				*v++ = p[12];
				p++;
			}
		} break;
		case GL_LINE_WIDTH:
			*v = 1.0f;
			break;
		case GL_LINE_WIDTH_RANGE:
			v[0] = v[1] = 1.0f;
			break;
		case GL_POINT_SIZE:
			*v = c->zb->pointsize;
			break;
		case GL_ZOOM_X:
			*v = c->pzoomx;
			break;
		case GL_ZOOM_Y:
			*v = c->pzoomy;
			break;
		case GL_POINT_SIZE_RANGE:
			v[0] = v[1] = 1.0f;
		default:
			tgl_warning("warning: unknown pname in glGetFloatv()\n");
			break;
	}
}
