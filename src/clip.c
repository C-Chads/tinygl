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



#define clip_funcdef(name, sign, dir, dir1, dir2)                                                                                                                 \
	GLfloat name(V4* c, V4* a, V4* b) {                                                                                                                 \
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
//MARK <POSSIBLE_PERF_BONUS>
clip_funcdef(clip_xmin, -, X, Y, Z)

	clip_funcdef(clip_xmax, +, X, Y, Z)

		clip_funcdef(clip_ymin, -, Y, X, Z)

			clip_funcdef(clip_ymax, +, Y, X, Z)
//MARK <POSSIBLE_PERF_BONUS>
				clip_funcdef(clip_zmin, -, Z, X, Y)

					clip_funcdef(clip_zmax, +, Z, X, Y)

GLfloat (*clip_proc[6])(V4*, V4*, V4*) = {clip_xmin, clip_xmax, clip_ymin, clip_ymax, clip_zmin, clip_zmax};
/* point */



/* line */
//Used only for lines.


/*
 * Line Clipping
 */

/* Line Clipping algorithm from 'Computer Graphics', Principles and
   Practice */






//see vertex.c to see how the draw functions are assigned.
void gl_draw_triangle_select(GLContext* c, GLVertex* p0, GLVertex* p1, GLVertex* p2) { 
	gl_add_select1(c, p0->zp.z, p1->zp.z, p2->zp.z); 
}
void gl_draw_triangle_feedback(GLContext* c, GLVertex* p0, GLVertex* p1, GLVertex* p2){
	gl_add_feedback(
					c,	GL_POLYGON_TOKEN,
					p0,
					p1,
					p2,
					0
				);
}

#ifdef PROFILE
int count_triangles, count_triangles_textured, count_pixels;
#warning "Compile with PROFILE slows down everything"
#endif

//see vertex.c to see how the draw functions are assigned.
void gl_draw_triangle_fill(GLContext* c, GLVertex* p0, GLVertex* p1, GLVertex* p2) { //Must be function pointer!

	if (c->texture_2d_enabled) {
		//if(c->current_texture)
#if TGL_FEATURE_LIT_TEXTURES == 1
		if(c->current_shade_model != GL_SMOOTH){
			p1->zp.r = p2->zp.r;
			p1->zp.g = p2->zp.g;
			p1->zp.b = p2->zp.b;

			p0->zp.r = p2->zp.r;
			p0->zp.g = p2->zp.g;
			p0->zp.b = p2->zp.b;
		}
#endif

//#ifdef PROFILE
//			count_triangles_textured++;
//#endif
			ZB_setTexture(c->zb, c->current_texture->images[0].pixmap);
#if TGL_FEATURE_BLEND == 1
			if(c->zb->enable_blend) 
				ZB_fillTriangleMappingPerspective(c->zb, &p0->zp, &p1->zp, &p2->zp);
			else 
				ZB_fillTriangleMappingPerspectiveNOBLEND(c->zb, &p0->zp, &p1->zp, &p2->zp);
#else
			ZB_fillTriangleMappingPerspectiveNOBLEND(c->zb, &p0->zp, &p1->zp, &p2->zp);
#endif
	} else if (c->current_shade_model == GL_SMOOTH) {
		//ZB_fillTriangleSmooth(c->zb, &p0->zp, &p1->zp, &p2->zp);
#if TGL_FEATURE_BLEND == 1
		if(c->zb->enable_blend) 
			ZB_fillTriangleSmooth(c->zb, &p0->zp, &p1->zp, &p2->zp);
		else 
			ZB_fillTriangleSmoothNOBLEND(c->zb, &p0->zp, &p1->zp, &p2->zp);
#else
		ZB_fillTriangleSmoothNOBLEND(c->zb, &p0->zp, &p1->zp, &p2->zp);
#endif
	} else {
		//ZB_fillTriangleFlat(c->zb, &p0->zp, &p1->zp, &p2->zp);
#if TGL_FEATURE_BLEND == 1
		if(c->zb->enable_blend) 
			ZB_fillTriangleFlat(c->zb, &p0->zp, &p1->zp, &p2->zp);
		else 
			ZB_fillTriangleFlatNOBLEND(c->zb, &p0->zp, &p1->zp, &p2->zp);
#else
		ZB_fillTriangleFlatNOBLEND(c->zb, &p0->zp, &p1->zp, &p2->zp);
#endif
	}
}

/* Render a clipped triangle in line mode */

void gl_draw_triangle_line(GLContext* c, GLVertex* p0, GLVertex* p1, GLVertex* p2) {
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
void gl_draw_triangle_point(GLContext* c, GLVertex* p0, GLVertex* p1, GLVertex* p2) {
	if (p0->edge_flag)
		ZB_plot(c->zb, &p0->zp);
	if (p1->edge_flag)
		ZB_plot(c->zb, &p1->zp);
	if (p2->edge_flag)
		ZB_plot(c->zb, &p2->zp);
}
