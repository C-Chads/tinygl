#include "msghandling.h"
#include "zgl.h"
/* fill triangle profile */
/* #define PROFILE */

#define CLIP_XMIN (1 << 0)
#define CLIP_XMAX (1 << 1)
#define CLIP_YMIN (1 << 2)
#define CLIP_YMAX (1 << 3)
#define CLIP_ZMIN (1 << 4)
#define CLIP_ZMAX (1 << 5)

static void gl_transform_to_viewport_clip_c(GLVertex* v) { /* MARK: NOT_INLINED_IN_OG*/
	GLContext* c = gl_get_context();
	/* coordinates */
	{
		GLfloat winv = 1.0 / v->pc.W;
		v->zp.x = (GLint)(v->pc.X * winv * c->viewport.scale.X + c->viewport.trans.X);
		v->zp.y = (GLint)(v->pc.Y * winv * c->viewport.scale.Y + c->viewport.trans.Y);
		v->zp.z = (GLint)(v->pc.Z * winv * c->viewport.scale.Z + c->viewport.trans.Z);
	}
	/* color */
	v->zp.r = (GLint)(v->color.v[0] * COLOR_CORRECTED_MULT_MASK + COLOR_MIN_MULT) & COLOR_MASK;
	v->zp.g = (GLint)(v->color.v[1] * COLOR_CORRECTED_MULT_MASK + COLOR_MIN_MULT) & COLOR_MASK;
	v->zp.b = (GLint)(v->color.v[2] * COLOR_CORRECTED_MULT_MASK + COLOR_MIN_MULT) & COLOR_MASK;

	/* texture */

	if (c->texture_2d_enabled) {
		v->zp.s = (GLint)(v->tex_coord.X * (ZB_POINT_S_MAX - ZB_POINT_S_MIN) + ZB_POINT_S_MIN); 
		v->zp.t = (GLint)(v->tex_coord.Y * (ZB_POINT_T_MAX - ZB_POINT_T_MIN) + ZB_POINT_T_MIN); 
	}
}



