#ifndef _tgl_zgl_h_
#define _tgl_zgl_h_
#include "../include/GL/gl.h"
#include "../include/zfeatures.h"
#include "../include/zbuffer.h"
#include "zmath.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
//Needed for memcpy
#include <string.h>
#ifndef M_PI
#define M_PI 3.14159265358979323
#endif
//#define DEBUG
#ifndef NDEBUG
#define NDEBUG 
#endif
enum {

#define ADD_OP(a, b, c) OP_##a,

#include "opinfo.h"

};

/* initially # of allocated GLVertexes (will grow when necessary) */
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
#define MAX_NAME_STACK_DEPTH 64
#define MAX_TEXTURE_LEVELS 1
#define MAX_LIGHTS 16


#define VERTEX_ARRAY 0x0001
#define COLOR_ARRAY 0x0002
#define NORMAL_ARRAY 0x0004
#define TEXCOORD_ARRAY 0x0008
//#define VERTEX_HASH_SIZE 1031

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
	GLint updated;
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
	void* pixmap;
	GLint xsize, ysize;
} GLImage;

/* textures */

#define TEXTURE_HASH_TABLE_SIZE 256
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

typedef void (*gl_draw_triangle_func)(struct GLContext* c, GLVertex* p0, GLVertex* p1, GLVertex* p2);

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
	GLint render_mode;
	GLuint* select_buffer;
	GLint select_size;
	GLuint *select_ptr, *select_hit;
	GLint select_overflow;
	GLint select_hits;
	/* glDrawBuffer, glRenderBuffer */
	GLenum drawbuffer;
	GLenum readbuffer;
	/* feedback */
	//render_mode as seen above
	GLfloat* feedback_buffer;
	GLfloat* feedback_ptr;
	GLuint feedback_size;
	GLint feedback_hits;
	GLubyte feedback_overflow;
	GLenum feedback_type;
	/* names */
	GLuint name_stack[MAX_NAME_STACK_DEPTH];
	GLint name_stack_size;

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
	GLint vertex_max;
	GLVertex* vertex;

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
	GLint (*gl_resize_viewport)(struct GLContext* c, GLint* xsize, GLint* ysize);

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

extern GLContext* gl_ctx;
static inline GLContext* gl_get_context(void) { return gl_ctx; }
//void gl_add_op(GLParam* p);
extern void (*op_table_func[])(GLContext*, GLParam*);
extern GLint op_table_size[];
extern void gl_compile_op(GLContext* c, GLParam* p);
static inline void gl_add_op(GLParam* p) {
	GLContext* c = gl_ctx;
#include "error_check.h"
	GLint op;
	op = p[0].op;
	if (c->exec_flag) {
		op_table_func[op](c, p);
#include "error_check.h"
	}
	if (c->compile_flag) {
		gl_compile_op(c, p);
#include "error_check.h"
	}
	//if (c->print_flag) {
		//		gl_print_op(stderr, p);
	//}
}

