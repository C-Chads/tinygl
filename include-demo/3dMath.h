/* Public Domain / CC0 C99 Vector Math Library

*/

#ifndef CHAD_MATH_H
#define CHAD_MATH_H
/* Default behavior- compatibility.

*/
#ifndef CHAD_MATH_NO_ALIGN
#define CHAD_MATH_NO_ALIGN
#endif

#ifdef __TINYC__
#define CHAD_MATH_NO_ALIGN
#endif


#ifndef CHAD_MATH_NO_ALIGN
#include <stdalign.h>
#define CHAD_ALIGN alignas(16)
#warning "Chad math library compiling with alignas of 16, malloc and realloc MUST return 16-byte-aligned pointers."
#else
#define CHAD_ALIGN /*a comment*/
#endif
#include <math.h>
#include <string.h>
typedef float f_;
typedef unsigned int uint;
#define MAX(x,y) (x>y?x:y)
#define MIN(x,y) (x<y?x:y)
typedef struct {CHAD_ALIGN f_ d[3];} vec3;
typedef struct {CHAD_ALIGN int d[3];} ivec3;
typedef struct {CHAD_ALIGN f_ d[4];} vec4;
typedef struct {CHAD_ALIGN f_ d[16];} mat4;

/*Collision detection
These Algorithms return the penetration vector into
the shape in the first argument
With depth of penetration in element 4
if depth of penetration is zero or lower then there is no penetration.
*/
typedef struct{
	vec4 c;
	vec3 e;
}aabb;
typedef aabb colshape; /*c.d[3] determines if it's a sphere or box. 0 or less = box, greater than 0 = sphere*/





static inline mat4 scalemat4( vec4 s){
	mat4 ret;
	for(int i = 1; i < 16; i++)
		ret.d[i]= 0.0;
	ret.d[0*4 + 0] = s.d[0]; 
	ret.d[1*4 + 1] = s.d[1]; 
	ret.d[2*4 + 2] = s.d[2]; 
	ret.d[3*4 + 3] = s.d[3]; 
	return ret;
}

