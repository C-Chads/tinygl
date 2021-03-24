#ifndef CHAD_PHYS_H
#define CHAD_PHYS_H
#include "3dMath.h"
typedef struct {
	aabb shape; //c.d[3] is sphere radius. 
				//if it's zero or less, it's not a sphere, it's a box
	mat4 localt; //Local Transform.
	vec3 v; //velocity
	vec3 a; //Body specific acceleration, combined with gravity
	void* d; //User defined pointer.
	f_ mass; //0 means kinematic, or static. Defaults to zero.
	f_ bounciness; //default 0, put portion of displacement into velocity.
	f_ airfriction; //default 1, multiplied by velocity every time timestep.
	f_ friction; //default 0.1
} phys_body;
typedef struct{
	vec3 g; //gravity
	phys_body** bodies;
	f_ ms; //max speed
	int nbodies; //number of bodies
} phys_world;



static inline void initPhysBody(phys_body* body){
	body->shape = (aabb){
		.c=(vec4){.d[0] = 0,.d[1] = 0,.d[2] = 0,.d[3] = 0},
		.e=(vec3){.d[0] = 0,.d[1] = 0,.d[2] = 0}
	};
	body->mass = 0;
	body->bounciness = 0;
	body->friction = 0.99; //The amount of coplanar velocity preserved in collisions.
	body->airfriction = 1.0;
	body->a = (vec3){.d[0] = 0,.d[1] = 0,.d[2] = 0};
	body->localt = identitymat4();
	body->d = NULL;
}
static inline mat4 getPhysBodyRenderTransform(phys_body* body){
	return multm4(
		translate(downv4(body->shape.c)),
		body->localt
	);
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
	}
#ifdef CHADPHYS_DEBUG
	else {
		puts("\nInvalid configuration. Error.\n");
	}
#endif
	if(penvec.d[3] <= 0) return; //No penetration detected, or invalid configuration.
	vec3 penvecnormalized = scalev3(1.0/penvec.d[3], downv4(penvec)); //the penetration vector points into B...
	f_ friction = a->friction * b->friction;
	//We now have the penetration vector. There is a penetration.
	//determine how much each should be displaced by.
	//The penvec points INTO A and is of length penvec.d[3]
	f_ bdisplacefactor = a->mass / (a->mass + b->mass);
	f_ adisplacefactor = b->mass / (a->mass + b->mass);
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
		vec3 a_relvel = subv3(a->v, comvel);
		vec3 a_planarvel = subv3(a_relvel,
								 scalev3(
								 	dotv3(a_relvel, penvecnormalized),
								 	penvecnormalized
								 )
								);
		a->shape.c.d[0] += displacea.d[0];
		a->shape.c.d[1] += displacea.d[1];
		a->shape.c.d[2] += displacea.d[2];
		a->v = addv3( comvel, scalev3(1-friction, a_planarvel) ); //The center of mass velocity, plus a portion of coplanar velocity.
		a->v = addv3(a->v, scalev3( a->bounciness, downv4(displacea) ) );
	}
	if(b->mass > 0){
		vec4 displaceb = scalev4(bdisplacefactor, penvec);
		vec3 b_relvel = subv3(b->v, comvel);
		vec3 b_planarvel = subv3(b_relvel,  //brelvel - portion of brelvel in the direction of penvecnormalized
									scalev3(
										dotv3(b_relvel, penvecnormalized), //the component in that direction
										penvecnormalized //that direction
									)
								);
#pragma omp simd
		for(int i = 0; i < 3; i++)
			b->shape.c.d[i] += displaceb.d[i];
		b->v = addv3(comvel, scalev3(1-friction, b_planarvel) ); //The center of mass velocity, plus a portion of coplanar velocity.
		b->v = addv3(b->v, scalev3( b->bounciness, downv4(displaceb) ) );
	}
}

static inline void stepPhysWorld(phys_world* world, const int collisioniter){
	for(int i = 0; i < world->nbodies; i++)
		if(world->bodies[i]){
			phys_body* body = world->bodies[i];
			vec3 bodypos = addv3(downv4(body->shape.c),body->v);
			body->shape.c.d[0] = bodypos.d[0];
			body->shape.c.d[1] = bodypos.d[1];
			body->shape.c.d[2] = bodypos.d[2];
			body->v = addv3(body->v, body->a);
			body->v = addv3(body->v, world->g);
		}
	//Resolve collisions (if any)
	for(int iter = 0; iter < collisioniter; iter++)
	for(int i = 0; i < (int)(world->nbodies-1); i++)
	for(int j = i+1; j < (int)world->nbodies; j++)
		resolveBodies(world->bodies[i], world->bodies[j]);
}
#endif
