#include "msghandling.h"
#include "zgl.h"

#define xstr(s) str(s)
#define str(s) #s


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

const GLubyte* vendor_string = (const GLubyte*)"Fabrice Bellard, Gek, and the C-Chads";
const GLubyte* renderer_string = (const GLubyte*)"TinyGL";
const GLubyte* version_string = (const GLubyte*)"" xstr(TINYGL_VERSION) " TinyGLv" xstr(TINYGL_VERSION) " "
#ifdef __GNUC__
																										"Compiled using GCC " __VERSION__
#endif
#ifdef __TINYC__
																										"Compiled using Tiny C Compiler " xstr(__TINYC__)
#endif
#ifdef _MSVC_VER
																											"Compiled using the worst compiler on earth, M$VC. "
#endif
#ifdef __clang__
																											"Compiled Using Clang " __clang_version__
#endif
	;
const GLubyte* extensions_string =
	(const GLubyte*)"TGL_TEXTURE "
					"TGL_SMOOTHSHADING "
					"TGL_LIGHTING "
					"TGL_OPTIMIZATION_HINT_BRANCH_COST=" xstr(
						TGL_OPTIMIZATION_HINT_BRANCH_COST) " "
#if TGL_FEATURE_ERROR_CHECK == 1
														   "TGL_FEATURE_ERROR_CHECK "
#endif
#if TGL_FEATURE_STRICT_OOM_CHECKS == 1
														   "TGL_FEATURE_STRICT_OOM_CHECKS "
#endif
#if TGL_FEATURE_CUSTOM_MALLOC == 1
														   "TGL_FEATURE_CUSTOM_MALLOC "
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
														   "TGL_FEATURE_TEXTURE_POW2=" xstr(
															   TGL_FEATURE_TEXTURE_POW2) " "
#if TGL_FEATURE_LIT_TEXTURES == 1
																						 "TGL_FEATURE_LIT_TEXTURES "
#endif
#if TGL_FEATURE_SPECULAR_BUFFERS == 1
																						 "TGL_FEATURE_SPECULAR_BUFFERS "
#endif

#if TGL_FEATURE_POLYGON_OFFSET == 1
																						 "TGL_FEATURE_POLYGON_OFFSET "
#endif

#if TGL_FEATURE_POLYGON_STIPPLE == 1
																						 "TGL_FEATURE_POLYGON_STIPPLE "
#endif
#if TGL_FEATURE_GL_POLYGON == 1
																						 "TGL_FEATURE_GL_POLYGON "
#endif

#if TGL_FEATURE_BLEND == 1
																						 "TGL_FEATURE_BLEND "
#endif
#if TGL_FEATURE_BLEND_DRAW_PIXELS == 1
																						 "TGL_FEATURE_BLEND_DRAW_PIXELS "
#endif
#if TGL_FEATURE_NO_DRAW_COLOR == 1
																						 "TGL_FEATURE_NO_DRAW_COLOR "
#endif

#if TGL_FEATURE_NO_COPY_COLOR == 1
																						 "TGL_FEATURE_NO_COPY_COLOR "
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
#if defined(_OPENMP)
																						 "TGL_FEATURE_MULTI_THREADED "
																						 "TGL_FEATURE_MULTI_THREADED_POST_PROCESS "

#if TGL_FEATURE_MULTITHREADED_DRAWPIXELS == 1
																						 "TGL_FEATURE_MULTITHREADED_DRAWPIXELS "
#endif

#if TGL_FEATURE_MULTITHREADED_COPY_TEXIMAGE_2D == 1
																						 "TGL_FEATURE_MULTITHREADED_COPY_TEXIMAGE_2D "
#endif

#if TGL_FEATURE_MULTITHREADED_ZB_COPYBUFFER == 1
																						 "TGL_FEATURE_MULTITHREADED_ZB_COPYBUFFER "
#endif

#else
																						 "TGL_FEATURE_SINGLE_THREADED "
#endif

#if TGL_FEATURE_ALIGNAS
																						 "TGL_FEATURE_ALIGNAS "
#endif
																						 "TGL_BUFFER_EXT "
#if TGL_FEATURE_ALT_RENDERMODES
																						 "TGL_FEEDBACK "
																						 "TGL_SELECT "
#endif
																						 "TGL_SOFTWARE_ACCELERATED";