#define clip_funcdef(name, sign, dir, dir1, dir2)                                                                                                              \
	static GLfloat name(V4* c, V4* a, V4* b) {                                                                                                                 \
		GLfloat t, dX, dY, dZ, dW, den;                                                                                                                        \
		dX = (b->X - a->X);                                                                                                                                    \
		dY = (b->Y - a->Y);                                                                                                                                    \
		dZ = (b->Z - a->Z);                                                                                                                                    \
		dW = (b->W - a->W);                                                                                                                                    \
		den = -(sign d##dir) + dW;                                                                                                                             \
		if (den == 0)                                                                                                                                          \
			t = 0;                                                                                                                                             \
		else                                                                                                                                                   \
			t = (sign a->dir - a->W) / den;                                                                                                                    \
		c->dir1 = a->dir1 + t * d##dir1;                                                                                                                       \
		c->dir2 = a->dir2 + t * d##dir2;                                                                                                                       \
		c->W = a->W + t * dW;                                                                                                                                  \
		c->dir = sign c->W;                                                                                                                                    \
		return t;                                                                                                                                              \
	}
clip_funcdef(clip_xmin, -, X, Y, Z)

	clip_funcdef(clip_xmax, +, X, Y, Z)

		clip_funcdef(clip_ymin, -, Y, X, Z)

			clip_funcdef(clip_ymax, +, Y, X, Z)
			
				clip_funcdef(clip_zmin, -, Z, X, Y)

					clip_funcdef(clip_zmax, +, Z, X, Y)

static GLfloat (*clip_proc[6])(V4*, V4*, V4*) = {clip_xmin, clip_xmax, clip_ymin, clip_ymax, clip_zmin, clip_zmax};
/* point */
#if TGL_FEATURE_ALT_RENDERMODES == 1
static void gl_add_select1(GLint z1, GLint z2, GLint z3) {
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

	gl_add_select(0xffffffff - min, 0xffffffff - max);
}
#else
#define gl_add_select1(a, b, c) /*a comment*/
#endif
void gl_draw_point(GLVertex* p0) {
	GLContext* c = gl_get_context();
	if (p0->clip_code == 0) {
#if TGL_FEATURE_ALT_RENDERMODES == 1
		if (c->render_mode == GL_SELECT) {
			gl_add_select(p0->zp.z, p0->zp.z);

		} else if (c->render_mode == GL_FEEDBACK) {
			gl_add_feedback(GL_POINT_TOKEN, p0, NULL, NULL, 0);
		} else
#endif
		{
			ZB_plot(c->zb, &p0->zp);
		}
	}
}

/* line */

/*
 * Line Clipping
 */

static void GLinterpolate(GLVertex* q, GLVertex* p0, GLVertex* p1, GLfloat t) { 
	GLint i;
	q->pc.X = p0->pc.X + (p1->pc.X - p0->pc.X) * t;
	q->pc.Y = p0->pc.Y + (p1->pc.Y - p0->pc.Y) * t;
	q->pc.Z = p0->pc.Z + (p1->pc.Z - p0->pc.Z) * t;
	q->pc.W = p0->pc.W + (p1->pc.W - p0->pc.W) * t;
#ifdef _OPENMP
#pragma omp simd
#endif
	for (i = 0; i < 3; i++)
		q->color.v[i] = p0->color.v[i] + (p1->color.v[i] - p0->color.v[i]) * t;
}

/* Line Clipping algorithm from 'Computer Graphics', Principles and
   Practice */
static GLint ClipLine1(GLfloat denom, GLfloat num, GLfloat* tmin, GLfloat* tmax) {
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
void gl_draw_line(GLVertex* p1, GLVertex* p2) {
	GLContext* c = gl_get_context();
	GLfloat dx, dy, dz, dw, x1, y1, z1, w1;

	GLVertex q1, q2;
	GLint cc1, cc2;

	cc1 = p1->clip_code;
	cc2 = p2->clip_code;

	if ((cc1 | cc2) == 0) {
#if TGL_FEATURE_ALT_RENDERMODES == 1
		if (c->render_mode == GL_SELECT) {
			gl_add_select1(p1->zp.z, p2->zp.z, p2->zp.z);
		} else if (c->render_mode == GL_FEEDBACK) {
			gl_add_feedback(GL_LINE_TOKEN, p1, p2, NULL, 0);
		} else
#endif
		{
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
			gl_transform_to_viewport_clip_c(&q1);
			gl_transform_to_viewport_clip_c(&q2);
#if TGL_FEATURE_ALT_RENDERMODES == 1
			if (c->render_mode == GL_SELECT) {
				gl_add_select1(q1.zp.z, q2.zp.z, q2.zp.z);
			} else if (c->render_mode == GL_FEEDBACK) {
				gl_add_feedback(GL_LINE_TOKEN, &q1, &q2, NULL, 0);
			} else
#endif
			{
				if (c->zb->depth_test)
					ZB_line_z(c->zb, &q1.zp, &q2.zp);
				else
					ZB_line(c->zb, &q1.zp, &q2.zp);
			}
		}
	}
}



/*Triangles*/

static void updateTmp(GLVertex* q, GLVertex* p0, GLVertex* p1, GLfloat t) { 
	GLContext* c = gl_get_context();
	{

		q->color.v[0] = p0->color.v[0] + (p1->color.v[0] - p0->color.v[0]) * t;
		q->color.v[1] = p0->color.v[1] + (p1->color.v[1] - p0->color.v[1]) * t;
		q->color.v[2] = p0->color.v[2] + (p1->color.v[2] - p0->color.v[2]) * t;
	}

#if TGL_OPTIMIZATION_HINT_BRANCH_COST < 1
	if (c->texture_2d_enabled)
#endif
	{
		q->tex_coord.X = p0->tex_coord.X + (p1->tex_coord.X - p0->tex_coord.X) * t;
		q->tex_coord.Y = p0->tex_coord.Y + (p1->tex_coord.Y - p0->tex_coord.Y) * t;
	}

	q->clip_code = gl_clipcode(q->pc.X, q->pc.Y, q->pc.Z, q->pc.W);
	if (q->clip_code == 0)
		gl_transform_to_viewport_clip_c(q);
}

static void gl_draw_triangle_clip(GLVertex* p0, GLVertex* p1, GLVertex* p2, GLint clip_bit); 

void gl_draw_triangle(GLVertex* p0, GLVertex* p1, GLVertex* p2) {
	GLContext* c = gl_get_context();
	GLint co, cc[3], front;

	cc[0] = p0->clip_code;
	cc[1] = p1->clip_code;
	cc[2] = p2->clip_code;

	co = cc[0] | cc[1] | cc[2];

	/* we handle the non clipped case here to go faster */
	if (co == 0) {
		GLfloat norm;
		norm = (GLfloat)(p1->zp.x - p0->zp.x) * (GLfloat)(p2->zp.y - p0->zp.y) - (GLfloat)(p2->zp.x - p0->zp.x) * (GLfloat)(p1->zp.y - p0->zp.y);

		if (norm == 0) 
			return;

		front = norm < 0.0;
		front = front ^ c->current_front_face; 

		/* back face culling */
		if (c->cull_face_enabled) {
			/* most used case first */
			if (c->current_cull_face == GL_BACK) {
				if (front == 0)
					return;
				c->draw_triangle_front(p0, p1, p2);
			} else if (c->current_cull_face == GL_FRONT) {
				if (front != 0)
					return;
				c->draw_triangle_back(p0, p1, p2);
			} else {
				return;
			}
		} else {
			/* no culling */
			if (front) {
				c->draw_triangle_front(p0, p1, p2);
			} else {
				c->draw_triangle_back(p0, p1, p2);
			}
		}
	} else {
		/* GLint c_and = cc[0] & cc[1] & cc[2];*/
		if ((cc[0] & cc[1] & cc[2]) == 0) { /* Don't draw a triangle with no points*/
			gl_draw_triangle_clip(p0, p1, p2, 0);
		}
	}
}

static void gl_draw_triangle_clip(GLVertex* p0, GLVertex* p1, GLVertex* p2, GLint clip_bit) {
	
	GLint co, c_and, co1, cc[3], edge_flag_tmp, clip_mask;
	
	GLVertex* q[3];

	cc[0] = p0->clip_code;
	cc[1] = p1->clip_code;
	cc[2] = p2->clip_code;

	co = cc[0] | cc[1] | cc[2];
	if (co == 0) {
		gl_draw_triangle(p0, p1, p2);
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
		if (clip_bit == 6) { /* The 2 bit and the 4 bit.*/
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
			{
				GLVertex tmp1, tmp2;
				GLfloat tt;
				tt = clip_proc[clip_bit](&tmp1.pc, &q[0]->pc, &q[1]->pc);
				updateTmp(&tmp1, q[0], q[1], tt);

				tt = clip_proc[clip_bit](&tmp2.pc, &q[0]->pc, &q[2]->pc);
				updateTmp(&tmp2, q[0], q[2], tt);

				tmp1.edge_flag = q[0]->edge_flag;
				edge_flag_tmp = q[2]->edge_flag;
				q[2]->edge_flag = 0;
				gl_draw_triangle_clip(&tmp1, q[1], q[2], clip_bit + 1);

				tmp2.edge_flag = 1;
				tmp1.edge_flag = 0;
				q[2]->edge_flag = edge_flag_tmp;
				gl_draw_triangle_clip(&tmp2, &tmp1, q[2], clip_bit + 1);
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
			{
				GLVertex tmp1, tmp2;
				GLfloat tt;
				tt = clip_proc[clip_bit](&tmp1.pc, &q[0]->pc, &q[1]->pc);
				updateTmp(&tmp1, q[0], q[1], tt);

				tt = clip_proc[clip_bit](&tmp2.pc, &q[0]->pc, &q[2]->pc);
				updateTmp(&tmp2, q[0], q[2], tt);

				tmp1.edge_flag = 1;
				tmp2.edge_flag = q[2]->edge_flag;
				gl_draw_triangle_clip(q[0], &tmp1, &tmp2, clip_bit + 1);
			}
		}
	}
}

/* see vertex.c to see how the draw functions are assigned.*/
void gl_draw_triangle_select(GLVertex* p0, GLVertex* p1, GLVertex* p2) { gl_add_select1(p0->zp.z, p1->zp.z, p2->zp.z); }
void gl_draw_triangle_feedback(GLVertex* p0, GLVertex* p1, GLVertex* p2) { gl_add_feedback(GL_POLYGON_TOKEN, p0, p1, p2, 0); }

#ifdef PROFILE
int count_triangles, count_triangles_textured, count_pixels;
#warning "Compile with PROFILE slows down everything"
#endif

/* see vertex.c to see how the draw functions are assigned.*/
void gl_draw_triangle_fill(GLVertex* p0, GLVertex* p1, GLVertex* p2) { 
	GLContext* c = gl_get_context();
	if (c->texture_2d_enabled) {
		/* if(c->current_texture)*/
#if TGL_FEATURE_LIT_TEXTURES == 1
		if (c->current_shade_model != GL_SMOOTH) {
			p1->zp.r = p2->zp.r;
			p1->zp.g = p2->zp.g;
			p1->zp.b = p2->zp.b;

			p0->zp.r = p2->zp.r;
			p0->zp.g = p2->zp.g;
			p0->zp.b = p2->zp.b;
		}
#endif

		ZB_setTexture(c->zb, c->current_texture->images[0].pixmap);
#if TGL_FEATURE_BLEND == 1
		if (c->zb->enable_blend)
			ZB_fillTriangleMappingPerspective(c->zb, &p0->zp, &p1->zp, &p2->zp);
		else
			ZB_fillTriangleMappingPerspectiveNOBLEND(c->zb, &p0->zp, &p1->zp, &p2->zp);
#else
		ZB_fillTriangleMappingPerspectiveNOBLEND(c->zb, &p0->zp, &p1->zp, &p2->zp);
#endif
	} else if (c->current_shade_model == GL_SMOOTH) {
#if TGL_FEATURE_BLEND == 1
		if (c->zb->enable_blend)
			ZB_fillTriangleSmooth(c->zb, &p0->zp, &p1->zp, &p2->zp);
		else
			ZB_fillTriangleSmoothNOBLEND(c->zb, &p0->zp, &p1->zp, &p2->zp);
#else
		ZB_fillTriangleSmoothNOBLEND(c->zb, &p0->zp, &p1->zp, &p2->zp);
#endif
	} else {
#if TGL_FEATURE_BLEND == 1
		if (c->zb->enable_blend)
			ZB_fillTriangleFlat(c->zb, &p0->zp, &p1->zp, &p2->zp);
		else
			ZB_fillTriangleFlatNOBLEND(c->zb, &p0->zp, &p1->zp, &p2->zp);
#else
		ZB_fillTriangleFlatNOBLEND(c->zb, &p0->zp, &p1->zp, &p2->zp);
#endif
	}
}

/* Render a clipped triangle in line mode */

void gl_draw_triangle_line(GLVertex* p0, GLVertex* p1, GLVertex* p2) {
	GLContext* c = gl_get_context();
	if (c->zb->depth_test) {
		if (p0->edge_flag)
			ZB_line_z(c->zb, &p0->zp, &p1->zp);
		if (p1->edge_flag)
			ZB_line_z(c->zb, &p1->zp, &p2->zp);
		if (p2->edge_flag)
			ZB_line_z(c->zb, &p2->zp, &p0->zp);
	} else {
		if (p0->edge_flag)
			ZB_line(c->zb, &p0->zp, &p1->zp);
		if (p1->edge_flag)
			ZB_line(c->zb, &p1->zp, &p2->zp);
		if (p2->edge_flag)
			ZB_line(c->zb, &p2->zp, &p0->zp);
	}
}

/* Render a clipped triangle in point mode */
void gl_draw_triangle_point(GLVertex* p0, GLVertex* p1, GLVertex* p2) {
	GLContext* c = gl_get_context();
	if (p0->edge_flag)
		ZB_plot(c->zb, &p0->zp);
	if (p1->edge_flag)
		ZB_plot(c->zb, &p1->zp);
	if (p2->edge_flag)
		ZB_plot(c->zb, &p2->zp);
}
