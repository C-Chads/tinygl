
#include "3dMath.h"

#include "chadphys.h"
#include "../include/GL/gl.h"
#include "../include/zbuffer.h"
#include "api_audio.h"
#include "tobjparse.h"

static inline GLuint loadRGBTexture(unsigned char* buf, unsigned int w, unsigned int h) {
	GLuint t = 0;
	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, buf);
	return t;
}

static inline GLuint createModelDisplayList(
	vec3* points, GLuint npoints, vec3* colors, vec3* normals, vec3* texcoords) {
	GLuint i, ret = 0;
	if (!points)
		return 0;
	ret = glGenLists(1);
	glNewList(ret, GL_COMPILE);
	glBegin(GL_TRIANGLES);
	for (i = 0; i < npoints; i++) {
		if (colors) {
			glColor3f(colors[i].d[0], colors[i].d[1], colors[i].d[2]);
		}
		if (texcoords)
			glTexCoord2f(texcoords[i].d[0], texcoords[i].d[1]);
		if (normals)
			glNormal3f(normals[i].d[0], normals[i].d[1], normals[i].d[2]);
		glVertex3f(points[i].d[0], points[i].d[1], points[i].d[2]);
	}
	glEnd();
	glEndList();
	return ret;
}

typedef struct{
	GLuint dlframedata[256];
	GLuint tx; /*if this is zero, texture will be unbound.*/
} ChadAsset;

typedef struct {
	phys_body body; /*body*/
	void* pimpl; /*pointer to implementation*/
	GLuint asset; /*Rendering details*/
	unsigned char frame; /*the frame of animation to be used from the asset.*/
} ChadEntity;

typedef struct{
	phys_world world;
	ChadEntity* ents;
	ChadEntity* ents_phys; /*Where you place your objects.*/
	ChadAsset* assets;
	unsigned long n_ents; /*ents and ents_phys must contain the same number of elements.*/
	unsigned long n_assets;
} ChadWorld;

static inline void initChadWorld(ChadWorld* w){
	*w = (ChadWorld){0};
	w->world.ms = 9000;/*plenty fast*/
	w->world.g = (vec3){{0,-10,0}};/*plenty realistic*/
}

static inline void stepChadWorld(ChadWorld* world){
	stepPhysWorld(&world->world, 1);
}
/*Called at the beginning of every frame before the split.*/
static inline void syncChadWorld(ChadWorld* world){
	memcpy(world->ents, world->ents_phys, sizeof(ChadEntity) * world->n_ents);
}
/*invoked every single time that an object is added or removed*/
static inline void prepChadWorld(ChadWorld* world){
unsigned long i;
	if(world->world.bodies) free(world->world.bodies);
	world->world.bodies = calloc(1,sizeof(phys_body*) * world->n_ents);
	/*Bodies is an array of pointers.*/
	for(i = 0; i < world->n_ents; i++){
		world->world.bodies[i] = &world->ents_phys[i].body;
	}
	syncChadWorld(world);
}

static inline void ChadWorld_AddEntity(ChadWorld* world, ChadEntity ent){
	world->ents_phys = realloc(world->ents_phys, sizeof(ChadEntity) * (world->n_ents++));
	world->ents_phys[world->n_ents-1] = ent;
	prepChadWorld(world);
}

static inline void ChadWorld_RemoveEntity(ChadWorld* world, GLuint index){
	ChadEntity* old_ents_phys = world->ents_phys;
	if(world->n_ents <= index) return; /*Bad index*/
	world->ents_phys = calloc(1,(--world->n_ents) * sizeof(ChadEntity));
	for(unsigned long i = 0; i < world->n_ents+1; i++) /**/
	{
		
		if(i < index)
			world->ents_phys[i] = old_ents_phys[i];
		else if(i==index) continue; /*Skip.*/
		else if(i > index) /*Gets moved back a position.*/
			world->ents_phys[i-1] = old_ents_phys[i];
	}
	free(old_ents_phys);
	prepChadWorld(world);
}


static inline void renderChadWorld(ChadWorld* world){
	unsigned long i; 
	/*The user will already have set up the viewport, the camera, and the lights.*/
	for(i = 0; i < world->n_ents; i++){
		glPushMatrix();
			/*build the transformation matrix*/
			glTranslatef(world->ents[i].body.v.d[0],
						world->ents[i].body.v.d[1],
						world->ents[i].body.v.d[2]);
			glMultMatrixf(world->ents[i].body.localt.d);
			if(world->assets[world->ents[i].asset].tx){
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, world->assets[world->ents[i].asset].tx);
			} else {
				glDisable(GL_TEXTURE_2D);
			}
			/*Render that shizzle!*/
			glCallList(
				world->assets[world->ents[i].asset].dlframedata[world->ents[i].frame]
			);
		glPopMatrix();
	}
}