const GLubyte* glGetString(GLenum name) {
	switch (name) {
	case GL_VENDOR:
		return vendor_string;
	case GL_RENDERER:
		return renderer_string;
	case GL_VERSION:
		return version_string;
	case GL_EXTENSIONS:
		return extensions_string;
	case GL_LICENSE:
		return license_string;
	}
	return NULL;
}



void glGetIntegerv(GLint pname, GLint* params) {
	GLint i;
	GLContext* c = gl_get_context();
	i = 0;
	switch (pname) {
	case GL_MAX_BUFFERS:
		*params = MAX_BUFFERS;
		break;
	case GL_TEXTURE_HASH_TABLE_SIZE:
		*params = TEXTURE_HASH_TABLE_SIZE;
		break;

	case GL_LIGHT15:
		i++;
	case GL_LIGHT14:
		i++;
	case GL_LIGHT13:
		i++;
	case GL_LIGHT12:
		i++;
	case GL_LIGHT11:
		i++;
	case GL_LIGHT10:
		i++;
	case GL_LIGHT9:
		i++;
	case GL_LIGHT8:
		i++;
	case GL_LIGHT7:
		i++;
	case GL_LIGHT6:
		i++;
	case GL_LIGHT5:
		i++;
	case GL_LIGHT4:
		i++;
	case GL_LIGHT3:
		i++;
	case GL_LIGHT2:
		i++;
	case GL_LIGHT1:
		i++;
	case GL_LIGHT0:
		*params = c->lights[i].enabled;
		break;
	case GL_COLOR_ARRAY:
		*params = ((c->client_states & COLOR_ARRAY) != 0);
		break;
	case GL_COLOR_ARRAY_SIZE:
		*params = (c->color_array_size);
		break;
	case GL_COLOR_ARRAY_STRIDE:
		*params = c->color_array_stride;
		break;
	case GL_VERTEX_ARRAY:
		*params = ((c->client_states & VERTEX_ARRAY) != 0);
		break;
	case GL_VERTEX_ARRAY_SIZE:
		*params = c->vertex_array_size;
		break;
	case GL_VERTEX_ARRAY_STRIDE:
		*params = c->vertex_array_stride;
		break;
	case GL_TEXTURE_COORD_ARRAY:
		*params = ((c->client_states & TEXCOORD_ARRAY) != 0);
		break;
	case GL_TEXTURE_COORD_ARRAY_SIZE:
		*params = c->texcoord_array_size;
		break;
	case GL_TEXTURE_COORD_ARRAY_STRIDE:
		*params = c->texcoord_array_stride;
		break;
	case GL_NORMAL_ARRAY:
		*params = ((c->client_states & NORMAL_ARRAY) != 0);
		break;
	case GL_NORMAL_ARRAY_STRIDE:
		*params = c->normal_array_stride;
		break;
	case GL_BLEND:
		*params = c->zb->enable_blend;
		break;
	case GL_SHADE_MODEL:
		*params = c->current_shade_model;
		break;
	case GL_BLEND_DST:
		*params = c->zb->dfactor;
		break;
	case GL_BLEND_SRC:
		*params = c->zb->sfactor;
		break;
	case GL_POLYGON_MODE:
		params[0] = c->polygon_mode_front;
		params[1] = c->polygon_mode_back;
		break;
	case GL_LIST_MODE:
		if (c->compile_flag && !c->exec_flag)
			params[0] = GL_COMPILE;
		else if (c->exec_flag && c->compile_flag)
			params[0] = GL_COMPILE_AND_EXECUTE;
		else
			params[0] = 0;
		break;
	case GL_LIST_BASE:
		*params = c->listbase;
		break;
	case GL_LIST_INDEX: /* TODO*/
		params[0] = 0;
		break;
	case GL_TEXTURE_2D:
	case GL_TEXTURE_1D:
		params[0] = c->texture_2d_enabled;
		break;
	case GL_POLYGON_STIPPLE:
#if TGL_FEATURE_POLYGON_STIPPLE == 1
		params[0] = c->zb->dostipple;
#else
		params[0] = GL_FALSE;
#endif
		break;
	case GL_LIGHT_MODEL_LOCAL_VIEWER:
		*params = c->local_light_model;
		break;
	case GL_FOG_INDEX:
		*params = 0;
		break;
	case GL_FOG_COLOR:
		params[0] = 0;
		params[1] = 0;
		params[2] = 0;
		params[3] = 0;
		break;
	case GL_FOG_MODE:
		*params = GL_EXP;
		break;
	case GL_LIGHTING:
		*params = (c->lighting_enabled != 0);
		break;
	case GL_LIGHT_MODEL_TWO_SIDE:
		*params = (c->light_model_two_side != 0);
		break;
	case GL_LINE_STIPPLE_REPEAT:
		*params = 1;
		break;
	case GL_LINE_STIPPLE:
		*params = 0;
		break;
	case GL_LINE_STIPPLE_PATTERN:
		*params = (GLushort)(~0);
		break;
	case GL_NORMALIZE:
		params[0] = c->normalize_enabled;
		break;
	case GL_POINT_SMOOTH_HINT:
	case GL_FOG_HINT:
	case GL_LINE_SMOOTH_HINT:
	case GL_PERSPECTIVE_CORRECTION_HINT:
	case GL_POLYGON_SMOOTH_HINT:
		*params = GL_FASTEST;
		break;
	case GL_BLUE_SCALE:
	case GL_RED_SCALE:
	case GL_GREEN_SCALE:
	case GL_ALPHA_SCALE:
		*params = 1;
		break;
	case GL_SUBPIXEL_BITS:
		*params = 6;
		break;
	case GL_MATRIX_MODE:
		if (c->matrix_mode == 0)
			*params = GL_MODELVIEW;
		else if (c->matrix_mode == 1)
			*params = GL_PROJECTION;
		else if (c->matrix_mode == 2)
			*params = GL_TEXTURE;
		break;
	case GL_BLUE_BIAS:
	case GL_RED_BIAS:
	case GL_GREEN_BIAS:
	case GL_ALPHA_BIAS:
	case GL_CLIP_PLANE0:
	case GL_CLIP_PLANE1:
	case GL_CLIP_PLANE2:
	case GL_CLIP_PLANE3:
	case GL_CLIP_PLANE4:
	case GL_CLIP_PLANE5:
	case GL_SCISSOR_TEST:
	case GL_UNPACK_SWAP_BYTES:
	case GL_UNPACK_SKIP_ROWS:
	case GL_UNPACK_SKIP_PIXELS:
	case GL_UNPACK_ROW_LENGTH:
	case GL_UNPACK_LSB_FIRST:
	case GL_TEXTURE_GEN_T:
	case GL_TEXTURE_GEN_S:
	case GL_TEXTURE_GEN_R:
	case GL_TEXTURE_GEN_Q:

	case GL_STEREO:
	case GL_PACK_SWAP_BYTES:
	case GL_PACK_SKIP_ROWS:
	case GL_PACK_SKIP_PIXELS:
	case GL_PACK_ROW_LENGTH:
	case GL_PACK_LSB_FIRST:
	case GL_STENCIL_TEST:
	case GL_MAX_PIXEL_MAP_TABLE:
	case GL_MAX_EVAL_ORDER:
	case GL_MAX_CLIENT_ATTRIB_STACK_DEPTH:
		params[0] = 0;
		break;
	case GL_POLYGON_MAX_VERTEX:
		params[0] = POLYGON_MAX_VERTEX;
		break;
	case GL_MAX_VIEWPORT_DIMS:
		params[0] = 4096;
		params[1] = 4096;
		break;
	case GL_MAX_LIST_NESTING:
		*params = ~(GLuint)0; /* No checking is done, indicate it to the user!*/
		break;
	case GL_STENCIL_FUNC:
		*params = GL_ALWAYS;
		break;
	case GL_STENCIL_CLEAR_VALUE:
		*params = 0;
		break;
	case GL_STENCIL_BITS:
	case GL_MAP_STENCIL:
	case GL_MAP_COLOR:
	case GL_INDEX_SHIFT:
	case GL_INDEX_OFFSET:
	case GL_INDEX_MODE:
	case GL_INDEX_CLEAR_VALUE:
	case GL_INDEX_BITS:
	case GL_INDEX_ARRAY:
		*params = 0;
		break;
	case GL_FRONT_FACE:
		*params = c->current_front_face;
		break;
	case GL_STENCIL_PASS_DEPTH_PASS:
	case GL_STENCIL_PASS_DEPTH_FAIL:
		*params = GL_KEEP;
		break;
	case GL_STENCIL_VALUE_MASK:
	case GL_INDEX_WRITEMASK:
		*params = ~0;
		break;
	case GL_UNPACK_ALIGNMENT:
	case GL_PACK_ALIGNMENT:
		*params = 4;
		break;
	case GL_COLOR_ARRAY_TYPE:
	case GL_NORMAL_ARRAY_TYPE:
	case GL_TEXTURE_COORD_ARRAY_TYPE:
	case GL_VERTEX_ARRAY_TYPE:
		*params = GL_FLOAT;
		break;
	case GL_RENDER_MODE:
#if TGL_FEATURE_ALT_RENDERMODES == 1
		*params = c->render_mode;
#else
		*params = GL_RENDER;
#endif
		break;
	case GL_BLEND_EQUATION:
		*params = c->zb->blendeq;
		break;
	case GL_DRAW_BUFFER:
		*params = c->drawbuffer;
		break;
	case GL_READ_BUFFER:
		*params = c->readbuffer;
		break;
	case GL_AUX_BUFFERS:
		*params = 0;
		break;
	case GL_PIXEL_MAP_S_TO_S_SIZE:
	case GL_PIXEL_MAP_I_TO_I_SIZE:
	case GL_PIXEL_MAP_I_TO_R_SIZE:
	case GL_PIXEL_MAP_I_TO_G_SIZE:
	case GL_PIXEL_MAP_I_TO_B_SIZE:
	case GL_PIXEL_MAP_I_TO_A_SIZE:
	case GL_PIXEL_MAP_R_TO_R_SIZE:
	case GL_PIXEL_MAP_G_TO_G_SIZE:
	case GL_PIXEL_MAP_B_TO_B_SIZE:
	case GL_PIXEL_MAP_A_TO_A_SIZE:
		*params = 0;
		break;
	case GL_RGBA_MODE:
		*params = 1; /* yes, even in 565 (it's what the spec says)*/
		break;
	case GL_VIEWPORT:
		params[0] = c->viewport.xmin;
		params[1] = c->viewport.ymin;
		params[2] = c->viewport.xsize;
		params[3] = c->viewport.ysize;
		break;
	case GL_MAX_SPECULAR_BUFFERS:
#if TGL_FEATURE_SPECULAR_BUFFERS == 1
		*params = MAX_SPECULAR_BUFFERS;
#else
		*params = 0;
#endif
		break;
	case GL_MAX_DISPLAY_LISTS:
		*params = MAX_DISPLAY_LISTS;
		break;
	case GL_ERROR_CHECK_LEVEL:
#if TGL_FEATURE_STRICT_OOM_CHECKS == 1
		*params = 2;
#elif TGL_FEATURE_ERROR_CHECK == 1
		*params = 1;
#else
		*params = 0;
#endif
		break;
	
	case GL_IS_SPECULAR_ENABLED:
		*params = c->zEnableSpecular;
		break;
	case GL_MAX_MODELVIEW_STACK_DEPTH:
		*params = MAX_MODELVIEW_STACK_DEPTH;
		break;
	case GL_MAX_NAME_STACK_DEPTH:
		*params = MAX_NAME_STACK_DEPTH;
		break;
	case GL_MAX_PROJECTION_STACK_DEPTH:
		*params = MAX_PROJECTION_STACK_DEPTH;
		break;
	case GL_MAX_TEXTURE_STACK_DEPTH:
		*params = MAX_TEXTURE_STACK_DEPTH;
		break;
	case GL_MAX_TEXTURE_LEVELS:
		*params = MAX_TEXTURE_LEVELS;
		break;
	case GL_GREEN_BITS:
#if TGL_FEATURE_RENDER_BITS == 16
		*params = 6;
#elif TGL_FEATURE_RENDER_BITS == 32
		*params = 8;
#endif
		break;
	case GL_BLUE_BITS:
	case GL_RED_BITS:
#if TGL_FEATURE_RENDER_BITS == 16
		*params = 5;
#elif TGL_FEATURE_RENDER_BITS == 32
		*params = 8;
#endif
		break;
	case GL_POLYGON_OFFSET_FILL:
		*params = ((c->offset_states & TGL_OFFSET_FILL) != 0);
		break;
	case GL_POLYGON_OFFSET_LINE:
		*params = ((c->offset_states & TGL_OFFSET_LINE) != 0);
		break;
	case GL_DEPTH_BITS:
		*params = 16;
		break;
	case GL_POLYGON_OFFSET_POINT:
		*params = ((c->offset_states & TGL_OFFSET_POINT) != 0);
		break;
	case GL_POLYGON_SMOOTH:
		*params = GL_FALSE;
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
	case GL_CURRENT_RASTER_POSITION_VALID:
		*params = c->rasterposvalid;
		break;
	case GL_FOG:
		*params = GL_FALSE;
		break;
	case GL_EDGE_FLAG:
		*params = c->current_edge_flag;
		break;
	case GL_DOUBLEBUFFER:
	case GL_DITHER:
		*params = GL_FALSE;
		break;
	case GL_DEPTH_TEST:
		*params = (c->zb->depth_test == 1);
		break;
	case GL_DEPTH_FUNC:
		*params = GL_LESS;
		break;

	default:
		tgl_warning("glGet: option not implemented");
#if TGL_FEATURE_ERROR_CHECK == 1
#define ERROR_FLAG GL_INVALID_ENUM
#include "error_check.h"
#endif
		break;
	}
}

