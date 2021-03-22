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
	phys_body* bodies;
	f_ ms; //max speed
	uint nbodies; //number of bodies
} phys_world;
typedef struct{
	phys_body** data;
	uint capacity;
} phys_cell;
typedef struct{
	phys_cell* data;
	f_ celldim;
	uint xcells;
	uint ycells;
	uint zcells;
	vec3 offset;
} phys_spatialhash;
static inline phys_spatialhash spatialhash_init(uint x, uint y, uint z, f_ celldim, vec3 offset){
	phys_spatialhash ret = {0};
	ret.xcells = x;
	ret.ycells = y;
	ret.zcells = z;
	ret.celldim = celldim;
	ret.offset = offset;
	ret.data = calloc(1,sizeof(phys_cell) * x * y * z);
	return ret;
}
static inline void destroy_spatialhash(phys_spatialhash* p){
	if(p->data) free(p->data);
	p->data = NULL;
}
static inline uint phys_cell_insert(phys_cell* c, phys_body* b){ //1 = error.
	uint inserted = 0;
	while(!inserted){
		for(uint b_ = 0; b_ < c->capacity; b_++)
			if(c->data[b_]==NULL)
			{
				c->data[b_] = b;
				inserted = 1;
				return 0;
			}
		phys_body** old = c->data;
		
		
		c->data = calloc(1,((c->capacity)<<1) * sizeof(phys_body*));
		if(c->data == NULL) { //The malloc failed! Wowza!
			if(old)//In case we had something there...
				free(old);
			return 1;
		}
		memcpy(c->data, old, c->capacity);
		c->capacity <<= 1;
		free(old);
	}
	return 1; //Unreachable.
}
//update the placement of this body b in the spatial hash.
static inline void spatialhash_clear(phys_spatialhash* h){
#pragma omp parallel for collapse(4)
	for(uint i = 0; i < h->xcells; i++)
	for(uint j = 0; j < h->ycells; j++)
	for(uint k = 0; k < h->zcells; k++)
	for(uint b_cell = 0; b_cell < h->data[i+ j*h->xcells + k*h->xcells*h->ycells].capacity; b_cell++)
			h->data[i+ j*h->xcells + k*h->xcells*h->ycells].data[b_cell] = NULL;
}


static inline uint spatialhash_update(phys_spatialhash* h, phys_body* b){
	//Assumes that the spatial hash was cleared this frame- no duplicates.
	vec3 b1c = downv4(b->shape.c);
	f_ rad = 0;
	if(b->shape.c.d[3] == 0){
		vec3 b1max = addv3(b1c,b->shape.e);
		rad = MAX(MAX(b1max.d[0],b1max.d[1]),b1max.d[2]);
	} else {
		rad = b->shape.c.d[3];
	}
	rad /= h->celldim;
	b1c = addv3(h->offset, b1c);
	b1c = scalev3(1.0/h->celldim, b1c);
	ivec3 beg, end;
#pragma omp simd
	for(int i = 0; i < 3; i++){
		beg.d[i] = b1c.d[i] - rad;
		end.d[i] = b1c.d[i] + rad;
	}
//error checking.
	for(int i = 0; i < 3; i++)
		if(beg.d[i] < 0 || end.d[i] < 0) return 1;
	if(	end.d[0] > h->xcells ||
		end.d[1] > h->ycells ||
		end.d[2] > h->zcells)
			return 1;
	if(	beg.d[0] > h->xcells ||
		beg.d[1] > h->ycells ||
		beg.d[2] > h->zcells)
				return 1;
#pragma omp parallel for collapse(3)
	for(uint i = beg.d[0]; i < h->xcells && i<=end.d[0]; i++)
	for(uint j = beg.d[1]; j < h->ycells && j<=end.d[1]; j++)
	for(uint k = beg.d[2]; k < h->zcells && k<=end.d[2]; k++){
		phys_cell_insert(h->data+i+ j*h->xcells + k*h->xcells*h->ycells, b);
	}
	return 0;
}



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
	} else {
#ifdef CHADPHYS_DEBUG
		puts("\nInvalid configuration. Error.\n");
#endif
	}
	if(penvec.d[3] <= 0) return; //No penetration detected, or invalid configuration.
	vec3 penvecnormalized = scalev3(1.0/penvec.d[3], downv4(penvec)); //the penetration vector points into B...
	float friction = a->friction * b->friction;
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
		b->shape.c.d[0] += displaceb.d[0];
		b->shape.c.d[1] += displaceb.d[1];
		b->shape.c.d[2] += displaceb.d[2];
		b->v = addv3(comvel, scalev3(1-friction, b_planarvel) ); //The center of mass velocity, plus a portion of coplanar velocity.
		b->v = addv3(b->v, scalev3( b->bounciness, downv4(displaceb) ) );
	}
}
#endif
