#include "zgl.h"
#include <string.h>
void glopNormal(GLParam* p) {
	V3 v;
	GLContext* c = gl_get_context();
	v.X = p[1].f;
	v.Y = p[2].f;
	v.Z = p[3].f;

	c->current_normal.X = v.X;
	c->current_normal.Y = v.Y;
	c->current_normal.Z = v.Z;
	c->current_normal.W = 0;
}

void glopTexCoord(GLParam* p) {
	GLContext* c = gl_get_context();
	c->current_tex_coord.X = p[1].f;
	c->current_tex_coord.Y = p[2].f;
	c->current_tex_coord.Z = p[3].f;
	c->current_tex_coord.W = p[4].f;
}

void glopEdgeFlag(GLParam* p) {
	GLContext* c = gl_get_context();
	c->current_edge_flag = p[1].i;
}

void glopColor(GLParam* p) {
	GLContext* c = gl_get_context();
	c->current_color.X = p[1].f;
	c->current_color.Y = p[2].f;
	c->current_color.Z = p[3].f;
	c->current_color.W = p[4].f;

	if (c->color_material_enabled) {
		GLParam q[7];
		q[0].op = OP_Material;
		q[1].i = c->current_color_material_mode;
		q[2].i = c->current_color_material_type;
		q[3].f = p[1].f;
		q[4].f = p[2].f;
		q[5].f = p[3].f;
		q[6].f = p[4].f;
		glopMaterial(q);
	}
}

void glopBegin(GLParam* p) {
	GLint type;
	M4 tmp;
	GLContext* c = gl_get_context();
#if TGL_FEATURE_ERROR_CHECK == 1
	if (c->in_begin != 0)
#define ERROR_FLAG GL_INVALID_OPERATION
#include "error_check.h"
#else
	
#endif
		type = p[1].i;
	c->begin_type = type;
	c->in_begin = 1;
	c->vertex_n = 0;
	c->vertex_cnt = 0;

	if (c->matrix_model_projection_updated) {

		if (c->lighting_enabled) {
			/* precompute inverse modelview */
			gl_M4_Inv(&tmp, c->matrix_stack_ptr[0]);
			gl_M4_Transpose(&c->matrix_model_view_inv, &tmp);
		} else {
			GLfloat* m = &c->matrix_model_projection.m[0][0];
			/* precompute projection matrix */
			gl_M4_Mul(&c->matrix_model_projection, c->matrix_stack_ptr[1], c->matrix_stack_ptr[0]);
			/* test to accelerate computation */
			c->matrix_model_projection_no_w_transform = 0;
			if (m[12] == 0.0 && m[13] == 0.0 && m[14] == 0.0)
				/*
				 if(c->matrix_model_projection.m[3][0] == 0.0 &&
					c->matrix_model_projection.m[3][1] == 0.0 &&
					c->matrix_model_projection.m[3][2] == 0.0)
				*/
				c->matrix_model_projection_no_w_transform = 1;
		}

		/* test if the texture matrix is not Identity */
		c->apply_texture_matrix = !gl_M4_IsId(c->matrix_stack_ptr[2]);

		c->matrix_model_projection_updated = 0;
	}
	/*  viewport- this is now updated on a glViewport call. 
	if (c->viewport.updated) {
		gl_eval_viewport(c);
		c->viewport.updated = 0;
	}
	 triangle drawing functions 
	*/
#if TGL_FEATURE_ALT_RENDERMODES == 1
	if (c->render_mode == GL_SELECT) {
		c->draw_triangle_front = gl_draw_triangle_select;
		c->draw_triangle_back = gl_draw_triangle_select;
	} else if (c->render_mode == GL_FEEDBACK) {
		c->draw_triangle_front = gl_draw_triangle_feedback;
		c->draw_triangle_back = gl_draw_triangle_feedback;
	} else
#endif
	{
		switch (c->polygon_mode_front) {
		case GL_POINT:
			c->draw_triangle_front = gl_draw_triangle_point;
			break;
		case GL_LINE:
			c->draw_triangle_front = gl_draw_triangle_line;
			break;
		default:
			c->draw_triangle_front = gl_draw_triangle_fill;
			break;
		}

		switch (c->polygon_mode_back) {
		case GL_POINT:
			c->draw_triangle_back = gl_draw_triangle_point;
			break;
		case GL_LINE:
			c->draw_triangle_back = gl_draw_triangle_line;
			break;
		default:
			c->draw_triangle_back = gl_draw_triangle_fill;
			break;
		}
	}
}

