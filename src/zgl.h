#ifndef _tgl_zgl_h_
#define _tgl_zgl_h_
#ifndef NDEBUG
#define NDEBUG 
#endif
#include "../include/GL/gl.h"
#include "../include/zfeatures.h"
#include "../include/zbuffer.h"
#include "zmath.h"
#include <math.h>
#include <stdlib.h>
//Needed for memcpy
#include <string.h>
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif
//#define DEBUG

enum {

#define ADD_OP(a, b, c) OP_##a,

#include "opinfo.h"

};

//
#if TGL_FEATURE_GL_POLYGON == 1
//Large enough for your nice juicy GL_POLYGONs
#define POLYGON_MAX_VERTEX 16
#else 
//Just large enough for a quad!
#define POLYGON_MAX_VERTEX 4
#endif
/* Max # of specular light pow buffers */
#define MAX_SPECULAR_BUFFERS 32
//#define MAX_SPECULAR_BUFFERS 16
/* # of entries in specular buffer */
#define SPECULAR_BUFFER_SIZE 512
/* specular buffer granularity */
//#define SPECULAR_BUFFER_RESOLUTION 1024

#define MAX_MODELVIEW_STACK_DEPTH 32
#define MAX_PROJECTION_STACK_DEPTH 8
#define MAX_TEXTURE_STACK_DEPTH 8
#define MAX_NAME_STACK_DEPTH 16
#define MAX_TEXTURE_LEVELS 1
#define MAX_LIGHTS 16


#define VERTEX_ARRAY 0x0001
#define COLOR_ARRAY 0x0002
#define NORMAL_ARRAY 0x0004
#define TEXCOORD_ARRAY 0x0008

#define MAX_DISPLAY_LISTS 1024
#define OP_BUFFER_MAX_SIZE 4096

#define TGL_OFFSET_FILL 0x1
#define TGL_OFFSET_LINE 0x2
#define TGL_OFFSET_POINT 0x4



typedef struct GLSpecBuf {
	GLint shininess_i;
	GLint last_used;
	GLfloat buf[SPECULAR_BUFFER_SIZE + 1];
	struct GLSpecBuf* next;
} GLSpecBuf;

typedef struct GLLight {
	V4 ambient;
	V4 diffuse;
	V4 specular;
	V4 position;
	V3 spot_direction;
	GLfloat spot_exponent;
	GLfloat spot_cutoff;
	GLfloat attenuation[3];
	/* precomputed values */
	GLfloat cos_spot_cutoff;
	V3 norm_spot_direction;
	V3 norm_position;
	/* we use a linked list to know which are the enabled lights */
	GLint enabled;
	struct GLLight *next, *prev;
} GLLight;

typedef struct GLMaterial {
	V4 emission;
	V4 ambient;
	V4 diffuse;
	V4 specular;
	GLfloat shininess;

	/* computed values */
	GLint shininess_i;
	GLint do_specular;
} GLMaterial;

typedef struct GLViewport {
	GLint xmin, ymin, xsize, ysize;
	V3 scale;
	V3 trans;
//	GLint updated;
} GLViewport;

typedef union {
	GLint op;
	GLfloat f;
	GLint i;
	GLuint ui;
	void* p;
} GLParam;

typedef struct GLParamBuffer {
	GLParam ops[OP_BUFFER_MAX_SIZE];
	struct GLParamBuffer* next;
} GLParamBuffer;

typedef struct GLList {
	GLParamBuffer* first_op_buffer;
	/* TODO: extensions for an hash table or a better allocating scheme */
} GLList;

typedef struct GLVertex {
	GLint edge_flag;
	V3 normal;
	V4 coord;
	V4 tex_coord;
	V4 color;

	/* computed values */
	V4 ec;			 /* eye coordinates */
	V4 pc;			 /* coordinates in the normalized volume */
	GLint clip_code; /* clip code */
	ZBufferPoint zp; /* GLinteger coordinates for the rasterization */
} GLVertex;

