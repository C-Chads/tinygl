#ifndef __ZMATH__
#define __ZMATH__
#include "../include/GL/gl.h"
/* Matrix & Vertex */

typedef struct {
	GLfloat m[4][4];
} M4;

typedef struct {
	GLfloat m[3][3];
} M3;

typedef struct {
	GLfloat m[3][4];
} M34;

#define X v[0]
#define Y v[1]
#define Z v[2]
#define W v[3]

typedef struct {
	GLfloat v[3];
} V3;

typedef struct {
	GLfloat v[4];
} V4;

void gl_M4_Id(M4* a);
int gl_M4_IsId(M4* a);
void gl_M4_Move(M4* a, M4* b);
void gl_MoveV3(V3* a, V3* b);
void gl_MulM4V3(V3* a, M4* b, V3* c);
void gl_MulM3V3(V3* a, M4* b, V3* c);

void gl_M4_MulV4(V4* a, M4* b, V4* c);
void gl_M4_InvOrtho(M4* a, M4 b);
void gl_M4_Inv(M4* a, M4* b);
void gl_M4_Mul(M4* c, M4* a, M4* b);
void gl_M4_MulLeft(M4* c, M4* a);
void gl_M4_Transpose(M4* a, M4* b);
void gl_M4_Rotate(M4* c, GLfloat t, GLint u);
int gl_V3_Norm(V3* a);

V3 gl_V3_New(GLfloat x, GLfloat y, GLfloat z);
V4 gl_V4_New(GLfloat x, GLfloat y, GLfloat z, GLfloat w);

int gl_Matrix_Inv(GLfloat* r, GLfloat* m, GLint n);

#endif
// __ZMATH__
