#include "../include/zbuffer.h"
#include "msghandling.h"
#include <stdlib.h>

#if TGL_FEATURE_RENDER_BITS == 32
#elif TGL_FEATURE_RENDER_BITS == 16
#else
#error "WRONG MODE!!!"
#endif

#if TGL_FEATURE_POLYGON_STIPPLE

#define TGL_STIPPLEVARS GLubyte* zbstipplepattern = zb->stipplepattern; GLubyte zbdostipple = zbdostipple;
#define THE_X (((GLushort)(pp - pp1)))
#define XSTIP(_a) ((THE_X + _a) & TGL_POLYGON_STIPPLE_MASK_X)
#define YSTIP (the_y & TGL_POLYGON_STIPPLE_MASK_Y)
// NOTES                                                           Divide by 8 to get the byte        Get the actual bit
#define STIPBIT(_a) (zbstipplepattern[(XSTIP(_a) | (YSTIP << TGL_POLYGON_STIPPLE_POW2_WIDTH)) >> 3] & (1 << (XSTIP(_a) & 7)))
#define STIPTEST(_a) !(zbdostipple && !STIPBIT(_a))

#else

#define TGL_STIPPLEVARS /* a comment */
#define STIPTEST(_a) (1)
//#define ZCMP(z,zpix,_a) ((z) >= (zpix))

#endif

#if TGL_FEATURE_NO_DRAW_COLOR == 1
#define NODRAWTEST(c) ((c & TGL_COLOR_MASK) != TGL_NO_DRAW_COLOR)
#else
#define NODRAWTEST(c) (1)
#endif

#define ZCMP(z, zpix, _a, c) ( ((!zbdt) || (z) >= (zpix)) && STIPTEST(_a) && NODRAWTEST(c))
#define ZCMPSIMP(z, zpix, _a, c) ( ((!zbdt) || (z) >= (zpix)) && STIPTEST(_a))

void ZB_fillTriangleFlat(ZBuffer* zb, ZBufferPoint* p0, ZBufferPoint* p1, ZBufferPoint* p2) {

	PIXEL color = RGB_TO_PIXEL(p2->r, p2->g, p2->b); GLubyte zbdw = zb->depth_write; GLubyte zbdt = zb->depth_test;
	TGL_BLEND_VARS
	TGL_STIPPLEVARS

#undef INTERP_Z
#undef INTERP_RGB
#undef INTERP_ST
#undef INTERP_STZ

#define INTERP_Z
//#define INTERP_RGB

#define DRAW_INIT()                                                                                                                                            \
	{  }

#define PUT_PIXEL(_a)                                                                                                                                          \
	{                                                                                                                                                          \
		zz = z >> ZB_POINT_Z_FRAC_BITS;                                                                                                                        \
		if (ZCMPSIMP(zz, pz[_a], _a, color)) {                                                                                                                 \
			TGL_BLEND_FUNC(color, (pp[_a])) /*pp[_a] = color;*/                                                                                                \
			if(zbdw)pz[_a] = zz;                                                                                                                                       \
		}                                                                                                                                                      \
		z += dzdx;                                                                                                                                             \
	}

#include "ztriangle.h"
}

void ZB_fillTriangleFlatNOBLEND(ZBuffer* zb, ZBufferPoint* p0, ZBufferPoint* p1, ZBufferPoint* p2) {

	PIXEL color = RGB_TO_PIXEL(p2->r, p2->g, p2->b); GLubyte zbdw = zb->depth_write; GLubyte zbdt = zb->depth_test;
	TGL_STIPPLEVARS
#undef INTERP_Z
#undef INTERP_RGB
#undef INTERP_ST
#undef INTERP_STZ

#define INTERP_Z
//#define INTERP_RGB
#define DRAW_INIT()                                                                                                                                            \
	{  }

#define PUT_PIXEL(_a)                                                                                                                                          \
	{                                                                                                                                                          \
		zz = z >> ZB_POINT_Z_FRAC_BITS;                                                                                                                        \
		if (ZCMPSIMP(zz, pz[_a], _a, 0)) {                                                                                                                 		\
			pp[_a] = color;                                                                                                										\
			if(zbdw)pz[_a] = zz;                                                                                            									\
		}                                                                                                                                                      \
		z += dzdx;                                                                                                                                             \
	}

#include "ztriangle.h"
}

