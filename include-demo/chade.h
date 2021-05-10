
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


static inline void drawModel(
	vec3* points, GLuint npoints, vec3* colors, vec3* normals, vec3* texcoords) {
	GLuint i;
	if (!points)
		return;
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
	return;
}

typedef struct {
	phys_body body; /*body*/
	GLuint dl; /*Rendering details*/
} ChadEntity;

typedef struct{
	phys_world world;
	ChadEntity** ents;
	long n_ents; /*ents and ents_phys must contain the same number of elements.*/
	long max_ents;
} ChadWorld;

static inline void initChadWorld(ChadWorld* w, long max_entities){
	*w = (ChadWorld){0};
	w->world.ms = 1000;/*plenty fast*/
	w->world.g = (vec3){{0,-1,0}};/*plenty realistic*/
	w->ents = calloc(1, sizeof(void*) * max_entities);
	if(w->ents == NULL)exit(1);
	w->max_ents = max_entities;
	w->n_ents = 0;
}

static inline void stepChadWorld(ChadWorld* world, long iter){
	stepPhysWorld(&world->world, iter);
}
static inline long ChadWorld_AddEntity(ChadWorld* world, ChadEntity* ent){
	/*safety check.
	for(long i = 0; i < world->max_ents; i++)
		if(world->ents[i] == ent) return i;
	*/		
	for(long i = 0; i < world->max_ents; i++){
		if(world->ents[i] == NULL){
			world->ents[i] = ent;
			world->world.bodies[i] = &ent->body;
			return i;
		}
	}
	return -1;

}

static inline void ChadWorld_RemoveEntity(ChadWorld* world, unsigned long index){
	if(index < (unsigned long)world->max_ents && index > 0)
		{world->ents[index] = NULL;world->world.bodies[index] = NULL;}
}


static inline void renderChadWorld(ChadWorld* world){
	long i; 
	/*The user will already have set up the viewport, the camera, and the lights.*/
	for(i = 0; i < world->n_ents; i++)
		if(world->ents[i]){
			glPushMatrix();
				/*build the transformation matrix*/
				glTranslatef(world->ents[i]->body.v.d[0],
							world->ents[i]->body.v.d[1],
							world->ents[i]->body.v.d[2]);
				glMultMatrixf(world->ents[i]->body.localt.d);
				/*Render that shizzle!*/
				glCallList(
					world->ents[i]->dl
				);
			glPopMatrix();
		}
}

