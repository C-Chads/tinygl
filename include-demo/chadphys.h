#ifndef CHAD_PHYS_H
#define CHAD_PHYS_H
#include "3dMath.h"
typedef struct {
	aabb shape; //c.d[3] is sphere radius. 
				//if it's zero or less, it's not a sphere, it's a box
	mat4 localt; //Local Transform.
	vec3 v; //velocity
	vec3 a; //Body specific acceleration, combined with gravity
	f_ mass; //0 means kinematic, or static. Defaults to zero.
	f_ bounciness; //default 0, put portion of displacement into velocity.
	f_ airfriction; //default 1, multiplied by velocity every time timestep.
	f_ friction; //default 0.1
} phys_body;
typedef struct{
	vec3 g; //gravity
	phys_body** bodies;
	f_ ms; //max speed
	long nbodies; //number of bodies
	char is_2d; //is this a 2-dimensional simulation?
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
}
static inline mat4 getPhysBodyRenderTransform(phys_body* body){
	return multm4(
		translate(downv4(body->shape.c)),
		body->localt
	);
}

//Check for and, if necessary, resolve colliding bodies.
static inline void resolveBodies(phys_body* a, phys_body* b){
	vec4 penvec; vec3 penvecnormalized, comvel; f_ friction, bdisplacefactor, adisplacefactor;
	if(a->mass > 0 || b->mass > 0){ //Perform a preliminary check. Do we even have to do anything?
		/*We must do shit*/
	} else {return;}
	/*Optimized for branch prediction.*/
	penvec = (vec4){
		.d[0]=0,
		.d[1]=0,
		.d[2]=0,
		.d[3]=0
	};
	/*Check if the two bodies are colliding.*/
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
	if(penvec.d[3] <= 0.0){return;}//No penetration detected, or invalid configuration.
	penvecnormalized = scalev3(1.0/penvec.d[3], downv4(penvec)); //the penetration vector points into B...
	friction = a->friction * b->friction;
	//We now have the penetration vector. There is a penetration.
	//determine how much each should be displaced by.
	//The penvec points INTO A and is of length penvec.d[3]
	bdisplacefactor = a->mass / (a->mass + b->mass);
	adisplacefactor = b->mass / (a->mass + b->mass);
	if(!(a->mass > 0)) {
		adisplacefactor = 0; bdisplacefactor = 1;
	}else if(!(b->mass > 0)) {
		bdisplacefactor = 0; adisplacefactor = 1;
	}
	comvel = addv3( scalev3(bdisplacefactor, a->v), scalev3(adisplacefactor, b->v));
	if(a->mass > 0){
		vec4 displacea; vec3 a_relvel, a_planarvel; 
		displacea = scalev4(-adisplacefactor, penvec);
		a_relvel = subv3(a->v, comvel);
		a_planarvel = subv3(a_relvel,
								 scalev3(
								 	dotv3(a_relvel, penvecnormalized),
								 	penvecnormalized
								 )
								);
		a->shape.c.d[0] += displacea.d[0];
		a->shape.c.d[1] += displacea.d[1];
		a->shape.c.d[2] += displacea.d[2];
		a->v = addv3( comvel, scalev3(friction, a_planarvel) ); //The center of mass velocity, plus a portion of coplanar velocity.
		a->v = addv3(a->v, scalev3( a->bounciness, downv4(displacea) ) );
	}
	if(b->mass > 0){
		vec4 displaceb; vec3 b_relvel, b_planarvel;
		displaceb = scalev4(bdisplacefactor, penvec);
		b_relvel = subv3(b->v, comvel);
		b_planarvel = subv3(b_relvel,  //brelvel - portion of brelvel in the direction of penvecnormalized
									scalev3(
										dotv3(b_relvel, penvecnormalized), //the component in that direction
										penvecnormalized //that direction
									)
								);
		b->shape.c.d[0] += displaceb.d[0];
		b->shape.c.d[1] += displaceb.d[1];
		b->shape.c.d[2] += displaceb.d[2];
		b->v = addv3(comvel, scalev3(friction, b_planarvel) ); //The center of mass velocity, plus a portion of coplanar velocity.
		b->v = addv3(b->v, scalev3( b->bounciness, downv4(displaceb) ) );
	}
}

static inline void stepPhysWorld(phys_world* world, const long collisioniter){
	for(long i = 0; i < world->nbodies; i++)
		if(world->bodies[i] && world->bodies[i]->mass > 0){
			unsigned long n_scale_iter = 0;
			phys_body* body = world->bodies[i];
			vec3 bodypos = addv3(downv4(body->shape.c),body->v);
			body->shape.c.d[0] = bodypos.d[0];
			body->shape.c.d[1] = bodypos.d[1];
			body->shape.c.d[2] = bodypos.d[2];
			body->v = addv3(body->v, body->a);
			body->v = addv3(body->v, world->g);
			while(dotv3(body->v, body->v) > world->ms && n_scale_iter < 100) {
				body->v = scalev3(0.9, body->v); n_scale_iter++;
			}
			if(world->is_2d) {body->shape.c.d[2] = 0;}
			
		}
	
	//Resolve collisions (if any)
	for(long iter = 0; iter < collisioniter; iter++)
	for(long i = 0; i < (long)(world->nbodies-1); i++){
		if(world->bodies[i]){
			for(long j = i+1; j < (long)world->nbodies; j++)
				if(world->bodies[j])
					resolveBodies(world->bodies[i], world->bodies[j]);
		}
	}
}
#endif
