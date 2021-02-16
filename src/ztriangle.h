/*
 * We draw a triangle with various GLinterpolations
 */

{
  ZBufferPoint *t,*pr1,*pr2,*l1,*l2;
  GLfloat fdx1, fdx2, fdy1, fdy2, fz, d1, d2;
  GLushort *pz1;
  PIXEL *pp1;
  GLint part,update_left,update_right;

  GLint nb_lines,dx1,dy1,tmp,dx2,dy2;
#if TGL_FEATURE_POLYGON_STIPPLE == 1
  GLushort the_y;
#endif
  GLint error,derror;
  GLint x1,dxdy_min,dxdy_max;
/* warning: x2 is multiplied by 2^16 */
  GLint x2,dx2dy2;  

#ifdef INTERP_Z
  GLint z1,dzdx,dzdy,dzdl_min,dzdl_max;
#endif
#ifdef INTERP_RGB
  GLint r1,drdx,drdy,drdl_min,drdl_max;
  GLint g1,dgdx,dgdy,dgdl_min,dgdl_max;
  GLint b1,dbdx,dbdy,dbdl_min,dbdl_max;
#endif
#ifdef INTERP_ST
  GLint s1,dsdx,dsdy,dsdl_min,dsdl_max;
  GLint t1,dtdx,dtdy,dtdl_min,dtdl_max;
#endif
#ifdef INTERP_STZ
  GLfloat sz1,dszdx,dszdy,dszdl_min,dszdl_max;
  GLfloat tz1,dtzdx,dtzdy,dtzdl_min,dtzdl_max;
#endif

  /* we sort the vertex with increasing y */
  if (p1->y < p0->y) {
    t = p0;
    p0 = p1;
    p1 = t;
  }
  if (p2->y < p0->y) {
    t = p2;
    p2 = p1;
    p1 = p0;
    p0 = t;
  } else if (p2->y < p1->y) {
    t = p1;
    p1 = p2;
    p2 = t;
  }

  /* we compute dXdx and dXdy for all GLinterpolated values */
  
  fdx1 = p1->x - p0->x;
  fdy1 = p1->y - p0->y;

  fdx2 = p2->x - p0->x;
  fdy2 = p2->y - p0->y;

  fz = fdx1 * fdy2 - fdx2 * fdy1;
  if (fz == 0)
    return;
  fz = 1.0 / fz;

  fdx1 *= fz;
  fdy1 *= fz;
  fdx2 *= fz;
  fdy2 *= fz;

#ifdef INTERP_Z
  d1 = p1->z - p0->z;
  d2 = p2->z - p0->z;
  dzdx = (GLint) (fdy2 * d1 - fdy1 * d2);
  dzdy = (GLint) (fdx1 * d2 - fdx2 * d1);
#endif

#ifdef INTERP_RGB
  d1 = p1->r - p0->r;
  d2 = p2->r - p0->r;
  drdx = (GLint) (fdy2 * d1 - fdy1 * d2);
  drdy = (GLint) (fdx1 * d2 - fdx2 * d1);

  d1 = p1->g - p0->g;
  d2 = p2->g - p0->g;
  dgdx = (GLint) (fdy2 * d1 - fdy1 * d2);
  dgdy = (GLint) (fdx1 * d2 - fdx2 * d1);

  d1 = p1->b - p0->b;
  d2 = p2->b - p0->b;
  dbdx = (GLint) (fdy2 * d1 - fdy1 * d2);
  dbdy = (GLint) (fdx1 * d2 - fdx2 * d1);

#endif
  
#ifdef INTERP_ST
  d1 = p1->s - p0->s;
  d2 = p2->s - p0->s;
  dsdx = (GLint) (fdy2 * d1 - fdy1 * d2);
  dsdy = (GLint) (fdx1 * d2 - fdx2 * d1);
  
  d1 = p1->t - p0->t;
  d2 = p2->t - p0->t;
  dtdx = (GLint) (fdy2 * d1 - fdy1 * d2);
  dtdy = (GLint) (fdx1 * d2 - fdx2 * d1);
#endif

#ifdef INTERP_STZ
  {
    GLfloat zz;
    zz=(GLfloat) p0->z;
    p0->sz= (GLfloat) p0->s * zz;
    p0->tz= (GLfloat) p0->t * zz;
    zz=(GLfloat) p1->z;
    p1->sz= (GLfloat) p1->s * zz;
    p1->tz= (GLfloat) p1->t * zz;
    zz=(GLfloat) p2->z;
    p2->sz= (GLfloat) p2->s * zz;
    p2->tz= (GLfloat) p2->t * zz;

    d1 = p1->sz - p0->sz;
    d2 = p2->sz - p0->sz;
    dszdx = (fdy2 * d1 - fdy1 * d2);
    dszdy = (fdx1 * d2 - fdx2 * d1);
    
    d1 = p1->tz - p0->tz;
    d2 = p2->tz - p0->tz;
    dtzdx = (fdy2 * d1 - fdy1 * d2);
    dtzdy = (fdx1 * d2 - fdx2 * d1);
  }
#endif

  /* screen coordinates */

  pp1 = (PIXEL *) ((GLbyte *) zb->pbuf + zb->linesize * p0->y); 
#if TGL_FEATURE_POLYGON_STIPPLE == 1
  the_y = p0->y;
#endif
  pz1 = zb->zbuf + p0->y * zb->xsize;

  DRAW_INIT();

  for(part=0;part<2;part++) {
    if (part == 0) {
      if (fz > 0) {
	update_left=1;
	update_right=1;
	l1=p0;
	l2=p2;
	pr1=p0;
	pr2=p1;
      } else {
	update_left=1;
	update_right=1;
	l1=p0;
	l2=p1;
	pr1=p0;
	pr2=p2;
      }
      nb_lines = p1->y - p0->y;
    } else {
      /* second part */
      if (fz > 0) {
	update_left=0;
	update_right=1;
	pr1=p1;
	pr2=p2;
      } else {
	update_left=1;
	update_right=0;
	l1=p1; 
	l2=p2;
      }
      nb_lines = p2->y - p1->y + 1;
    }

    /* compute the values for the left edge */

    if (update_left) {
      dy1 = l2->y - l1->y;
      dx1 = l2->x - l1->x;
      if (dy1 > 0) 
	tmp = (dx1 << 16) / dy1;
      else
	tmp = 0;
      x1 = l1->x;
      error = 0;
      derror = tmp & 0x0000ffff;
      dxdy_min = tmp >> 16;
      dxdy_max = dxdy_min + 1;
      
#ifdef INTERP_Z
      z1=l1->z;
      dzdl_min=(dzdy + dzdx * dxdy_min); 
      dzdl_max=dzdl_min + dzdx;
#endif
#ifdef INTERP_RGB
      r1=l1->r;
      drdl_min=(drdy + drdx * dxdy_min);
      drdl_max=drdl_min + drdx;
      
      g1=l1->g;
      dgdl_min=(dgdy + dgdx * dxdy_min);
      dgdl_max=dgdl_min + dgdx;
      
      b1=l1->b;
      dbdl_min=(dbdy + dbdx * dxdy_min);
      dbdl_max=dbdl_min + dbdx;
#endif
#ifdef INTERP_ST
      s1=l1->s;
      dsdl_min=(dsdy + dsdx * dxdy_min);
      dsdl_max=dsdl_min + dsdx;
      
      t1=l1->t;
      dtdl_min=(dtdy + dtdx * dxdy_min);
      dtdl_max=dtdl_min + dtdx;
#endif
#ifdef INTERP_STZ
      sz1=l1->sz;
      dszdl_min=(dszdy + dszdx * dxdy_min);
      dszdl_max=dszdl_min + dszdx;
      
      tz1=l1->tz;
      dtzdl_min=(dtzdy + dtzdx * dxdy_min);
      dtzdl_max=dtzdl_min + dtzdx;
#endif
    }

    /* compute values for the right edge */

    if (update_right) {
      dx2 = (pr2->x - pr1->x);
      dy2 = (pr2->y - pr1->y);
      if (dy2>0) 
	dx2dy2 = ( dx2 << 16) / dy2;
      else
	dx2dy2 = 0;
      x2 = pr1->x << 16;
    }

    /* we draw all the scan line of the part */

    while (nb_lines>0) {
      nb_lines--;
#ifndef DRAW_LINE
      /* generic draw line */
      {
          register PIXEL *pp;
          register GLint n;
#ifdef INTERP_Z
          register GLushort *pz;
          register GLuint z,zz;
#endif
#ifdef INTERP_RGB
          register GLuint or1,og1,ob1;
#endif
#ifdef INTERP_ST
          register GLuint s,t;
#endif
#ifdef INTERP_STZ
          GLfloat sz,tz;
#endif

          n=(x2 >> 16) - x1;
          /*the_x = x1; //Gek added this to make determining the X coordinate easier!*/
          pp=(PIXEL *)((GLbyte *)pp1 + x1 * PSZB);
#ifdef INTERP_Z
          pz=pz1+x1;
          z=z1;
#endif
#ifdef INTERP_RGB
          or1 = r1;
          og1 = g1;
          ob1 = b1;
#endif
#ifdef INTERP_ST
          s=s1;
          t=t1;
#endif
#ifdef INTERP_STZ
          sz=sz1;
          tz=tz1;
#endif
          while (n>=3) {
              PUT_PIXEL(0);/*the_x++;*/
              PUT_PIXEL(1);/*the_x++;*/
              PUT_PIXEL(2);/*the_x++;*/
              PUT_PIXEL(3);/*the_x++;*/
#ifdef INTERP_Z
              pz+=4;
#endif
              pp=(PIXEL *)((GLbyte *)pp + 4 * PSZB);
              n-=4;
          }
          while (n>=0) {
              PUT_PIXEL(0);/*the_x++;*/
#ifdef INTERP_Z
              pz+=1;
#endif
              pp=(PIXEL *)((GLbyte *)pp + PSZB);
              n-=1;
          }
      }
      //the_y++;
#else
      DRAW_LINE();//the_y++;
#endif
      
      /* left edge */
      error+=derror;
      if (error > 0) {
	error-=0x10000;
	x1+=dxdy_max;
#ifdef INTERP_Z
	z1+=dzdl_max;
#endif      
#ifdef INTERP_RGB
	r1+=drdl_max;
	g1+=dgdl_max;
	b1+=dbdl_max;
#endif
#ifdef INTERP_ST
	s1+=dsdl_max;
	t1+=dtdl_max;
#endif
#ifdef INTERP_STZ
	sz1+=dszdl_max;
	tz1+=dtzdl_max;
#endif
      } else {
	x1+=dxdy_min;
#ifdef INTERP_Z
	z1+=dzdl_min;
#endif      
#ifdef INTERP_RGB
	r1+=drdl_min;
	g1+=dgdl_min;
	b1+=dbdl_min;
#endif
#ifdef INTERP_ST
	s1+=dsdl_min;
	t1+=dtdl_min;
#endif
#ifdef INTERP_STZ
	sz1+=dszdl_min;
	tz1+=dtzdl_min;
#endif
      } 
      
      /* right edge */
      x2+=dx2dy2;

      /* screen coordinates */
      pp1=(PIXEL *)((GLbyte *)pp1 + zb->linesize);
#if TGL_FEATURE_POLYGON_STIPPLE == 1
      the_y++;
#endif
      pz1+=zb->xsize;
    }
  }
}

#undef INTERP_Z
#undef INTERP_RGB
#undef INTERP_ST
#undef INTERP_STZ

#undef DRAW_INIT
#undef DRAW_LINE  
#undef PUT_PIXEL