void glGetFloatv(GLint pname, GLfloat* v) {
	GLint i, mnr;
	GLContext* c;
	mnr = 0; /* just a trick to return the correct matrix */
	c = gl_get_context();
	switch (pname) {
	case GL_BLUE_SCALE:
	case GL_RED_SCALE:
	case GL_GREEN_SCALE:
	case GL_ALPHA_SCALE:
	case GL_FOG_END:
	case GL_FOG_DENSITY:
		*v = 1;
		break;
	case GL_BLUE_BIAS:
	case GL_RED_BIAS:
	case GL_GREEN_BIAS:
	case GL_ALPHA_BIAS:
	case GL_FOG_START:
		*v = 0;
		break;
	case GL_DEPTH_SCALE:
		*v = 1;
		break;
	case GL_TEXTURE_MATRIX:
		mnr++;
	case GL_PROJECTION_MATRIX:
		mnr++;
	case GL_MODELVIEW_MATRIX: {
		GLfloat* p = &c->matrix_stack_ptr[mnr]->m[0][0];
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
	case GL_DEPTH_CLEAR_VALUE:
		*v = 1; /* This is not entirely true, but... good enough?*/
		break;
	case GL_DEPTH_RANGE:
		v[0] = 0;
		v[1] = 1;
		break;
	case GL_DEPTH_BIAS:
		*v = 0;
		break;
	case GL_CURRENT_TEXTURE_COORDS:
		v[0] = c->current_tex_coord.X;
		v[1] = c->current_tex_coord.Y;
		v[2] = c->current_tex_coord.Z;
		v[3] = c->current_tex_coord.W;
		break;
	case GL_CURRENT_RASTER_POSITION:
		v[0] = c->rastervertex.pc.X;
		v[1] = c->rastervertex.pc.Y;
		v[2] = c->rastervertex.pc.Z;
		v[3] = c->rastervertex.pc.W;
		break;
	case GL_CURRENT_RASTER_DISTANCE:
		*v = c->rastervertex.ec.Z;
		break;
	case GL_LINE_WIDTH_RANGE:
		v[0] = v[1] = 1.0f;
		break;
	case GL_POINT_SIZE:
		/* case GL_POINT_SIZE_MIN:*/
		/* case GL_POINT_SIZE_MAX:*/
		*v = c->zb->pointsize;
		break;
	case GL_FOG_COLOR:
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
		v[3] = 0;
		break;
	case GL_POINT_SIZE_GRANULARITY:
		*v = 1.0f; /* if we ever implement AA'd points...*/
		break;
	case GL_POLYGON_OFFSET_FACTOR:
		*v = 0;
		break;
	case GL_POLYGON_OFFSET_UNITS:
		*v = 0;
		break;
	case GL_LIGHT_MODEL_AMBIENT:
		for (i = 0; i < 4; i++)
			v[i] = c->ambient_light_model.v[i];
		break;
	case GL_ZOOM_X:
		*v = c->pzoomx;
		break;
	case GL_ZOOM_Y:
		*v = c->pzoomy;
		break;
	case GL_POINT_SIZE_RANGE:
		v[0] = c->zb->pointsize;
		v[1] = c->zb->pointsize;
		break;
	default:
		tgl_warning("warning: unknown pname in glGetFloatv()\n");
		break;
	}
}