typedef struct GLImage {
	PIXEL pixmap[TGL_FEATURE_TEXTURE_DIM * TGL_FEATURE_TEXTURE_DIM];
	GLint xsize, ysize;
} GLImage;

/* textures */

#define TEXTURE_HASH_TABLE_SIZE 256
#define TEXTURE_HASH_TABLE_MASK 255
typedef struct GLTexture {
	GLImage images[MAX_TEXTURE_LEVELS];
	GLint handle;
	struct GLTexture *next, *prev;
} GLTexture;

/* buffers */
#define MAX_BUFFERS 2048
typedef struct GLBuffer{
	void* data;
	GLuint size;
} GLBuffer;

/* shared state */
typedef struct GLSharedState {
	GLList** lists;
	GLTexture** texture_hash_table;
	GLBuffer** buffers;
} GLSharedState;

struct GLContext;

typedef void (*gl_draw_triangle_func)(GLVertex* p0, GLVertex* p1, GLVertex* p2);

/* display context */

typedef struct GLContext {
	/* Z buffer */
	ZBuffer* zb;
#if TGL_FEATURE_ERROR_CHECK == 1
	GLenum error_flag;
#endif
	/* lights */
	GLLight lights[MAX_LIGHTS];
	GLLight* first_light;
	V4 ambient_light_model;
	GLint local_light_model;
	GLint lighting_enabled;
	GLint light_model_two_side;

	/* materials */
	GLMaterial materials[2];
	GLint color_material_enabled;
	GLint current_color_material_mode;
	GLint current_color_material_type;

	/* textures */
	GLTexture* current_texture;
	GLint texture_2d_enabled;

	/* shared state */
	GLSharedState shared_state;

	/* current list */
	GLParamBuffer* current_op_buffer;
	GLint current_op_buffer_index;
	GLint exec_flag, compile_flag, print_flag;
	GLuint listbase;
	/* matrix */

	GLint matrix_mode;
	M4* matrix_stack[3];
	M4* matrix_stack_ptr[3];
	GLint matrix_stack_depth_max[3];

	M4 matrix_model_view_inv;
	M4 matrix_model_projection;
	GLint matrix_model_projection_updated;
	GLint matrix_model_projection_no_w_transform;
	GLint apply_texture_matrix;

	/* viewport */
	GLViewport viewport;

	/* current state */
	GLint polygon_mode_back;
	GLint polygon_mode_front;

	GLint current_front_face;
	GLint current_shade_model;
	GLint current_cull_face;
	GLint cull_face_enabled;
	GLint normalize_enabled;
	gl_draw_triangle_func draw_triangle_front, draw_triangle_back;

	/* selection */
#if TGL_FEATURE_ALT_RENDERMODES == 1
	GLint render_mode;
	GLuint* select_buffer;
	GLint select_size;
	GLuint *select_ptr, *select_hit;
	GLint select_overflow;
	GLint select_hits;
#endif
	/* glDrawBuffer, glRenderBuffer */
	GLenum drawbuffer;
	GLenum readbuffer;
	/* feedback */
	//render_mode as seen above
#if TGL_FEATURE_ALT_RENDERMODES == 1
	GLfloat* feedback_buffer;
	GLfloat* feedback_ptr;
	GLuint feedback_size;
	GLint feedback_hits;
	GLubyte feedback_overflow;
	GLenum feedback_type;
	/* names */
	GLuint name_stack[MAX_NAME_STACK_DEPTH];
	GLint name_stack_size;
#endif
	


	/* clear */
	GLfloat clear_depth;
	V4 clear_color;

	/* current vertex state */
	V4 current_color;
	V4 current_normal;
	V4 current_tex_coord;
	GLint current_edge_flag;

	/* glBegin / glEnd */
	GLint in_begin;
	GLint begin_type;
	GLint vertex_n, vertex_cnt;
	//GLint vertex_max;
	//GLVertex vertex*;
	GLVertex vertex[POLYGON_MAX_VERTEX];

	/* opengl 1.1 arrays  */
	GLfloat* vertex_array;
	GLint vertex_array_size;
	GLint vertex_array_stride;
	GLfloat* normal_array;
	GLint normal_array_stride;
	GLfloat* color_array;
	GLint color_array_size;
	GLint color_array_stride;
	GLfloat* texcoord_array;
	GLint texcoord_array_size;
	GLint texcoord_array_stride;
	GLint client_states;

	/* opengl 1.1 polygon offset */
	GLfloat offset_factor;
	GLfloat offset_units;
	GLint offset_states;

	/* opengl blending */
	// All the settings are in the Zbuffer!

	/* specular buffer. could probably be shared between contexts,
	  but that wouldn't be 100% thread safe */
#if TGL_FEATURE_SPECULAR_BUFFERS == 1
	GLSpecBuf* specbuf_first;
	GLint specbuf_used_counter;
	GLint specbuf_num_buffers;
#endif 
	GLint zEnableSpecular; // Enable specular lighting
	/* opaque structure for user's use */
	void* opaque;
	/* resize viewport function */
	GLint (*gl_resize_viewport)(GLint* xsize, GLint* ysize);

	/* depth test */
	//Moved to Zbuffer.

	/* raster position */
	V4 rasterpos;
	GLint rasterpos_zz;
	GLubyte rasterposvalid;
	GLfloat pzoomx, pzoomy;
	GLVertex rastervertex;
	/* text */
	GLTEXTSIZE textsize;

	/* buffers */
	GLint boundarraybuffer; //0 if no buffer is bound.
	GLint boundvertexbuffer;
	GLint boundnormalbuffer;
	GLint boundcolorbuffer;
	GLint boundtexcoordbuffer;
} GLContext;