/* select.c */
void gl_add_select(GLContext* c, GLuint zmin, GLuint zmax);
void gl_add_feedback(GLContext* c, GLfloat token,
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

static inline void gl_transform_to_viewport_clip_c(GLContext* c, GLVertex* v) {

	/* coordinates */
	{
		GLfloat winv = 1.0 / v->pc.W;
		v->zp.x = (GLint)(v->pc.X * winv * c->viewport.scale.X + c->viewport.trans.X);
		v->zp.y = (GLint)(v->pc.Y * winv * c->viewport.scale.Y + c->viewport.trans.Y);
		v->zp.z = (GLint)(v->pc.Z * winv * c->viewport.scale.Z + c->viewport.trans.Z);
	}
	/* color */
	v->zp.r = (GLuint)(v->color.v[0] * COLOR_CORRECTED_MULT_MASK + COLOR_MIN_MULT) & COLOR_MASK;
	v->zp.g = (GLuint)(v->color.v[1] * COLOR_CORRECTED_MULT_MASK + COLOR_MIN_MULT) & COLOR_MASK;
	v->zp.b = (GLuint)(v->color.v[2] * COLOR_CORRECTED_MULT_MASK + COLOR_MIN_MULT) & COLOR_MASK;

	/* texture */

	if (c->texture_2d_enabled) {
		v->zp.s = (GLint)(v->tex_coord.X * (ZB_POINT_S_MAX - ZB_POINT_S_MIN) + ZB_POINT_S_MIN); //MARKED
		v->zp.t = (GLint)(v->tex_coord.Y * (ZB_POINT_T_MAX - ZB_POINT_T_MIN) + ZB_POINT_T_MIN); //MARKED
	}
}
//void gl_transform_to_viewport(GLContext* c, GLVertex* v);
//inline void gl_draw_triangle(GLContext* c, GLVertex* p0, GLVertex* p1, GLVertex* p2);

/* triangle */

/*
 * Clipping
 */

/* We clip the segment [a,b] against the 6 planes of the normal volume.
 * We compute the point 'c' of GLintersection and the value of the parameter 't'
 * of the GLintersection if x=a+t(b-a).
 */
//MARK <POSSIBLE_PERF_BONUS>
#define clip_func(name, sign, dir, dir1, dir2) GLfloat name(V4* c, V4* a, V4* b);
//MARK <POSSIBLE_PERF_BONUS>
clip_func(clip_xmin, -, X, Y, Z)

	clip_func(clip_xmax, +, X, Y, Z)

		clip_func(clip_ymin, -, Y, X, Z)

			clip_func(clip_ymax, +, Y, X, Z)
//MARK <POSSIBLE_PERF_BONUS>
				clip_func(clip_zmin, -, Z, X, Y)

					clip_func(clip_zmax, +, Z, X, Y)

extern GLfloat (*clip_proc[6])(V4*, V4*, V4*);// = {clip_xmin, clip_xmax, clip_ymin, clip_ymax, clip_zmin, clip_zmax};

static inline void updateTmp(GLContext* c, GLVertex* q, GLVertex* p0, GLVertex* p1, GLfloat t) {
	{


		q->color.v[0] = p0->color.v[0] + (p1->color.v[0] - p0->color.v[0]) * t;
		q->color.v[1] = p0->color.v[1] + (p1->color.v[1] - p0->color.v[1]) * t;
		q->color.v[2] = p0->color.v[2] + (p1->color.v[2] - p0->color.v[2]) * t;
	}
	//	*/
	if (c->texture_2d_enabled) {
		q->tex_coord.X = p0->tex_coord.X + (p1->tex_coord.X - p0->tex_coord.X) * t;
		q->tex_coord.Y = p0->tex_coord.Y + (p1->tex_coord.Y - p0->tex_coord.Y) * t;
	}

	q->clip_code = gl_clipcode(q->pc.X, q->pc.Y, q->pc.Z, q->pc.W);
	if (q->clip_code == 0)
		gl_transform_to_viewport_clip_c(c, q);
}

//inline void gl_draw_triangle_clip(GLContext* c, GLVertex* p0, GLVertex* p1, GLVertex* p2, GLint clip_bit);

static inline void gl_draw_triangle_clip(GLContext* c, GLVertex* p0, GLVertex* p1, GLVertex* p2, GLint clip_bit);

static inline void gl_draw_triangle(GLContext* c, GLVertex* p0, GLVertex* p1, GLVertex* p2) {
	GLint co, cc[3], front;
	

	cc[0] = p0->clip_code;
	cc[1] = p1->clip_code;
	cc[2] = p2->clip_code;

	co = cc[0] | cc[1] | cc[2];

	/* we handle the non clipped case here to go faster */
	if (co == 0) {
		GLfloat norm;
		norm = (GLfloat)(p1->zp.x - p0->zp.x) * (GLfloat)(p2->zp.y - p0->zp.y) - (GLfloat)(p2->zp.x - p0->zp.x) * (GLfloat)(p1->zp.y - p0->zp.y);

		if (norm == 0) //MARK <POSSIBLE_PERF_BONUS>
			return;

		front = norm < 0.0;
		front = front ^ c->current_front_face; //I don't know how this works, but it does, GL_CCW is 0x901 and CW is 900.

		/* back face culling */
		if (c->cull_face_enabled) {
			/* most used case first */
			if (c->current_cull_face == GL_BACK) {
				if (front == 0)
					return;
				c->draw_triangle_front(c, p0, p1, p2);
			} else if (c->current_cull_face == GL_FRONT) {
				if (front != 0)
					return;
				c->draw_triangle_back(c, p0, p1, p2);
			} else {
				return;
			}
		} else {
			/* no culling */
			if (front) {
				c->draw_triangle_front(c, p0, p1, p2);
			} else {
				c->draw_triangle_back(c, p0, p1, p2);
			}
		}
	} else {
		//GLint c_and = cc[0] & cc[1] & cc[2];
		if ((cc[0] & cc[1] & cc[2]) == 0) { // Don't draw a triangle with no points
			gl_draw_triangle_clip(c, p0, p1, p2, 0);
		}
	}
}


static inline void gl_draw_triangle_clip(GLContext* c, GLVertex* p0, GLVertex* p1, GLVertex* p2, GLint clip_bit) {
	GLint co, c_and, co1, cc[3], edge_flag_tmp, clip_mask;
	//GLVertex tmp1, tmp2, *q[3];
	GLVertex *q[3];


	cc[0] = p0->clip_code;
	cc[1] = p1->clip_code;
	cc[2] = p2->clip_code;

	co = cc[0] | cc[1] | cc[2];
	if (co == 0) {
		gl_draw_triangle(c, p0, p1, p2);
	} else {
		
		c_and = cc[0] & cc[1] & cc[2];
		/* the triangle is completely outside */
		if (c_and != 0)
			return;

		/* find the next direction to clip */
		while (clip_bit < 6 && (co & (1 << clip_bit)) == 0) {
			clip_bit++;
		}

		/* this test can be true only in case of rounding errors */
		if (clip_bit == 6) { //The 2 bit and the 4 bit.
#if 0
      tgl_warning("Error:\n");tgl_warning("%f %f %f %f\n",p0->pc.X,p0->pc.Y,p0->pc.Z,p0->pc.W);tgl_warning("%f %f %f %f\n",p1->pc.X,p1->pc.Y,p1->pc.Z,p1->pc.W);tgl_warning("%f %f %f %f\n",p2->pc.X,p2->pc.Y,p2->pc.Z,p2->pc.W);
#endif
			return;
		}

		clip_mask = 1 << clip_bit;
		co1 = (cc[0] ^ cc[1] ^ cc[2]) & clip_mask;

		if (co1) {
			/* one point outside */
			
			if (cc[0] & clip_mask) {
				q[0] = p0;
				q[1] = p1;
				q[2] = p2;
			} else if (cc[1] & clip_mask) {
				q[0] = p1;
				q[1] = p2;
				q[2] = p0;
			} else {
				q[0] = p2;
				q[1] = p0;
				q[2] = p1;
			}
			{GLVertex tmp1, tmp2;GLfloat tt;
				tt = clip_proc[clip_bit](&tmp1.pc, &q[0]->pc, &q[1]->pc);
				updateTmp(c, &tmp1, q[0], q[1], tt);

				tt = clip_proc[clip_bit](&tmp2.pc, &q[0]->pc, &q[2]->pc);
				updateTmp(c, &tmp2, q[0], q[2], tt);

				tmp1.edge_flag = q[0]->edge_flag;
				edge_flag_tmp = q[2]->edge_flag;
				q[2]->edge_flag = 0;
				gl_draw_triangle_clip(c, &tmp1, q[1], q[2], clip_bit + 1);

				tmp2.edge_flag = 1;
				tmp1.edge_flag = 0;
				q[2]->edge_flag = edge_flag_tmp;
				gl_draw_triangle_clip(c, &tmp2, &tmp1, q[2], clip_bit + 1);
			}
		} else {
			/* two points outside */
			
			if ((cc[0] & clip_mask) == 0) {
				q[0] = p0;
				q[1] = p1;
				q[2] = p2;
			} else if ((cc[1] & clip_mask) == 0) {
				q[0] = p1;
				q[1] = p2;
				q[2] = p0;
			} else {
				q[0] = p2;
				q[1] = p0;
				q[2] = p1;
			}
			{GLVertex tmp1, tmp2;GLfloat tt;
				tt = clip_proc[clip_bit](&tmp1.pc, &q[0]->pc, &q[1]->pc);
				updateTmp(c, &tmp1, q[0], q[1], tt);

				tt = clip_proc[clip_bit](&tmp2.pc, &q[0]->pc, &q[2]->pc);
				updateTmp(c, &tmp2, q[0], q[2], tt);

				tmp1.edge_flag = 1;
				tmp2.edge_flag = q[2]->edge_flag;
				gl_draw_triangle_clip(c, q[0], &tmp1, &tmp2, clip_bit + 1);
			}
		}
	}
}

//inline void gl_draw_line(GLContext* c, GLVertex* p0, GLVertex* p1);
static inline void gl_add_select1(GLContext* c, GLint z1, GLint z2, GLint z3) {
	GLint min, max;
	min = max = z1;
	if (z2 < min)
		min = z2;
	if (z3 < min)
		min = z3;
	if (z2 > max)
		max = z2;
	if (z3 > max)
		max = z3;

	gl_add_select(c, 0xffffffff - min, 0xffffffff - max);
}
static inline void GLinterpolate(GLVertex* q, GLVertex* p0, GLVertex* p1, GLfloat t) {
	q->pc.X = p0->pc.X + (p1->pc.X - p0->pc.X) * t;
	q->pc.Y = p0->pc.Y + (p1->pc.Y - p0->pc.Y) * t;
	q->pc.Z = p0->pc.Z + (p1->pc.Z - p0->pc.Z) * t;
	q->pc.W = p0->pc.W + (p1->pc.W - p0->pc.W) * t;

	for(int i = 0; i < 3; i++)
		q->color.v[i] = p0->color.v[i] + (p1->color.v[i] - p0->color.v[i]) * t;
}
static inline GLint ClipLine1(GLfloat denom, GLfloat num, GLfloat* tmin, GLfloat* tmax) {
	GLfloat t;

	if (denom > 0) {
		t = num / denom;
		if (t > *tmax)
			return 0;
		if (t > *tmin)
			*tmin = t;
	} else if (denom < 0) {
		t = num / denom;
		if (t < *tmin)
			return 0;
		if (t < *tmax)
			*tmax = t;
	} else if (num > 0)
		return 0;
	return 1;
}
static inline void gl_draw_line(GLContext* c, GLVertex* p1, GLVertex* p2) {
	GLfloat dx, dy, dz, dw, x1, y1, z1, w1;
	
	GLVertex q1, q2;
	GLint cc1, cc2;

	cc1 = p1->clip_code;
	cc2 = p2->clip_code;

	if ((cc1 | cc2) == 0) {
		if (c->render_mode == GL_SELECT) {
			gl_add_select1(c, p1->zp.z, p2->zp.z, p2->zp.z);
		}else if (c->render_mode == GL_FEEDBACK){
			gl_add_feedback(
				c,	GL_LINE_TOKEN,
				p1,
				p2,
				NULL,
				0
			);
		} else {
			if (c->zb->depth_test)
				ZB_line_z(c->zb, &p1->zp, &p2->zp);
			else
				ZB_line(c->zb, &p1->zp, &p2->zp);
		}
	} else if ((cc1 & cc2) != 0) {
		return;
	} else {
		dx = p2->pc.X - p1->pc.X;
		dy = p2->pc.Y - p1->pc.Y;
		dz = p2->pc.Z - p1->pc.Z;
		dw = p2->pc.W - p1->pc.W;
		x1 = p1->pc.X;
		y1 = p1->pc.Y;
		z1 = p1->pc.Z;
		w1 = p1->pc.W;

		GLfloat tmin = 0;
		GLfloat tmax = 1;
		if (ClipLine1(dx + dw, -x1 - w1, &tmin, &tmax) && ClipLine1(-dx + dw, x1 - w1, &tmin, &tmax) && ClipLine1(dy + dw, -y1 - w1, &tmin, &tmax) &&
			ClipLine1(-dy + dw, y1 - w1, &tmin, &tmax) && ClipLine1(dz + dw, -z1 - w1, &tmin, &tmax) && ClipLine1(-dz + dw, z1 - w1, &tmin, &tmax)) {

			GLinterpolate(&q1, p1, p2, tmin);
			GLinterpolate(&q2, p1, p2, tmax);
			gl_transform_to_viewport_clip_c(c, &q1);
			gl_transform_to_viewport_clip_c(c, &q2);

			if (c->zb->depth_test)
				ZB_line_z(c->zb, &q1.zp, &q2.zp);
			else
				ZB_line(c->zb, &q1.zp, &q2.zp);
		}
	}
}

//inline void gl_draw_point(GLContext* c, GLVertex* p0);
inline void gl_draw_point(GLContext* c, GLVertex* p0) {
	if (p0->clip_code == 0) {
		if (c->render_mode == GL_SELECT) {
			gl_add_select(c, p0->zp.z, p0->zp.z);
		}else if (c->render_mode == GL_FEEDBACK){
			gl_add_feedback(c,GL_POINT_TOKEN,p0,NULL,NULL,0);
		} else {
			ZB_plot(c->zb, &p0->zp);
		}
	}
}

void gl_draw_triangle_point(GLContext* c, GLVertex* p0, GLVertex* p1, GLVertex* p2); //MUST BE FUNCTION POINTER
void gl_draw_triangle_line(GLContext* c, GLVertex* p0, GLVertex* p1, GLVertex* p2); //MUST BE FUNCTION POINTER
void gl_draw_triangle_fill(GLContext* c, GLVertex* p0, GLVertex* p1, GLVertex* p2); //MUST BE FUNCTION POINTER
void gl_draw_triangle_select(GLContext* c, GLVertex* p0, GLVertex* p1, GLVertex* p2); //MUST BE FUNCTION POINTER
void gl_draw_triangle_feedback(GLContext* c, GLVertex* p0, GLVertex* p1, GLVertex* p2); //MUST BE FUNCTION POINTER
/* matrix.c */
void gl_print_matrix(const GLfloat* m);
/*
void glopLoadIdentity(GLContext *c,GLParam *p);
void glopTranslate(GLContext *c,GLParam *p);*/



/* light.c */
void gl_enable_disable_light(GLContext* c, GLint light, GLint v);
void gl_shade_vertex(GLContext* c, GLVertex* v);

void glInitTextures(GLContext* c);
void glEndTextures(GLContext* c);
GLTexture* alloc_texture(GLContext* c, GLint h);

/* image_util.c */
void gl_convertRGB_to_5R6G5B(GLushort* pixmap, GLubyte* rgb, GLint xsize, GLint ysize);
void gl_convertRGB_to_8A8R8G8B(GLuint* pixmap, GLubyte* rgb, GLint xsize, GLint ysize);
void gl_resizeImage(GLubyte* dest, GLint xsize_dest, GLint ysize_dest, GLubyte* src, GLint xsize_src, GLint ysize_src);
void gl_resizeImageNoInterpolate(GLubyte* dest, GLint xsize_dest, GLint ysize_dest, GLubyte* src, GLint xsize_src, GLint ysize_src);

//static GLContext* gl_get_context(void);

void gl_fatal_error(char* format, ...);

/* specular buffer "api" */
GLSpecBuf* specbuf_get_buffer(GLContext* c, const GLint shininess_i, const GLfloat shininess);

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

#define ADD_OP(a, b, c) void glop##a(GLContext*, GLParam*);
#include "opinfo.h"

/* this clip epsilon is needed to avoid some rounding errors after
   several clipping stages */






#endif /* _tgl_zgl_h_ */