/*
 * Smooth filled triangle.
 * The code below is very tricky :)
 */

void ZB_fillTriangleSmooth(ZBuffer* zb, ZBufferPoint* p0, ZBufferPoint* p1, ZBufferPoint* p2) {
#if TGL_FEATURE_NO_DRAW_COLOR == 1
	PIXEL c;
#endif
	GLubyte zbdw = zb->depth_write; GLubyte zbdt = zb->depth_test;
	TGL_BLEND_VARS
	TGL_STIPPLEVARS
// GLuint color;
#define INTERP_Z
#define INTERP_RGB

#define SAR_RND_TO_ZERO(v, n) (v / (1 << n))

#if TGL_FEATURE_RENDER_BITS == 32
#define DRAW_INIT()                                                                                                                                            \
	{}

#if TGL_FEATURE_NO_DRAW_COLOR != 1
#define PUT_PIXEL(_a)                                                                                                                                          \
	{                                                                                                                                                          \
		zz = z >> ZB_POINT_Z_FRAC_BITS;                                                                                                                        \
		if (ZCMPSIMP(zz, pz[_a], _a, 0)) {                                                                                                                     \
			/*pp[_a] = RGB_TO_PIXEL(or1, og1, ob1);*/                                                                                                          \
			TGL_BLEND_FUNC_RGB(or1, og1, ob1, (pp[_a]));                                                                                                       \
			if(zbdw)pz[_a] = zz;                                                                                                                               \
		}                                                                                                                                                      \
		z += dzdx;                                                                                                                                             \
		og1 += dgdx;                                                                                                                                           \
		or1 += drdx;                                                                                                                                           \
		ob1 += dbdx;                                                                                                                                           \
	}
#else
#define PUT_PIXEL(_a)                                                                                                                                          \
	{                                                                                                                                                          \
		zz = z >> ZB_POINT_Z_FRAC_BITS;                                                                                                                        \
		/*c = RGB_TO_PIXEL(or1, og1, ob1);*/                                                                                                                   \
		if (ZCMPSIMP(zz, pz[_a], _a, 0)) {                                                                                                                     \
			/*pp[_a] = c;*/                                                                                                                                    \
			TGL_BLEND_FUNC_RGB(or1, og1, ob1, (pp[_a]));                                                                                                       \
			if(zbdw)pz[_a] = zz;                                                                                                                                       \
		}                                                                                                                                                      \
		z += dzdx;                                                                                                                                             \
		og1 += dgdx;                                                                                                                                           \
		or1 += drdx;                                                                                                                                           \
		ob1 += dbdx;                                                                                                                                           \
	}
#endif
// END OF 32 bit mode
#elif TGL_FEATURE_RENDER_BITS == 16

#define DRAW_INIT()                                                                                                                                            \
	{}

	/*
	#define PUT_PIXEL(_a)				\
	{						\
		zz=z >> ZB_POINT_Z_FRAC_BITS;		\
		if (ZCMP(zz,pz[_a],_a)) {				\
		  pp[_a] = RGB_TO_PIXEL(or1, og1, ob1);\
		  pz[_a]=zz;				\
		}\
		z+=dzdx;					\
		og1+=dgdx;					\
		or1+=drdx;					\
		ob1+=dbdx;					\
	}
	*/

#if TGL_FEATURE_NO_DRAW_COLOR != 1
#define PUT_PIXEL(_a)                                                                                                                                          \
	{                                                                                                                                                          \
		zz = z >> ZB_POINT_Z_FRAC_BITS;                                                                                                                        \
		if (ZCMPSIMP(zz, pz[_a], _a, 0)) {                                                                                                                     \
			/*pp[_a] = RGB_TO_PIXEL(or1, og1, ob1);*/                                                                                                          \
			TGL_BLEND_FUNC_RGB(or1, og1, ob1, (pp[_a]));                                                                                                       \
			                                                                                                                                                   \
			if(zbdw)pz[_a] = zz;                                                                                                                   				\
		}                                                                                                                                                      \
		z += dzdx;                                                                                                                                             \
		og1 += dgdx;                                                                                                                                           \
		or1 += drdx;                                                                                                                                           \
		ob1 += dbdx;                                                                                                                                           \
	}
#else
#define PUT_PIXEL(_a)                                                                                                                                          \
	{                                                                                                                                                          \
		zz = z >> ZB_POINT_Z_FRAC_BITS;                                                                                                                        \
		/*c = RGB_TO_PIXEL(or1, og1, ob1);*/                                                                                                                   \
		if (ZCMPSIMP(zz, pz[_a], _a, c)) {                                                                                                                     \
			/*pp[_a] = c;*/                                                                                                                                    \
			TGL_BLEND_FUNC_RGB(or1, og1, ob1, (pp[_a]));                                                                                                       \
			if(zbdw) pz[_a] = zz;                                                                                                                   \
		}                                                                                                                                                      \
		z += dzdx;                                                                                                                                             \
		og1 += dgdx;                                                                                                                                           \
		or1 += drdx;                                                                                                                                           \
		ob1 += dbdx;                                                                                                                                           \
	}
#endif

#endif
//^ End of 16 bit mode stuff
#include "ztriangle.h"
} // EOF smooth fill triangle





