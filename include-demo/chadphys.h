#ifndef CHAD_PHYS_H
#define CHAD_PHYS_H

#ifdef CHAD_PHYS_IMPL
#define CHAD_MATH_IMPL
#endif
#include "3dMath.h"
typedef struct {
	aabb shape; //c.d[3] is sphere radius. 
		//if it's zero or less, it's not a sphere, it's a box
	f_ mass; //0 means kinematic, or static. Defaults to zero.
	f_ bounciness; //default 0, put portion of displacement into velocity.
	f_ airfriction; //default 1, multiplied by velocity every time timestep.
	f_ friction; //default 0.1
	vec3 r; //Rotation, Used for rendering only
	vec3 v; //velocity
	vec3 a; //Body specific acceleration, combined with gravity
	void* d; //User defined pointer.
} phys_body;
typedef struct{
	phys_body* abodies; //mass non-zero
	phys_body* sbodies; //mass zero
	uint nabodies; //number of abodies
	uint nsbodies; //number of sbodies
	vec3 g; //gravity
	f_ ms; //max speed
} phys_world;
//TODO: implement functions
static inline void initPhysBody(phys_body* body){
	body->shape = (aabb){
		.c=(vec4){.d[0] = 0,.d[1] = 0,.d[2] = 0,.d[3] = 0},
		.e=(vec3){.d[0] = 0,.d[1] = 0,.d[2] = 0}
	};
	body->mass = 0;
	body->bounciness = 0;
	body->friction = 0.1;
	body->airfriction = 1.0;
	body->a = (vec3){.d[0] = 0,.d[1] = 0,.d[2] = 0};
	body->r = (vec3){.d[0] = 0,.d[1] = 0,.d[2] = 0};
	body->d = NULL;
}

//Check for and, if necessary, resolve colliding bodies.
static inline void resolveBodies(phys_body* a, phys_body* b){
	if(a->mass <= 0 && b->mass <= 0) return; //Perform a preliminary check. Do we even have to do anything?
	
	vec4 penvec = (vec4){
		.d[0]=0,
		.d[1]=0,
		.d[2]=0,
		.d[3]=0
	};
	//Check if the two bodies are colliding.
	if(a->shape.c.d[3] > 0 && b->shape.c.d[3] > 0) //Both Spheres!
	{
		penvec = spherevsphere(a->shape.c, b->shape.c);
	} else if(a->shape.c.d[3] <= 0 && b->shape.c.d[3] <= 0) //Both boxes!
	{
		penvec = boxvbox(a->shape,b->shape);
	} else if (a->shape.c.d[3] > 0 && b->shape.c.d[3] <= 0) //a is a sphere, b is a box
	{
		penvec = spherevaabb(a->shape.c,b->shape);
	} else if (a->shape.c.d[3] <= 0 && b->shape.c.d[3] > 0){ //a is a box, b is a sphere
		penvec = spherevaabb(b->shape.c,a->shape);
		penvec.d[0] *= -1;
		penvec.d[1] *= -1;
		penvec.d[2] *= -1;
	} else {
#ifdef CHADPHYS_DEBUG
		puts("\nInvalid configuration. Error.\n");
#endif
	}
	if(penvec.d[3] <= 0) return; //No penetration detected, or invalid configuration.
	//We now have the penetration vector. There is a penetration.
	//determine how much each should be displaced by.
	//The penvec points INTO A and is of length penvec.d[3]
	float bdisplacefactor = a->mass / (a->mass + b->mass), adisplacefactor = b->mass / (a->mass + b->mass);
	vec3 comvel;
	if(!(a->mass > 0)) {
		adisplacefactor = 0; bdisplacefactor = 1;comvel = (vec3){{0,0,0}};
	}else if(!(b->mass > 0)) {
		bdisplacefactor = 0; adisplacefactor = 1;comvel = (vec3){{0,0,0}};
	}else{
		comvel = addv3( scalev3(bdisplacefactor, a->v), scalev3(adisplacefactor, b->v));
	}
	if(a->mass > 0){
		vec4 displacea = scalev4(-adisplacefactor, penvec);
		a->shape.c.d[0] += displacea.d[0];
		a->shape.c.d[1] += displacea.d[1];
		a->shape.c.d[2] += displacea.d[2];
		//a->v = addv3(scalev3(1.0-a->friction, arelvel),bvel); //Apply friction!
		a->v = addv3( scalev3(a->friction, comvel), scalev3(1.0-a->friction, a->v)  );
		a->v = addv3(a->v, scalev3( a->bounciness, downv4(displacea) ) );
	}
	if(b->mass > 0){
		vec4 displaceb = scalev4(bdisplacefactor, penvec);
		b->shape.c.d[0] += displaceb.d[0];
		b->shape.c.d[1] += displaceb.d[1];
		b->shape.c.d[2] += displaceb.d[2];
		//b->v = addv3(scalev3(1.0 - b->friction, brelvel),avel);
		b->v = addv3( scalev3(b->friction, comvel), scalev3(1.0-b->friction, b->v)  );
		b->v = addv3(b->v, scalev3( b->bounciness, downv4(displaceb) ) );
	}
}
#endif