static void gl_transform_to_viewport_vertex_c(GLVertex* v) {
	GLContext* c = gl_get_context();
	{
		GLfloat winv = 1.0 / v->pc.W;
		v->zp.x = (GLint)(v->pc.X * winv * c->viewport.scale.X + c->viewport.trans.X);
		v->zp.y = (GLint)(v->pc.Y * winv * c->viewport.scale.Y + c->viewport.trans.Y);
		v->zp.z = (GLint)(v->pc.Z * winv * c->viewport.scale.Z + c->viewport.trans.Z);
	}

	v->zp.r = (GLint)(v->color.v[0] * COLOR_CORRECTED_MULT_MASK + COLOR_MIN_MULT) & COLOR_MASK;
	v->zp.g = (GLint)(v->color.v[1] * COLOR_CORRECTED_MULT_MASK + COLOR_MIN_MULT) & COLOR_MASK;
	v->zp.b = (GLint)(v->color.v[2] * COLOR_CORRECTED_MULT_MASK + COLOR_MIN_MULT) & COLOR_MASK;

	if (c->texture_2d_enabled) {
		v->zp.s = (GLint)(v->tex_coord.X * (ZB_POINT_S_MAX - ZB_POINT_S_MIN) + ZB_POINT_S_MIN); 
		v->zp.t = (GLint)(v->tex_coord.Y * (ZB_POINT_T_MAX - ZB_POINT_T_MIN) + ZB_POINT_T_MIN); 
	}
}

static void gl_vertex_transform(GLVertex* v) {
	GLfloat* m;
	GLContext* c = gl_get_context();

	if (c->lighting_enabled)

	{
		/* eye coordinates needed for lighting */
		V4* n;
		m = &c->matrix_stack_ptr[0]->m[0][0];
		v->ec.X = (v->coord.X * m[0] + v->coord.Y * m[1] + v->coord.Z * m[2] + m[3]);
		v->ec.Y = (v->coord.X * m[4] + v->coord.Y * m[5] + v->coord.Z * m[6] + m[7]);
		v->ec.Z = (v->coord.X * m[8] + v->coord.Y * m[9] + v->coord.Z * m[10] + m[11]);
		v->ec.W = (v->coord.X * m[12] + v->coord.Y * m[13] + v->coord.Z * m[14] + m[15]);

		/* projection coordinates */
		m = &c->matrix_stack_ptr[1]->m[0][0];
		v->pc.X = (v->ec.X * m[0] + v->ec.Y * m[1] + v->ec.Z * m[2] + v->ec.W * m[3]);
		v->pc.Y = (v->ec.X * m[4] + v->ec.Y * m[5] + v->ec.Z * m[6] + v->ec.W * m[7]);
		v->pc.Z = (v->ec.X * m[8] + v->ec.Y * m[9] + v->ec.Z * m[10] + v->ec.W * m[11]);
		v->pc.W = (v->ec.X * m[12] + v->ec.Y * m[13] + v->ec.Z * m[14] + v->ec.W * m[15]);

		m = &c->matrix_model_view_inv.m[0][0];
		n = &c->current_normal;

		v->normal.X = (n->X * m[0] + n->Y * m[1] + n->Z * m[2]);
		v->normal.Y = (n->X * m[4] + n->Y * m[5] + n->Z * m[6]);
		v->normal.Z = (n->X * m[8] + n->Y * m[9] + n->Z * m[10]);

		if (c->normalize_enabled) {
			gl_V3_Norm_Fast(&v->normal);
		}
	}

	else {
		/* no eye coordinates needed, no normal */
		/* NOTE: W = 1 is assumed */
		m = &c->matrix_model_projection.m[0][0];

		v->pc.X = (v->coord.X * m[0] + v->coord.Y * m[1] + v->coord.Z * m[2] + m[3]);
		v->pc.Y = (v->coord.X * m[4] + v->coord.Y * m[5] + v->coord.Z * m[6] + m[7]);
		v->pc.Z = (v->coord.X * m[8] + v->coord.Y * m[9] + v->coord.Z * m[10] + m[11]);
		if (c->matrix_model_projection_no_w_transform) {
			v->pc.W = m[15];
		} else {
			v->pc.W = (v->coord.X * m[12] + v->coord.Y * m[13] + v->coord.Z * m[14] + m[15]);
		}
	}

	v->clip_code = gl_clipcode(v->pc.X, v->pc.Y, v->pc.Z, v->pc.W);
}