static inline int invmat4( mat4 m, mat4* invOut) /*returns 1 if successful*/
{
    mat4 inv;
    f_ det;
    int i;

    inv.d[0] = m.d[5]  * m.d[10] * m.d[15] - 
             m.d[5]  * m.d[11] * m.d[14] - 
             m.d[9]  * m.d[6]  * m.d[15] + 
             m.d[9]  * m.d[7]  * m.d[14] +
             m.d[13] * m.d[6]  * m.d[11] - 
             m.d[13] * m.d[7]  * m.d[10];

    inv.d[4] = -m.d[4]  * m.d[10] * m.d[15] + 
              m.d[4]  * m.d[11] * m.d[14] + 
              m.d[8]  * m.d[6]  * m.d[15] - 
              m.d[8]  * m.d[7]  * m.d[14] - 
              m.d[12] * m.d[6]  * m.d[11] + 
              m.d[12] * m.d[7]  * m.d[10];

    inv.d[8] = m.d[4]  * m.d[9] * m.d[15] - 
             m.d[4]  * m.d[11] * m.d[13] - 
             m.d[8]  * m.d[5] * m.d[15] + 
             m.d[8]  * m.d[7] * m.d[13] + 
             m.d[12] * m.d[5] * m.d[11] - 
             m.d[12] * m.d[7] * m.d[9];

    inv.d[12] = -m.d[4]  * m.d[9] * m.d[14] + 
               m.d[4]  * m.d[10] * m.d[13] +
               m.d[8]  * m.d[5] * m.d[14] - 
               m.d[8]  * m.d[6] * m.d[13] - 
               m.d[12] * m.d[5] * m.d[10] + 
               m.d[12] * m.d[6] * m.d[9];

    inv.d[1] = -m.d[1]  * m.d[10] * m.d[15] + 
              m.d[1]  * m.d[11] * m.d[14] + 
              m.d[9]  * m.d[2] * m.d[15] - 
              m.d[9]  * m.d[3] * m.d[14] - 
              m.d[13] * m.d[2] * m.d[11] + 
              m.d[13] * m.d[3] * m.d[10];

    inv.d[5] = m.d[0]  * m.d[10] * m.d[15] - 
             m.d[0]  * m.d[11] * m.d[14] - 
             m.d[8]  * m.d[2] * m.d[15] + 
             m.d[8]  * m.d[3] * m.d[14] + 
             m.d[12] * m.d[2] * m.d[11] - 
             m.d[12] * m.d[3] * m.d[10];

    inv.d[9] = -m.d[0]  * m.d[9] * m.d[15] + 
              m.d[0]  * m.d[11] * m.d[13] + 
              m.d[8]  * m.d[1] * m.d[15] - 
              m.d[8]  * m.d[3] * m.d[13] - 
              m.d[12] * m.d[1] * m.d[11] + 
              m.d[12] * m.d[3] * m.d[9];

    inv.d[13] = m.d[0]  * m.d[9] * m.d[14] - 
              m.d[0]  * m.d[10] * m.d[13] - 
              m.d[8]  * m.d[1] * m.d[14] + 
              m.d[8]  * m.d[2] * m.d[13] + 
              m.d[12] * m.d[1] * m.d[10] - 
              m.d[12] * m.d[2] * m.d[9];

    inv.d[2] = m.d[1]  * m.d[6] * m.d[15] - 
             m.d[1]  * m.d[7] * m.d[14] - 
             m.d[5]  * m.d[2] * m.d[15] + 
             m.d[5]  * m.d[3] * m.d[14] + 
             m.d[13] * m.d[2] * m.d[7] - 
             m.d[13] * m.d[3] * m.d[6];

    inv.d[6] = -m.d[0]  * m.d[6] * m.d[15] + 
              m.d[0]  * m.d[7] * m.d[14] + 
              m.d[4]  * m.d[2] * m.d[15] - 
              m.d[4]  * m.d[3] * m.d[14] - 
              m.d[12] * m.d[2] * m.d[7] + 
              m.d[12] * m.d[3] * m.d[6];

    inv.d[10] = m.d[0]  * m.d[5] * m.d[15] - 
              m.d[0]  * m.d[7] * m.d[13] - 
              m.d[4]  * m.d[1] * m.d[15] + 
              m.d[4]  * m.d[3] * m.d[13] + 
              m.d[12] * m.d[1] * m.d[7] - 
              m.d[12] * m.d[3] * m.d[5];

    inv.d[14] = -m.d[0]  * m.d[5] * m.d[14] + 
               m.d[0]  * m.d[6] * m.d[13] + 
               m.d[4]  * m.d[1] * m.d[14] - 
               m.d[4]  * m.d[2] * m.d[13] - 
               m.d[12] * m.d[1] * m.d[6] + 
               m.d[12] * m.d[2] * m.d[5];

    inv.d[3] = -m.d[1] * m.d[6] * m.d[11] + 
              m.d[1] * m.d[7] * m.d[10] + 
              m.d[5] * m.d[2] * m.d[11] - 
              m.d[5] * m.d[3] * m.d[10] - 
              m.d[9] * m.d[2] * m.d[7] + 
              m.d[9] * m.d[3] * m.d[6];

    inv.d[7] = m.d[0] * m.d[6] * m.d[11] - 
             m.d[0] * m.d[7] * m.d[10] - 
             m.d[4] * m.d[2] * m.d[11] + 
             m.d[4] * m.d[3] * m.d[10] + 
             m.d[8] * m.d[2] * m.d[7] - 
             m.d[8] * m.d[3] * m.d[6];

    inv.d[11] = -m.d[0] * m.d[5] * m.d[11] + 
               m.d[0] * m.d[7] * m.d[9] + 
               m.d[4] * m.d[1] * m.d[11] - 
               m.d[4] * m.d[3] * m.d[9] - 
               m.d[8] * m.d[1] * m.d[7] + 
               m.d[8] * m.d[3] * m.d[5];

    inv.d[15] = m.d[0] * m.d[5] * m.d[10] - 
              m.d[0] * m.d[6] * m.d[9] - 
              m.d[4] * m.d[1] * m.d[10] + 
              m.d[4] * m.d[2] * m.d[9] + 
              m.d[8] * m.d[1] * m.d[6] - 
              m.d[8] * m.d[2] * m.d[5];

    det = m.d[0] * inv.d[0] + m.d[1] * inv.d[4] + m.d[2] * inv.d[8] + m.d[3] * inv.d[12];
    if (det == 0)
        return 0;
    det = 1.0 / det;
    for (i = 0; i < 16; i++)
        invOut->d[i] = inv.d[i] * det;
    return 1;
}
static inline mat4 perspective( f_ fov,  f_ aspect,  f_ near,  f_ far){
	mat4 ret;
	f_ D2R = 3.14159265358979323 / 180.0;
	f_ yScale = 1.0/tanf(D2R * fov/2);
	f_ xScale = yScale/aspect;
	f_ nearmfar = near-far;
	
	ret.d[0*4+0] = xScale; 	ret.d[0*4+1]=0; 	ret.d[0*4+2]=0;					ret.d[0*4+3]=0;
	ret.d[1*4+0]=0; 		ret.d[1*4+1]=yScale;ret.d[1*4+2]=0;					ret.d[1*4+3]=0;
	ret.d[2*4+0]=0; 		ret.d[2*4+1]=0;		ret.d[2*4+2]=(far+near)/nearmfar;ret.d[2*4+3]=-1;
	ret.d[3*4+0]=0; 		ret.d[3*4+1]=0;		ret.d[3*4+2]=2*far*near/nearmfar;ret.d[3*4+3]=0;
	
	/*
	ret.d[0*4+0] = xScale; 	ret.d[0*4+1]=0; 	ret.d[0*4+2]=0;						ret.d[0*4+3]=0;
	ret.d[1*4+0]=0; 		ret.d[1*4+1]=yScale;ret.d[1*4+2]=0;						ret.d[1*4+3]=0;
	ret.d[2*4+0]=0; 		ret.d[2*4+1]=0;		ret.d[2*4+2]=(far+near)/nearmfar;	ret.d[2*4+3]=2*far*near/nearmfar;
	ret.d[3*4+0]=0; 		ret.d[3*4+1]=0;		ret.d[3*4+2]=-1;					ret.d[3*4+3]=0;
	*/
	return ret;
}
static inline vec3 viewport( uint xdim,  uint ydim,  vec3 input){
	input.d[0] += 1;
	input.d[1] += 1;
	input.d[0] *= (f_)xdim / 2.0;
	input.d[1] *= (f_)ydim / 2.0;
	input.d[2] = (input.d[2])/2.0;
	return input;
}
static inline mat4 rotate( vec3 rotation){
	f_ a = rotation.d[0];
	f_ b = rotation.d[1];
	f_ c = rotation.d[2];
	mat4 rm;
	rm.d[0*4 + 0] = cosf(a)*cosf(b);
	rm.d[1*4 + 0] = sinf(a)*cosf(b);
	rm.d[2*4 + 0] = -sinf(b);
	rm.d[0*4 + 1] = cosf(a)*sinf(b)*sinf(c)-sinf(a)*cosf(c);
	rm.d[1*4 + 1] = sinf(a)*sinf(b)*sinf(c)+cosf(a)*cosf(c);
	rm.d[2*4 + 1] = cosf(b)*sinf(c);
	rm.d[0*4 + 2] = cosf(a)*sinf(b)*cosf(c)+sinf(a)*sinf(c);
	rm.d[1*4 + 2] = sinf(a)*sinf(b)*cosf(c)-cosf(a)*sinf(c);
	rm.d[2*4 + 2] = cosf(b)*cosf(c);
	rm.d[0*4 + 3] = 0;
	rm.d[1*4 + 3] = 0;
	rm.d[2*4 + 3] = 0;
	rm.d[3*4 + 3] = 1; /*the bottom right corner of the matrix.*/
	rm.d[3*4 + 0] = 0;
	rm.d[3*4 + 1] = 0;
	rm.d[3*4 + 2] = 0;
	return rm;
}