extern GLContext gl_ctx;
static inline GLContext* gl_get_context(void) { return &gl_ctx; }
//void gl_add_op(GLParam* p);
extern void (*op_table_func[])(GLParam*);
extern GLint op_table_size[];
extern void gl_compile_op(GLParam* p);
static inline void gl_add_op(GLParam* p) {
	GLContext* c = gl_get_context();
#if TGL_FEATURE_ERROR_CHECK == 1
#include "error_check.h"
#endif
	GLint op;
	op = p[0].op;
	if (c->exec_flag) {
		op_table_func[op](p);
#if TGL_FEATURE_ERROR_CHECK == 1
#include "error_check.h"
#endif
	}
	if (c->compile_flag) {
		gl_compile_op( p);
#if TGL_FEATURE_ERROR_CHECK == 1
#include "error_check.h"
#endif
	}
	//if (c->print_flag) {
		//		gl_print_op(stderr, p);
	//}
}

/* select.c */
void gl_add_select( GLuint zmin, GLuint zmax);
void gl_add_feedback( GLfloat token,
										GLVertex* v1,
										GLVertex* v2,
										GLVertex* v3,
										GLfloat passthrough_token_value
);

/* clip.c */

#define CLIP_EPSILON (1E-5)
//Many of Clip.c's functions are inlined.
static inline GLint gl_clipcode(GLfloat x, GLfloat y, GLfloat z, GLfloat w1) {
	GLfloat w;

	w = w1 * (1.0 + CLIP_EPSILON);
	return (x < -w) | ((x > w) << 1) | ((y < -w) << 2) | ((y > w) << 3) | ((z < -w) << 4) | ((z > w) << 5);
}

#define CLIP_XMIN (1 << 0)
#define CLIP_XMAX (1 << 1)
#define CLIP_YMIN (1 << 2)
#define CLIP_YMAX (1 << 3)
#define CLIP_ZMIN (1 << 4)
#define CLIP_ZMAX (1 << 5)

static inline GLfloat clampf(GLfloat a, GLfloat min, GLfloat max) {
	if (a < min)
		return min;
	else if (a > max)
		return max;
	else
		return a;
}


//void gl_transform_to_viewport( GLVertex* v);
//inline void gl_draw_triangle( GLVertex* p0, GLVertex* p1, GLVertex* p2);