void glopVertex(GLParam* p) {
	GLVertex* v;
	GLint n, i, cnt;
	GLContext* c = gl_get_context();
#if TGL_FEATURE_ERROR_CHECK == 1
	if (c->in_begin == 0)
#define ERROR_FLAG GL_INVALID_OPERATION
#include "error_check.h"
#else
	
#endif

		n = c->vertex_n;
	cnt = c->vertex_cnt;
	cnt++;
	c->vertex_cnt = cnt;

	/* new vertex entry */
	v = &c->vertex[n];
	n++;

	v->coord.X = p[1].f;
	v->coord.Y = p[2].f;
	v->coord.Z = p[3].f;
	v->coord.W = p[4].f;

	gl_vertex_transform(v);

	/* color */

	if (c->lighting_enabled) {
		gl_shade_vertex(v);
#include "error_check.h"
		
	} else {
		v->color = c->current_color;
	}
	/* tex coords */
#if TGL_OPTIMIZATION_HINT_BRANCH_COST < 1
	if (c->texture_2d_enabled)
#endif
	{
		if (c->apply_texture_matrix) {
			gl_M4_MulV4(&v->tex_coord, c->matrix_stack_ptr[2], &c->current_tex_coord);
		} else {
			v->tex_coord = c->current_tex_coord;
		}
	}
	/* precompute the mapping to the viewport */
#if TGL_OPTIMIZATION_HINT_BRANCH_COST < 2
	if (v->clip_code == 0)
#endif
	{
		gl_transform_to_viewport_vertex_c(v);
	}

	/* edge flag */
	v->edge_flag = c->current_edge_flag;

	switch (c->begin_type) {
	case GL_POINTS:
		gl_draw_point(&c->vertex[0]);
		n = 0;
		break;

	case GL_LINES:
		if (n == 2) {
			gl_draw_line(&c->vertex[0], &c->vertex[1]);
			n = 0;
		}
		break;
	case GL_LINE_STRIP:
#if TGL_FEATURE_GL_POLYGON == 1
	case GL_LINE_LOOP:
#endif
		switch (n) {
		case 1: {
			c->vertex[2] = c->vertex[0];
		} break;
		case 2: {
			gl_draw_line(&c->vertex[0], &c->vertex[1]);
			c->vertex[0] = c->vertex[1];
			n = 1;
		} break;
		default:
			break;
		};
		break;
	case GL_TRIANGLES:
		if (n == 3) {
			gl_draw_triangle(&c->vertex[0], &c->vertex[1], &c->vertex[2]);
			n = 0;
		}
		break;
	case GL_TRIANGLE_STRIP:
		if (cnt >= 3) {
			if (n == 3)
				n = 0;
			/* needed to respect triangle orientation */
			switch (cnt & 1) {
			case 0:
				gl_draw_triangle(&c->vertex[2], &c->vertex[1], &c->vertex[0]);
				break;
			default:
			case 1:
				gl_draw_triangle(&c->vertex[0], &c->vertex[1], &c->vertex[2]);
				break;
			}
		}
		break;
	case GL_TRIANGLE_FAN:
		if (n == 3) {
			gl_draw_triangle(&c->vertex[0], &c->vertex[1], &c->vertex[2]);
			c->vertex[1] = c->vertex[2];
			n = 2;
		}
		break;

	case GL_QUADS:
		if (n == 4) {
			c->vertex[2].edge_flag = 0;
			gl_draw_triangle(&c->vertex[0], &c->vertex[1], &c->vertex[2]);
			c->vertex[2].edge_flag = 1;
			c->vertex[0].edge_flag = 0;
			gl_draw_triangle(&c->vertex[0], &c->vertex[2], &c->vertex[3]);
			n = 0;
		}
		break;

	case GL_QUAD_STRIP:
		if (n == 4) {
			gl_draw_triangle(&c->vertex[0], &c->vertex[1], &c->vertex[2]);
			gl_draw_triangle(&c->vertex[1], &c->vertex[3], &c->vertex[2]);
			for (i = 0; i < 2; i++)
				c->vertex[i] = c->vertex[i + 2];
			n = 2;
		}
		break;

#if TGL_FEATURE_GL_POLYGON == 1
	case GL_POLYGON:
		break;
#endif
#if TGL_FEATURE_ERROR_CHECK == 1
	default:
		gl_fatal_error("glBegin: type %x not handled\n", c->begin_type);
#else
	default:
		break;
#endif
	}

	c->vertex_n = n;
}

void glopEnd(GLParam* param) {
	GLContext* c = gl_get_context();
#if TGL_FEATURE_ERROR_CHECK == 1
	if (c->in_begin != 1)
#define ERROR_FLAG GL_INVALID_OPERATION
#include "error_check.h"
#else
	
	/* Assume it went alright.*/
#endif



#if TGL_FEATURE_GL_POLYGON == 1
		if (c->begin_type == GL_LINE_LOOP) {
			if (c->vertex_cnt >= 3) {
				gl_draw_line(&c->vertex[0], &c->vertex[2]);
			}
		} else if (c->begin_type == GL_POLYGON) {
			GLint i = c->vertex_cnt;
			while (i >= 3) {
				i--;
				gl_draw_triangle(&c->vertex[i], &c->vertex[0], &c->vertex[i - 1]);
			}
		}
#endif
	c->in_begin = 0;
}