static inline f_ clampf( f_ a,  f_ min,  f_ max){
	if(a<min) return min;
	if(a>max) return max;
	return a;
}
static inline f_ lengthv3( vec3 a){
	return sqrtf(a.d[0] * a.d[0] + a.d[1] * a.d[1] + a.d[2] * a.d[2]);
}
static inline f_ lengthv4( vec4 a){
	return sqrtf(a.d[0] * a.d[0] + a.d[1] * a.d[1] + a.d[2] * a.d[2] + a.d[3] * a.d[3]);
}
static inline vec3 multvec3( vec3 a,  vec3 b){
	return (vec3){
		.d[0]=a.d[0]*b.d[0],
		.d[1]=a.d[1]*b.d[1],
		.d[2]=a.d[2]*b.d[2]
	};
}
static inline vec4 multvec4( vec4 a,  vec4 b){
	return (vec4){
		.d[0]=a.d[0]*b.d[0],
		.d[1]=a.d[1]*b.d[1],
		.d[2]=a.d[2]*b.d[2],
		.d[3]=a.d[3]*b.d[3]
	};
}
static inline vec3 clampvec3( vec3 a,  vec3 min,  vec3 max){
	vec3 ret;
	ret.d[0] = clampf(a.d[0],min.d[0],max.d[0]);
	ret.d[1] = clampf(a.d[1],min.d[1],max.d[1]);
	ret.d[2] = clampf(a.d[2],min.d[2],max.d[2]);
	return ret;
}
static inline vec4 clampvec4( vec4 a,  vec4 min,  vec4 max){
	vec4 ret;
	ret.d[0] = clampf(a.d[0],min.d[0],max.d[0]);
	ret.d[1] = clampf(a.d[1],min.d[1],max.d[1]);
	ret.d[2] = clampf(a.d[2],min.d[2],max.d[2]);
	ret.d[3] = clampf(a.d[3],min.d[3],max.d[3]);
	return ret;
}
static inline f_ dotv3( vec3 a,  vec3 b){
	return a.d[0] * b.d[0] + a.d[1] * b.d[1] + a.d[2] * b.d[2]; 
}
static inline f_ dotv4( vec4 a,  vec4 b){
	return a.d[0] * b.d[0] + a.d[1] * b.d[1] + a.d[2] * b.d[2] + a.d[3] * b.d[3]; 
}
static inline vec4 getrow( mat4 a,  uint index){
	return (vec4){
		.d[0]=a.d[index],
		.d[1]=a.d[4+index],
		.d[2]=a.d[8+index],
		.d[3]=a.d[12+index]
	};
}
static inline mat4 swapRowColumnMajor( mat4 in){
	mat4 result;
	vec4 t;
	int i = 0;
	t = getrow(in,i);
	memcpy(result.d+i*4, t.d, 4*4);i++;
	t = getrow(in,i);
	memcpy(result.d+i*4, t.d, 4*4);i++;
	t = getrow(in,i);
	memcpy(result.d+i*4, t.d, 4*4);i++;
	t = getrow(in,i);
	memcpy(result.d+i*4, t.d, 4*4);
	return result;
}