void ZB_fillTriangleSmoothNOBLEND(ZBuffer* zb, ZBufferPoint* p0, ZBufferPoint* p1, ZBufferPoint* p2) {
#if TGL_FEATURE_NO_DRAW_COLOR == 1
	PIXEL c;
#endif
	GLubyte zbdw = zb->depth_write; GLubyte zbdt = zb->depth_test;
	TGL_STIPPLEVARS
// GLuint color;
#define INTERP_Z
#define INTERP_RGB

#define SAR_RND_TO_ZERO(v, n) (v / (1 << n))

#if TGL_FEATURE_RENDER_BITS == 32
#define DRAW_INIT()                                                                                                                                            \
	{}

#if TGL_FEATURE_NO_DRAW_COLOR != 1
#define PUT_PIXEL(_a)                                                                                                                                          \
	{                                                                                                                                                          \
		zz = z >> ZB_POINT_Z_FRAC_BITS;                                                                                                                        \
		if (ZCMPSIMP(zz, pz[_a], _a, 0)) {                                                                                                                     \
			pp[_a] = RGB_TO_PIXEL(or1, og1, ob1);                                                                                                          		\
			if(zbdw)pz[_a] = zz;                                                                                                                               \
		}                                                                                                                                                      \
		z += dzdx;                                                                                                                                             \
		og1 += dgdx;                                                                                                                                           \
		or1 += drdx;                                                                                                                                           \
		ob1 += dbdx;                                                                                                                                           \
	}
#else
#define PUT_PIXEL(_a)                                                                                                                                          \
	{                                                                                                                                                          \
		zz = z >> ZB_POINT_Z_FRAC_BITS;                                                                                                                        \
		/*c = RGB_TO_PIXEL(or1, og1, ob1);*/                                                                                                                   \
		if (ZCMPSIMP(zz, pz[_a], _a, 0)) {                                                                                                                     \
			pp[_a] = RGB_TO_PIXEL(or1,og1,ob1);                                                                                                            		\
			if(zbdw)pz[_a] = zz;                                                                                                                               \
		}                                                                                                                                                      \
		z += dzdx;                                                                                                                                             \
		og1 += dgdx;                                                                                                                                           \
		or1 += drdx;                                                                                                                                           \
		ob1 += dbdx;                                                                                                                                           \
	}
#endif
// END OF 32 bit mode
#elif TGL_FEATURE_RENDER_BITS == 16

#define DRAW_INIT()                                                                                                                                            \
	{}

	/*
	#define PUT_PIXEL(_a)				\
	{						\
		zz=z >> ZB_POINT_Z_FRAC_BITS;		\
		if (ZCMP(zz,pz[_a],_a)) {				\
		  pp[_a] = RGB_TO_PIXEL(or1, og1, ob1);\
		  pz[_a]=zz;				\
		}\
		z+=dzdx;					\
		og1+=dgdx;					\
		or1+=drdx;					\
		ob1+=dbdx;					\
	}
	*/

#if TGL_FEATURE_NO_DRAW_COLOR != 1
#define PUT_PIXEL(_a)                                                                                                                                          \
	{                                                                                                                                                          \
		zz = z >> ZB_POINT_Z_FRAC_BITS;                                                                                                                        \
		if (ZCMPSIMP(zz, pz[_a], _a, 0)) {                                                                                                                     \
			pp[_a] = RGB_TO_PIXEL(or1, og1, ob1);                                                                                                          \
			                                                                                                                                                   \
			if(zbdw)pz[_a] = zz;                                                                                                                   				\
		}                                                                                                                                                      \
		z += dzdx;                                                                                                                                             \
		og1 += dgdx;                                                                                                                                           \
		or1 += drdx;                                                                                                                                           \
		ob1 += dbdx;                                                                                                                                           \
	}
#else
#define PUT_PIXEL(_a)                                                                                                                                          \
	{                                                                                                                                                          \
		zz = z >> ZB_POINT_Z_FRAC_BITS;                                                                                                                        \
		/*c = RGB_TO_PIXEL(or1, og1, ob1);*/                                                                                                                   \
		if (ZCMPSIMP(zz, pz[_a], _a, c)) {                                                                                                                     \
			/*pp[_a] = c;*/                                                                                                                                    \
			pp[_a] = RGB_TO_PIXEL(or1, og1, ob1);                                                                                                       \
			if(zbdw) pz[_a] = zz;                                                                                                                   \
		}                                                                                                                                                      \
		z += dzdx;                                                                                                                                             \
		og1 += dgdx;                                                                                                                                           \
		or1 += drdx;                                                                                                                                           \
		ob1 += dbdx;                                                                                                                                           \
	}
#endif

#endif
//^ End of 16 bit mode stuff
#include "ztriangle.h"
} // EOF smooth fill triangle









