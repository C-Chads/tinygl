#include <stdlib.h>
#include "zbuffer.h"




#if TGL_FEATURE_RENDER_BITS == 32




#else

#error "USE 32 BIT MODE!!!"

#endif



#if TGL_FEATURE_POLYGON_STIPPLE

#define THE_X (((unsigned short)(pp-pp1)))
#define XSTIP(_a) ((THE_X+_a)& TGL_POLYGON_STIPPLE_MASK_X)
#define YSTIP (the_y & TGL_POLYGON_STIPPLE_MASK_Y)
//NOTES                                                           Divide by 8 to get the byte        Get the actual bit
#define STIPBIT(_a) (zb->stipplepattern[(XSTIP(_a) | (YSTIP<<TGL_POLYGON_STIPPLE_POW2_WIDTH))>>3] & (1<<(XSTIP(_a) & 7)))
#define STIPTEST(_a) !(zb->dostipple && !STIPBIT(_a))
#define ZCMP(z,zpix,_a) ((z) >= (zpix) && STIPTEST(_a))

#else

#define ZCMP(z,zpix,_a) ((z) >= (zpix))

#endif



void ZB_fillTriangleFlat(ZBuffer *zb,
			 ZBufferPoint *p0,ZBufferPoint *p1,ZBufferPoint *p2)
{




    unsigned int color;


#define INTERP_Z


#define DRAW_INIT()				\
{						\
  color=RGB_TO_PIXEL(p2->r,p2->g,p2->b);	\
}
  
#define PUT_PIXEL(_a)				\
{						\
    zz=z >> ZB_POINT_Z_FRAC_BITS;		\
    if (ZCMP(zz,pz[_a],_a)) {				\
      pp[_a]=color;				\
      pz[_a]=zz;				\
    }						\
    z+=dzdx;					\
}


#include "ztriangle.h"
}

/*
 * Smooth filled triangle.
 * The code below is very tricky :)
 */

void ZB_fillTriangleSmooth(ZBuffer *zb,
			   ZBufferPoint *p0,ZBufferPoint *p1,ZBufferPoint *p2)
{

//unsigned int color;
#define INTERP_Z
#define INTERP_RGB

#define SAR_RND_TO_ZERO(v,n) (v / (1<<n))



#define DRAW_INIT()				\
{						\
  	\
}

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

#include "ztriangle.h"
} //EOF smooth fill triangle

void ZB_setTexture(ZBuffer *zb,PIXEL *texture)
{
    zb->current_texture=texture;
}
//Ignore this it is never used
void ZB_fillTriangleMapping(ZBuffer *zb,
			    ZBufferPoint *p0,ZBufferPoint *p1,ZBufferPoint *p2)
{
    PIXEL *texture;

#define INTERP_Z
#define INTERP_ST

#define DRAW_INIT()				\
{						\
  texture=zb->current_texture;			\
}



#define PUT_PIXEL(_a)				\
{						\
   zz=z >> ZB_POINT_Z_FRAC_BITS;		\
     if (ZCMP(zz,pz[_a],_a)) {				\
       pp[_a]=texture[((t & 0x3FC00000) | s) >> 14];	\
       pz[_a]=zz;				\
    }						\
    z+=dzdx;					\
    s+=dsdx;					\
    t+=dtdx;					\
}

#include "ztriangle.h"
}

/*
 * Texture mapping with perspective correction.
 * We use the gradient method to make less divisions.
 * TODO: pipeline the division
 */


#if 1 //IF 1