static inline vec4 getcol( mat4 a,  uint index){
	return (vec4){
		.d[0]=a.d[index*4],
		.d[1]=a.d[index*4+1],
		.d[2]=a.d[index*4+2],
		.d[3]=a.d[index*4+3]
	};
}
static inline mat4 multm4( mat4 a,  mat4 b){
	mat4 ret;
#pragma omp simd
	for(int i = 0; i < 4; i++)
	for(int j = 0; j < 4; j++)
		ret.d[i*4 + j] = dotv4( /*j is the ROW of the target, i is the COLUMN.*/
			getrow(a, j), /*we retrieve the same ROW as our ROW INDEX.*/
			getcol(b, i) /*we retrieve the same COLUMN as our COLUMN INDEX.*/
		);
	return ret;
}
static inline vec4 mat4xvec4( mat4 t,  vec4 v){
	vec4 vr;
	/*
	Getting a ROW of the matrix and dotting it with the COLUMN VECTOR to get
	 ONE ROW of the output COLUMN VECTOR- one float.*/
	vr.d[0] = 	t.d[0*4] * v.d[0] + 
				t.d[1*4] * v.d[1] +
				t.d[2*4] * v.d[2] +
				t.d[3*4] * v.d[3];
	vr.d[1] = 	t.d[0*4+1] * v.d[0] +
				t.d[1*4+1] * v.d[1] + 
				t.d[2*4+1] * v.d[2] + 
				t.d[3*4+1] * v.d[3];
	vr.d[2] = 	t.d[0*4+2] * v.d[0] + 
				t.d[1*4+2] * v.d[1] + 
				t.d[2*4+2] * v.d[2] + 
				t.d[3*4+2] * v.d[3];
	vr.d[3] = 	t.d[0*4+3] * v.d[0] + 
				t.d[1*4+3] * v.d[1] + 
				t.d[2*4+3] * v.d[2] + 
				t.d[3*4+3] * v.d[3];
	return vr;
}
static inline vec3 crossv3( vec3 a,  vec3 b){
	vec3 retval;
	retval.d[0] = a.d[1] * b.d[2] - a.d[2] * b.d[1];
	retval.d[1] = a.d[2] * b.d[0] - a.d[0] * b.d[2];
	retval.d[2] = a.d[0] * b.d[1] - a.d[1] * b.d[0];
	return retval;
}
static inline vec3 scalev3( f_ s,  vec3 i){i.d[0] *= s; i.d[1] *= s; i.d[2] *= s; return i;}