//
//
//			TEXTURE MAPPED TRIANGLES
//               Section_Header
//
//
//
//

void ZB_setTexture(ZBuffer* zb, PIXEL* texture) { zb->current_texture = texture; }
// Ignore this it is never used
/*
void ZB_fillTriangleMapping(ZBuffer* zb, ZBufferPoint* p0, ZBufferPoint* p1, ZBufferPoint* p2) {
	PIXEL* texture;

//#if TGL_FEATURE_NO_DRAW_COLOR == 1
	PIXEL c;
//#endif
//#define INTERP_Z
//#define INTERP_ST
//#define INTERP_RGB
//#define DRAW_INIT() \ { texture = zb->current_texture; }

//#if TGL_FEATURE_NO_DRAW_COLOR != 1
//#define PUT_PIXEL(_a) \
	{                                                                                                                                                          \
		zz = z >> ZB_POINT_Z_FRAC_BITS;                                                                                                                        \
		if (ZCMP(zz, pz[_a], _a, 0)) {                                                                                                                         \
			pp[_a] = texture[((t & 0x3FC00000) | s) >> 14];                                                                                                    \
			pz[_a] = zz;                                                                                                                                       \
		}                                                                                                                                                      \
		z += dzdx;                                                                                                                                             \
		s += dsdx;                                                                                                                                             \
		t += dtdx;                                                                                                                                             \
	}
//#else
//#define PUT_PIXEL(_a) \
	{                                                                                                                                                          \
		zz = z >> ZB_POINT_Z_FRAC_BITS;                                                                                                                        \
		c = texture[((t & 0x3FC00000) | s) >> 14];                                                                                                             \
		if (ZCMP(zz, pz[_a], _a, c)) {                                                                                                                         \
			pp[_a] = c;                                                                                                                                        \
			pz[_a] = zz;                                                                                                                                       \
		}                                                                                                                                                      \
		z += dzdx;                                                                                                                                             \
		s += dsdx;                                                                                                                                             \
		t += dtdx;                                                                                                                                             \
	}
//#endif
//#include "ztriangle.h"
}
*/
/*
 * Texture mapping with perspective correction.
 * We use the gradient method to make less divisions.
 * TODO: pipeline the division
 */