/* triangle */

/*
 * Clipping
 */

/* We clip the segment [a,b] against the 6 planes of the normal volume.
 * We compute the point 'c' of GLintersection and the value of the parameter 't'
 * of the GLintersection if x=a+t(b-a).
 */



//inline void gl_draw_triangle_clip( GLVertex* p0, GLVertex* p1, GLVertex* p2, GLint clip_bit);

void gl_draw_triangle( GLVertex* p0, GLVertex* p1, GLVertex* p2);
void gl_draw_line( GLVertex* p0, GLVertex* p1);
void gl_draw_point( GLVertex* p0);


void gl_draw_triangle_point( GLVertex* p0, GLVertex* p1, GLVertex* p2); //MUST BE FUNCTION POINTER
void gl_draw_triangle_line( GLVertex* p0, GLVertex* p1, GLVertex* p2); //MUST BE FUNCTION POINTER
void gl_draw_triangle_fill( GLVertex* p0, GLVertex* p1, GLVertex* p2); //MUST BE FUNCTION POINTER
void gl_draw_triangle_select( GLVertex* p0, GLVertex* p1, GLVertex* p2); //MUST BE FUNCTION POINTER
void gl_draw_triangle_feedback( GLVertex* p0, GLVertex* p1, GLVertex* p2); //MUST BE FUNCTION POINTER
/* matrix.c */
void gl_print_matrix(const GLfloat* m);
/*
void glopLoadIdentity(GLParam *p);
void glopTranslate(GLParam *p);*/



/* light.c */
void gl_enable_disable_light( GLint light, GLint v);
void gl_shade_vertex(GLVertex* v);

void glInitTextures();
void glEndTextures();
GLTexture* alloc_texture( GLint h);

/* image_util.c */
void gl_convertRGB_to_5R6G5B(GLushort* pixmap, GLubyte* rgb, GLint xsize, GLint ysize);
void gl_convertRGB_to_8A8R8G8B(GLuint* pixmap, GLubyte* rgb, GLint xsize, GLint ysize);
void gl_resizeImage(GLubyte* dest, GLint xsize_dest, GLint ysize_dest, GLubyte* src, GLint xsize_src, GLint ysize_src);
void gl_resizeImageNoInterpolate(GLubyte* dest, GLint xsize_dest, GLint ysize_dest, GLubyte* src, GLint xsize_src, GLint ysize_src);

//static GLContext* gl_get_context(void);

void gl_fatal_error(char* format, ...);

/* specular buffer "api" */
GLSpecBuf* specbuf_get_buffer( const GLint shininess_i, const GLfloat shininess);

#ifdef __BEOS__
void dprintf(const char*, ...);

#else /* !BEOS */

#ifdef DEBUG

#define dprintf(format, args...) tgl_warning("In '%s': " format "\n", __FUNCTION__, ##args);

#else

#define dprintf(format, args...)

#endif
#endif /* !BEOS */

/* glopXXX functions */

#define ADD_OP(a, b, c) void glop##a(GLParam*);
#include "opinfo.h"

/* this clip epsilon is needed to avoid some rounding errors after
   several clipping stages */



static inline void gl_eval_viewport() {
	GLContext* c = gl_get_context();
	GLViewport* v;
	GLfloat zsize = (1 << (ZB_Z_BITS + ZB_POINT_Z_FRAC_BITS));

	v = &c->viewport;

	v->trans.X = ((v->xsize - 0.5) / 2.0) + v->xmin;
	v->trans.Y = ((v->ysize - 0.5) / 2.0) + v->ymin;
	v->trans.Z = ((zsize - 0.5) / 2.0) + ((1 << ZB_POINT_Z_FRAC_BITS)) / 2;

	v->scale.X = (v->xsize - 0.5) / 2.0;
	v->scale.Y = -(v->ysize - 0.5) / 2.0;
	v->scale.Z = -((zsize - 0.5) / 2.0);
}


#endif /* _tgl_zgl_h_ */