static inline vec4 scalev4( f_ s,  vec4 i){i.d[0] *= s; i.d[1] *= s; i.d[2] *= s;i.d[3] *= s; return i;}

static inline vec3 normalizev3( vec3 a){
  	if(lengthv3(a)==0) return (vec3){.d[0]=0.0,.d[1]=0.0,.d[2]=1.0};
	return scalev3(1.0/lengthv3(a), a);
}
static inline vec4 normalizev4( vec4 a){
  	if(lengthv4(a)==0) return (vec4){.d[0]=0.0,.d[1]=0.0,.d[2]=1.0,.d[3]=0.0};
	return scalev4(1.0/lengthv4(a), a);
}
static inline vec3 addv3( vec3 aa,  vec3 b){
	vec3 a = aa;
	a.d[0] += b.d[0]; a.d[1] += b.d[1]; a.d[2] += b.d[2]; return a;
}
static inline vec3 rotatev3( vec3 in,  vec3 axis,  f_ ang){
	vec3 t1 = scalev3(cosf(ang),in);
	vec3 t2 = scalev3(sinf(ang),crossv3(axis,in));
	vec3 t3 = scalev3((1-cosf(ang))*dotv3(axis,in),axis);
	return addv3(t1,addv3(t2,t3));
}
static inline vec4 addv4( vec4 aa,  vec4 b){
	vec4 a = aa;
	a.d[0] += b.d[0]; a.d[1] += b.d[1]; a.d[2] += b.d[2]; a.d[3] += b.d[3]; return a;
}
static inline vec3 subv3( vec3 a,  vec3 b){
	return addv3(a,scalev3(-1,b));
}
static inline mat4 identitymat4(){
	return scalemat4(
		(vec4){.d[0]=1.0,.d[1]=1.0,.d[2]=1.0,.d[3]=1.0}
	);
}
static inline mat4 translate( vec3 t){
	mat4 tm = identitymat4();
	tm.d[3*4+0] = t.d[0];
	tm.d[3*4+1] = t.d[1];
	tm.d[3*4+2] = t.d[2];
	return tm;
}
static inline vec4 subv4( vec4 a,  vec4 b){
	return addv4(a,scalev4(-1,b));
}
static inline vec3 reflect( vec3 in,  vec3 norm){
	return 
	addv3(in, 
		scalev3(-2.0*dotv3(norm, in),
			norm 
		)
	);
}
static inline vec4 upv3( vec3 in,  f_ w){
	return (vec4){
		.d[0]=in.d[0],
		.d[1]=in.d[1],
		.d[2]=in.d[2],
		.d[3]=w
	};
}
static inline vec3 downv4( vec4 in){
	return (vec3){
		.d[0]=in.d[0],
		.d[1]=in.d[1],
		.d[2]=in.d[2]
	};
}
static inline mat4 lookAt( vec3 eye,  vec3 at,  vec3 up){
	mat4 cw = identitymat4();
	vec3 zaxis = normalizev3(subv3(at,eye));
	vec3 xaxis = normalizev3(crossv3(zaxis,up));
	vec3 yaxis = crossv3(xaxis, zaxis);
	zaxis = scalev3(-1,zaxis);
	cw.d[0*4+0] = xaxis.d[0];
	cw.d[1*4+0] = xaxis.d[1];
	cw.d[2*4+0] = xaxis.d[2];
	cw.d[3*4+0] = -dotv3(xaxis,eye);

	cw.d[0*4+1] = yaxis.d[0];
	cw.d[1*4+1] = yaxis.d[1];
	cw.d[2*4+1] = yaxis.d[2];
	cw.d[3*4+1] = -dotv3(yaxis,eye);

	cw.d[0*4+2] = zaxis.d[0];
	cw.d[1*4+2] = zaxis.d[1];
	cw.d[2*4+2] = zaxis.d[2];
	cw.d[3*4+2] = -dotv3(zaxis,eye);
	cw.d[0*4+3] = 0;
	cw.d[1*4+3] = 0;
	cw.d[2*4+3] = 0;
	cw.d[3*4+3] = 1;
	return cw;
}
/*
Collision detection

These Algorithms return the penetration vector into
the shape in the first argument
With depth of penetration in element 4
if depth of penetration is zero or lower then there is no penetration.
*/
static inline vec4 spherevsphere( vec4 s1,  vec4 s2){ 
	vec4 ret;
	vec3 diff = subv3(
				downv4(s2),
				downv4(s1)
			);
	f_ lv3 = lengthv3(diff);
	f_ l = (s1.d[3] + s2.d[3]-lv3);
	
	if(l < 0 || lv3 == 0) {
		ret.d[3] = 0;return ret;
	}
	ret = upv3(
		scalev3(
			l/lv3,diff
		)
		,l
	);
	return ret;
}
static inline int boxvboxbool (aabb b1, aabb b2){
		vec3 sumextents = addv3(b1.e,b2.e);
		vec3 b1c = downv4(b1.c);
		vec3 b2c = downv4(b2.c);	
		if(
			!(
				(fabs(b1c.d[0] - b2c.d[0]) <= sumextents.d[0]) &&
				(fabs(b1c.d[1] - b2c.d[1]) <= sumextents.d[1]) &&
				(fabs(b1c.d[2] - b2c.d[2]) <= sumextents.d[2])
			)
		){
			return 0;
		}
		return 1;
}
static inline vec4 boxvbox( aabb b1,  aabb b2){ /*Just points along the minimum separating axis, Nothing fancy.*/
	vec4 ret = (vec4){
		.d[0]=0,
		.d[1]=0,
		.d[2]=0,
		.d[3]=0
	};
	vec3 sumextents = addv3(b1.e,b2.e);
	vec3 b1c = downv4(b1.c);
	vec3 b2c = downv4(b2.c);

	vec3 b1min = subv3(b1c,b1.e);
	vec3 b2min = subv3(b2c,b2.e);

	vec3 b1max = addv3(b1c,b1.e);
	vec3 b2max = addv3(b2c,b2.e);
	
	if(
		!(
			(fabs(b1c.d[0] - b2c.d[0]) <= sumextents.d[0]) &&
			(fabs(b1c.d[1] - b2c.d[1]) <= sumextents.d[1]) &&
			(fabs(b1c.d[2] - b2c.d[2]) <= sumextents.d[2])
		)
	){
		return ret;
	}
	vec3 axispen[2];
	axispen[0] = subv3(b1max,b2min);
	axispen[1] = subv3(b1min,b2max);
	ret.d[3] = fabs(axispen[0].d[0]);
	ret.d[0] = axispen[0].d[0];
	for(int i = 1; i < 6; i++){
		if(fabs(axispen[i/3].d[i%3]) < fabs(ret.d[3])){
			ret = (vec4){
						.d[0]=0,
						.d[1]=0,
						.d[2]=0,
						.d[3]=(axispen[i/3].d[i%3])
					};
			ret.d[i%3] = ret.d[3];
			ret.d[3] = fabs(ret.d[3]);
		}
	}
	return ret;
}
static inline vec3 closestpointAABB( aabb b,  vec3 p){
	vec3 b1min = subv3(downv4(b.c),b.e);
	vec3 b1max = addv3(downv4(b.c),b.e);
	return clampvec3(p,b1min,b1max);
}
static inline vec4 spherevaabb( vec4 sph,  aabb box){
	vec4 ret;
	vec3 p = closestpointAABB(box,downv4(sph));
	vec3 v = subv3(p,downv4(sph));
	f_ d2 = dotv3(v,v);
	
	if(d2 <= sph.d[3] * sph.d[3]){
		f_ len = lengthv3(v);
		f_ diff = (sph.d[3] - len);
		if(len > 0){
			f_ factor = diff/len;
			vec3 bruh = scalev3(factor, v);
			ret = upv3(bruh, diff);
			return ret;
		} else {
			aabb virt;
			virt.c = sph;
			virt.e.d[0] = sph.d[3];
			virt.e.d[1] = sph.d[3];
			virt.e.d[2] = sph.d[3];
			return boxvbox(virt,box);
		}
	}
	else
		return (vec4){
			.d[0]=0,
			.d[1]=0,
			.d[2]=0,
			.d[3]=0
		};
		
}


/*END Math_Library.h~~~~~~~~~~~~~~~~~~~~*/

#endif