#if 1 // IF 1

void ZB_fillTriangleMappingPerspective(ZBuffer* zb, ZBufferPoint* p0, ZBufferPoint* p1, ZBufferPoint* p2) {
	PIXEL* texture;
	GLfloat fdzdx, fndzdx, ndszdx, ndtzdx;
	GLubyte zbdw = zb->depth_write; GLubyte zbdt = zb->depth_test;
	TGL_BLEND_VARS
	TGL_STIPPLEVARS
#if TGL_FEATURE_NO_DRAW_COLOR == 1
	PIXEL c;
#endif
#define INTERP_Z
#define INTERP_STZ
#define INTERP_RGB

#define NB_INTERP 8

#define DRAW_INIT()                                                                                                                                            \
	{                                                                                                                                                          \
		texture = zb->current_texture;                                                                                                                         \
		fdzdx = (GLfloat)dzdx;                                                                                                                                 \
		fndzdx = NB_INTERP * fdzdx;                                                                                                                            \
		ndszdx = NB_INTERP * dszdx;                                                                                                                            \
		ndtzdx = NB_INTERP * dtzdx;                                                                                                                            \
	}
#if TGL_FEATURE_LIT_TEXTURES == 1
#define OR1OG1OB1DECL                                                                                                                                          \
	register GLuint or1, og1, ob1;                                                                                                                             \
	or1 = r1;                                                                                                                                                  \
	og1 = g1;                                                                                                                                                  \
	ob1 = b1;
#define OR1G1B1INCR                                                                                                                                            \
	og1 += dgdx;                                                                                                                                               \
	or1 += drdx;                                                                                                                                               \
	ob1 += dbdx;
#else
#define OR1OG1OB1DECL /*A comment*/
#define OR1G1B1INCR   /*Another comment*/
#define or1 0xffff
#define og1 0xffff
#define ob1 0xffff
#endif
#if TGL_FEATURE_NO_DRAW_COLOR != 1
#define PUT_PIXEL(_a)                                                                                                                                          \
	{                                                                                                                                                          \
		zz = z >> ZB_POINT_Z_FRAC_BITS;                                                                                                                        \
		if (ZCMP(zz, pz[_a], _a, 0)) {                                                                                                                         \
			/*pp[_a] = RGB_MIX_FUNC(or1, og1, ob1, *(PIXEL*)((GLbyte*)texture + (((t & 0x3FC00000) | (s & 0x003FC000)) >> (17 - PSZSH))));*/                   \
			TGL_BLEND_FUNC(RGB_MIX_FUNC(or1, og1, ob1, *(PIXEL*)((GLbyte*)texture + (((t & 0x3FC00000) | (s & 0x003FC000)) >> (17 - PSZSH)))), pp[_a])         \
			if(zbdw) pz[_a] = zz;                                                                                                                   \
		}                                                                                                                                                      \
		z += dzdx;                                                                                                                                             \
		s += dsdx;                                                                                                                                             \
		t += dtdx;                                                                                                                                             \
		OR1G1B1INCR                                                                                                                                            \
	}
#else
#define PUT_PIXEL(_a)                                                                                                                                          \
	{                                                                                                                                                          \
		zz = z >> ZB_POINT_Z_FRAC_BITS;                                                                                                                        \
		c = *(PIXEL*)((GLbyte*)texture + (((t & 0x3FC00000) | (s & 0x003FC000)) >> (17 - PSZSH)));                                                             \
		if (ZCMP(zz, pz[_a], _a, c)) {                                                                                                                         \
			/*pp[_a] = RGB_MIX_FUNC(or1, og1, ob1, c);*/                                                                                                       \
			TGL_BLEND_FUNC(RGB_MIX_FUNC(or1, og1, ob1, c), (pp[_a]));                                                                                          \
			if(zbdw) pz[_a] = zz;                                                                                                                   			\
		}                                                                                                                                                      \
		z += dzdx;                                                                                                                                             \
		s += dsdx;                                                                                                                                             \
		t += dtdx;                                                                                                                                             \
		OR1G1B1INCR                                                                                                                                            \
	}
#endif

#define DRAW_LINE()                                                                                                                                            \
	{                                                                                                                                                          \
		register GLushort* pz;                                                                                                                                 \
		register PIXEL* pp;                                                                                                                                    \
		register GLuint s, t, z, zz;                                                                                                                           \
		register GLint n, dsdx, dtdx;                                                                                                                          \
		OR1OG1OB1DECL                                                                                                                                          \
		GLfloat sz, tz, fz, zinv;                                                                                                                              \
		n = (x2 >> 16) - x1;                                                                                                                                   \
		fz = (GLfloat)z1;                                                                                                                                      \
		zinv = 1.0 / fz;                                                                                                                                       \
		pp = (PIXEL*)((GLbyte*)pp1 + x1 * PSZB);                                                                                                               \
		pz = pz1 + x1;                                                                                                                                         \
		z = z1;                                                                                                                                                \
		sz = sz1;                                                                                                                                              \
		tz = tz1;                                                                                                                                              \
		while (n >= (NB_INTERP - 1)) {                                                                                                                         \
			{                                                                                                                                                  \
				GLfloat ss, tt;                                                                                                                                \
				ss = (sz * zinv);                                                                                                                              \
				tt = (tz * zinv);                                                                                                                              \
				s = (GLint)ss;                                                                                                                                 \
				t = (GLint)tt;                                                                                                                                 \
				dsdx = (GLint)((dszdx - ss * fdzdx) * zinv);                                                                                                   \
				dtdx = (GLint)((dtzdx - tt * fdzdx) * zinv);                                                                                                   \
				fz += fndzdx;                                                                                                                                  \
				zinv = 1.0 / fz;                                                                                                                               \
			}                                                                                                                                                  \
			PUT_PIXEL(0); /*the_x++;*/                                                                                                                         \
			PUT_PIXEL(1); /*the_x++;*/                                                                                                                         \
			PUT_PIXEL(2); /*the_x++;*/                                                                                                                         \
			PUT_PIXEL(3); /*the_x++;*/                                                                                                                         \
			PUT_PIXEL(4); /*the_x++;*/                                                                                                                         \
			PUT_PIXEL(5); /*the_x++;*/                                                                                                                         \
			PUT_PIXEL(6); /*the_x++;*/                                                                                                                         \
			PUT_PIXEL(7); /*the_x-=7;*/                                                                                                                        \
			pz += NB_INTERP;                                                                                                                                   \
			pp = (PIXEL*)((GLbyte*)pp + NB_INTERP * PSZB); /*the_x+=NB_INTERP * PSZB;*/                                                                        \
			n -= NB_INTERP;                                                                                                                                    \
			sz += ndszdx;                                                                                                                                      \
			tz += ndtzdx;                                                                                                                                      \
		}                                                                                                                                                      \
		{                                                                                                                                                      \
			GLfloat ss, tt;                                                                                                                                    \
			ss = (sz * zinv);                                                                                                                                  \
			tt = (tz * zinv);                                                                                                                                  \
			s = (GLint)ss;                                                                                                                                     \
			t = (GLint)tt;                                                                                                                                     \
			dsdx = (GLint)((dszdx - ss * fdzdx) * zinv);                                                                                                       \
			dtdx = (GLint)((dtzdx - tt * fdzdx) * zinv);                                                                                                       \
		}                                                                                                                                                      \
		while (n >= 0) {                                                                                                                                       \
			PUT_PIXEL(0); /*the_x += PSZB;*/                                                                                                                   \
			pz += 1;                                                                                                                                           \
			pp = (PIXEL*)((GLbyte*)pp + PSZB);                                                                                                                 \
			n -= 1;                                                                                                                                            \
		}                                                                                                                                                      \
	}

#include "ztriangle.h"
}