void ZB_fillTriangleMappingPerspective(ZBuffer *zb,
                            ZBufferPoint *p0,ZBufferPoint *p1,ZBufferPoint *p2)
{
    PIXEL *texture;
    float fdzdx,fndzdx,ndszdx,ndtzdx;

#define INTERP_Z
#define INTERP_STZ

#define NB_INTERP 8

#define DRAW_INIT()				\
{						\
  texture=zb->current_texture;\
  fdzdx=(float)dzdx;\
  fndzdx=NB_INTERP * fdzdx;\
  ndszdx=NB_INTERP * dszdx;\
  ndtzdx=NB_INTERP * dtzdx;\
}


#define PUT_PIXEL(_a)				\
{						\
   zz=z >> ZB_POINT_Z_FRAC_BITS;		\
     if (ZCMP(zz,pz[_a],_a)) {				\
       pp[_a]=*(PIXEL *)((char *)texture+ \
               (((t & 0x3FC00000) | (s & 0x003FC000)) >> (17 - PSZSH)));\
       pz[_a]=zz;				\
    }						\
    z+=dzdx;					\
    s+=dsdx;					\
    t+=dtdx;					\
}



#define DRAW_LINE()				\
{						\
  register unsigned short *pz;		\
  register PIXEL *pp;		\
  register unsigned int s,t,z,zz;	\
  register int n,dsdx,dtdx;		\
  float sz,tz,fz,zinv; \
  n=(x2>>16)-x1;                             \
  fz=(float)z1;\
  zinv=1.0 / fz;\
  pp=(PIXEL *)((char *)pp1 + x1 * PSZB); \
  pz=pz1+x1;					\
  z=z1;						\
  sz=sz1;\
  tz=tz1;\
  while (n>=(NB_INTERP-1)) {						   \
    {\
      float ss,tt;\
      ss=(sz * zinv);\
      tt=(tz * zinv);\
      s=(int) ss;\
      t=(int) tt;\
      dsdx= (int)( (dszdx - ss*fdzdx)*zinv );\
      dtdx= (int)( (dtzdx - tt*fdzdx)*zinv );\
      fz+=fndzdx;\
      zinv=1.0 / fz;\
    }\
    PUT_PIXEL(0);/*the_x++;*/		   \
    PUT_PIXEL(1);/*the_x++;*/		   \
    PUT_PIXEL(2);/*the_x++;*/		   \
    PUT_PIXEL(3);/*the_x++;*/		   \
    PUT_PIXEL(4);/*the_x++;*/		   \
    PUT_PIXEL(5);/*the_x++;*/		   \
    PUT_PIXEL(6);/*the_x++;*/		   \
    PUT_PIXEL(7);/*the_x-=7;*/		   \
    pz+=NB_INTERP;							   \
    pp=(PIXEL *)((char *)pp + NB_INTERP * PSZB);/*the_x+=NB_INTERP * PSZB;*/\
    n-=NB_INTERP;							   \
    sz+=ndszdx;\
    tz+=ndtzdx;\
  }									   \
    {\
      float ss,tt;\
      ss=(sz * zinv);\
      tt=(tz * zinv);\
      s=(int) ss;\
      t=(int) tt;\
      dsdx= (int)( (dszdx - ss*fdzdx)*zinv );\
      dtdx= (int)( (dtzdx - tt*fdzdx)*zinv );\
    }\
  while (n>=0) {							   \
    PUT_PIXEL(0);/*the_x += PSZB;*/			   \
    pz+=1;								   \
    pp=(PIXEL *)((char *)pp + PSZB);\
    n-=1;								   \
  }									   \
}
  
#include "ztriangle.h"
}

#endif //if 1















#if 0

/* slow but exact version (only there for reference, incorrect for 24
   bits) */

void ZB_fillTriangleMappingPerspective(ZBuffer *zb,
                            ZBufferPoint *p0,ZBufferPoint *p1,ZBufferPoint *p2)
{
    PIXEL *texture;

#define INTERP_Z
#define INTERP_STZ

#define DRAW_INIT()				\
{						\
  texture=zb->current_texture;			\
}

#define PUT_PIXEL(_a)				\
{						\
   float zinv; \
   int s,t; \
   zz=z >> ZB_POINT_Z_FRAC_BITS;		\
     if (ZCMP(zz,pz[_a],_a)) {				\
       zinv= 1.0 / (float) z; \
       s= (int) (sz * zinv); \
       t= (int) (tz * zinv); \
       pp[_a]=texture[((t & 0x3FC00000) | s) >> 14];	\
       pz[_a]=zz;				\
    }						\
    z+=dzdx;					\
    sz+=dszdx;					\
    tz+=dtzdx;					\
}

#include "ztriangle.h"
}


#endif
