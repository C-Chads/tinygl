#include "zgl.h"


void glopClearColor(GLContext *c,GLParam *p)
{
  c->clear_color.v[0]=p[1].f;
  c->clear_color.v[1]=p[2].f;
  c->clear_color.v[2]=p[3].f;
  c->clear_color.v[3]=p[4].f;
}
void glopClearDepth(GLContext *c,GLParam *p)
{
  c->clear_depth=p[1].f;
}


void glopClear(GLContext *c,GLParam *p)
{
  GLint mask=p[1].i;
  GLint z=0;
  GLint r=(int)(c->clear_color.v[0]*65535);
  GLint g=(int)(c->clear_color.v[1]*65535);
  GLint b=(int)(c->clear_color.v[2]*65535);

  /* TODO : correct value of Z */

  ZB_clear(c->zb,mask & GL_DEPTH_BUFFER_BIT,z,
	   mask & GL_COLOR_BUFFER_BIT,r,g,b);
}