void ZB_fillTriangleMappingPerspectiveNOBLEND(ZBuffer* zb, ZBufferPoint* p0, ZBufferPoint* p1, ZBufferPoint* p2) {
	PIXEL* texture;
	GLfloat fdzdx, fndzdx, ndszdx, ndtzdx;
	GLubyte zbdw = zb->depth_write; GLubyte zbdt = zb->depth_test;
	TGL_STIPPLEVARS
#if TGL_FEATURE_NO_DRAW_COLOR == 1
	PIXEL c;
#endif
#define INTERP_Z
#define INTERP_STZ
#define INTERP_RGB

#define NB_INTERP 8

#define DRAW_INIT()                                                                                                                                            \
	{                                                                                                                                                          \
		texture = zb->current_texture;                                                                                                                         \
		fdzdx = (GLfloat)dzdx;                                                                                                                                 \
		fndzdx = NB_INTERP * fdzdx;                                                                                                                            \
		ndszdx = NB_INTERP * dszdx;                                                                                                                            \
		ndtzdx = NB_INTERP * dtzdx;                                                                                                                            \
	}
#if TGL_FEATURE_LIT_TEXTURES == 1
#define OR1OG1OB1DECL                                                                                                                                          \
	register GLuint or1, og1, ob1;                                                                                                                             \
	or1 = r1;                                                                                                                                                  \
	og1 = g1;                                                                                                                                                  \
	ob1 = b1;
#define OR1G1B1INCR                                                                                                                                            \
	og1 += dgdx;                                                                                                                                               \
	or1 += drdx;                                                                                                                                               \
	ob1 += dbdx;
#else
#define OR1OG1OB1DECL /*A comment*/
#define OR1G1B1INCR   /*Another comment*/
#define or1 0xffff
#define og1 0xffff
#define ob1 0xffff
#endif
#if TGL_FEATURE_NO_DRAW_COLOR != 1
#define PUT_PIXEL(_a)                                                                                                                                          \
	{                                                                                                                                                          \
		zz = z >> ZB_POINT_Z_FRAC_BITS;                                                                                                                        \
		if (ZCMP(zz, pz[_a], _a, 0)) {                                                                                                                         \
			pp[_a] = RGB_MIX_FUNC(or1, og1, ob1, *(PIXEL*)((GLbyte*)texture + (((t & 0x3FC00000) | (s & 0x003FC000)) >> (17 - PSZSH))));                   		\
			if(zbdw) pz[_a] = zz;                                                                                                                   			\
		}                                                                                                                                                      \
		z += dzdx;                                                                                                                                             \
		s += dsdx;                                                                                                                                             \
		t += dtdx;                                                                                                                                             \
		OR1G1B1INCR                                                                                                                                            \
	}
#else
#define PUT_PIXEL(_a)                                                                                                                                          \
	{                                                                                                                                                          \
		zz = z >> ZB_POINT_Z_FRAC_BITS;                                                                                                                        \
		c = *(PIXEL*)((GLbyte*)texture + (((t & 0x3FC00000) | (s & 0x003FC000)) >> (17 - PSZSH)));                                                             \
		if (ZCMP(zz, pz[_a], _a, c)) {                                                                                                                         \
			pp[_a] = RGB_MIX_FUNC(or1, og1, ob1, c);                                                                                                       		\
			/*TGL_BLEND_FUNC(RGB_MIX_FUNC(or1, og1, ob1, c), (pp[_a]));*/                                                                                          \
			if(zbdw) pz[_a] = zz;                                                                                                                   			\
		}                                                                                                                                                      \
		z += dzdx;                                                                                                                                             \
		s += dsdx;                                                                                                                                             \
		t += dtdx;                                                                                                                                             \
		OR1G1B1INCR                                                                                                                                            \
	}
#endif

#define DRAW_LINE()                                                                                                                                            \
	{                                                                                                                                                          \
		register GLushort* pz;                                                                                                                                 \
		register PIXEL* pp;                                                                                                                                    \
		register GLuint s, t, z, zz;                                                                                                                           \
		register GLint n, dsdx, dtdx;                                                                                                                          \
		OR1OG1OB1DECL                                                                                                                                          \
		GLfloat sz, tz, fz, zinv;                                                                                                                              \
		n = (x2 >> 16) - x1;                                                                                                                                   \
		fz = (GLfloat)z1;                                                                                                                                      \
		zinv = 1.0 / fz;                                                                                                                                       \
		pp = (PIXEL*)((GLbyte*)pp1 + x1 * PSZB);                                                                                                               \
		pz = pz1 + x1;                                                                                                                                         \
		z = z1;                                                                                                                                                \
		sz = sz1;                                                                                                                                              \
		tz = tz1;                                                                                                                                              \
		while (n >= (NB_INTERP - 1)) {                                                                                                                         \
			{                                                                                                                                                  \
				GLfloat ss, tt;                                                                                                                                \
				ss = (sz * zinv);                                                                                                                              \
				tt = (tz * zinv);                                                                                                                              \
				s = (GLint)ss;                                                                                                                                 \
				t = (GLint)tt;                                                                                                                                 \
				dsdx = (GLint)((dszdx - ss * fdzdx) * zinv);                                                                                                   \
				dtdx = (GLint)((dtzdx - tt * fdzdx) * zinv);                                                                                                   \
				fz += fndzdx;                                                                                                                                  \
				zinv = 1.0 / fz;                                                                                                                               \
			}                                                                                                                                                  \
			PUT_PIXEL(0); /*the_x++;*/                                                                                                                         \
			PUT_PIXEL(1); /*the_x++;*/                                                                                                                         \
			PUT_PIXEL(2); /*the_x++;*/                                                                                                                         \
			PUT_PIXEL(3); /*the_x++;*/                                                                                                                         \
			PUT_PIXEL(4); /*the_x++;*/                                                                                                                         \
			PUT_PIXEL(5); /*the_x++;*/                                                                                                                         \
			PUT_PIXEL(6); /*the_x++;*/                                                                                                                         \
			PUT_PIXEL(7); /*the_x-=7;*/                                                                                                                        \
			pz += NB_INTERP;                                                                                                                                   \
			pp = (PIXEL*)((GLbyte*)pp + NB_INTERP * PSZB); /*the_x+=NB_INTERP * PSZB;*/                                                                        \
			n -= NB_INTERP;                                                                                                                                    \
			sz += ndszdx;                                                                                                                                      \
			tz += ndtzdx;                                                                                                                                      \
		}                                                                                                                                                      \
		{                                                                                                                                                      \
			GLfloat ss, tt;                                                                                                                                    \
			ss = (sz * zinv);                                                                                                                                  \
			tt = (tz * zinv);                                                                                                                                  \
			s = (GLint)ss;                                                                                                                                     \
			t = (GLint)tt;                                                                                                                                     \
			dsdx = (GLint)((dszdx - ss * fdzdx) * zinv);                                                                                                       \
			dtdx = (GLint)((dtzdx - tt * fdzdx) * zinv);                                                                                                       \
		}                                                                                                                                                      \
		while (n >= 0) {                                                                                                                                       \
			PUT_PIXEL(0); /*the_x += PSZB;*/                                                                                                                   \
			pz += 1;                                                                                                                                           \
			pp = (PIXEL*)((GLbyte*)pp + PSZB);                                                                                                                 \
			n -= 1;                                                                                                                                            \
		}                                                                                                                                                      \
	}

#include "ztriangle.h"
}





#endif // if 1

// Not maintained by Gek

