
//#define PLAY_MUSIC

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/GL/gl.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../include-demo/stb_image.h"
#define CHAD_MATH_IMPL
#include "../include-demo/3dMath.h"
#include "../include-demo/tobjparse.h"
#define CHAD_API_IMPL
#include "../include/zbuffer.h"
#ifdef PLAY_MUSIC
#include "../include-demo/api_audio.h"
#else
typedef unsigned char uchar;
#endif
#include <SDL/SDL.h>
#include <time.h>
int noSDL = 0;
int doblend = 0;
#ifndef M_PI
#define M_PI 3.14159265
#endif

vec3 campos = (vec3){.d[0] = 0, .d[1] = 0, .d[2] = -3};
vec3 camforw = (vec3){.d[0] = 0, .d[1] = 0, .d[2] = -1};
vec3 camup = (vec3){.d[0] = 0, .d[1] = 1, .d[2] = 0};
uint wasdstate[4] = {0, 0, 0, 0};
const float mouseratiox = 1.0 / 300.0f;
const float mouseratioy = 1.0 / 300.0f;
int mousex = 0, mousey = 0;

GLint playermodel;
/*Used for loading.*/
struct {
	float* points;
	uint npoints;
	float* normals;
	float* colors;
	float* texcoords;
} ModelArray;

void rotateCamera() {
	vec3 a;
	a.d[1] = (float)mousex * mouseratiox;
	a.d[2] = (float)mousey * mouseratioy;

	vec3 right = normalizev3(crossv3(camforw, camup));
	right.d[1] = 0;
	// vec3 forward = glm::vec3(glm::normalize(glm::rotate(angle, right) * glm::vec4(forward, 0.0)));

	camforw = normalizev3(rotatev3(camforw, right, -a.d[2]));
	// up = glm::normalize(glm::cross(forward, right));
	camup = normalizev3(crossv3(right, camforw));

	// Perform the rotation about the Y axis last.
	static const vec3 UP = (vec3){{0, 1, 0}};
	static const vec3 DOWN = (vec3){{0, -1, 0}};
	if (dotv3(UP, camup) < 0) {
		camforw = normalizev3(rotatev3(camforw, DOWN, -a.d[1]));
		camup = normalizev3(rotatev3(camup, DOWN, -a.d[1]));
	} else {
		camforw = normalizev3(rotatev3(camforw, UP, -a.d[1]));
		camup = normalizev3(rotatev3(camup, UP, -a.d[1]));
	}
}

GLuint loadRGBTexture(unsigned char* buf, unsigned int w, unsigned int h) {
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

GLuint createModelDisplayList(
	// HUGE important note! these depend on the math library using
	// f_ as float and not double!
	// Remember that!
	vec3* points, uint npoints, vec3* colors, vec3* normals, vec3* texcoords) {
	GLuint ret = 0;
	if (!points)
		return 0;
	ret = glGenLists(1);
	glNewList(ret, GL_COMPILE);
	glBegin(GL_TRIANGLES);
	for (uint i = 0; i < npoints; i++) {
		if (colors) {
			glColor3f(colors[i].d[0], colors[i].d[1], colors[i].d[2]);
		}
		if (texcoords)
			glTexCoord2f(texcoords[i].d[0], texcoords[i].d[1]);
		if (normals)
			glNormal3f(normals[i].d[0], normals[i].d[1], normals[i].d[2]);
		glVertex3f(points[i].d[0], points[i].d[1], points[i].d[2]);
	}
	// printf("\ncreateModelDisplayList is not the problem.\n");
	glEnd();
	glEndList();
	return ret;
}

GLubyte stipplepattern[128] = {0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
							   0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,

							   0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
							   0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,

							   0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
							   0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,

							   0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
							   0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55};

#define BEGIN_EVENT_HANDLER                                                                                                                                    \
	void events(SDL_Event* e) {                                                                                                                                \
		switch (e->type) {
#define E_KEYSYM e->key.keysym.sym
#define END_EVENT_HANDLER                                                                                                                                      \
	}                                                                                                                                                          \
	}
#define EVENT_HANDLER events
#define E_MOTION e->motion
#define E_BUTTON e->button.button
#define E_WINEVENT e->window.event
#define E_WINW e->window.data1
#define E_WINH e->window.data2

int isRunning = 1;

/*
mousex = (kHeld & KEY_Y)?-30:0 + (kHeld & KEY_A)?30:0;
		mousey = (kHeld & KEY_X)?-30:0 + (kHeld & KEY_B)?30:0;
*/
BEGIN_EVENT_HANDLER
case SDL_KEYDOWN:
switch (E_KEYSYM) {
case SDLK_w:
	wasdstate[0] = 1;
	break;
case SDLK_a:
	wasdstate[1] = 1;
	break;
case SDLK_s:
	wasdstate[2] = 1;
	break;
case SDLK_d:
	wasdstate[3] = 1;
	break;
case SDLK_UP:
	mousey = -30;
	break;
case SDLK_DOWN:
	mousey = 30;
	break;
case SDLK_LEFT:
	mousex = -30;
	break;
case SDLK_RIGHT:
	mousex = 30;
	break;
case SDLK_ESCAPE:
case SDLK_q:
	isRunning = 0;
	break;
default:
	break;
}
break;
case SDL_KEYUP:
switch (E_KEYSYM) {
case SDLK_w:
	wasdstate[0] = 0;
	break;
case SDLK_a:
	wasdstate[1] = 0;
	break;
case SDLK_s:
	wasdstate[2] = 0;
	break;
case SDLK_d:
	wasdstate[3] = 0;
	break;
case SDLK_UP:
case SDLK_DOWN:
	mousey = 0;
	break;
case SDLK_LEFT:
case SDLK_RIGHT:
	mousex = 0;
	break;
default:
	break;
}
break;
case SDL_QUIT:
isRunning = 0;
break;
END_EVENT_HANDLER

int main(int argc, char** argv) {
	// initialize SDL video:
	int winSizeX = 640;
	int winSizeY = 480;
	char needsRGBAFix = 0;
	unsigned int count = 40;
	GLuint modelDisplayList = 0;
	int dlExists = 0;
	int doTextures = 1;
#ifdef PLAY_MUSIC
	track* myTrack = NULL;
#endif
	unsigned int fps = 0;
	if (argc > 1) {
		char* larg = argv[0];
		for (int i = 1; i < argc; i++) {
			if (!strcmp(larg, "-w"))
				winSizeX = atoi(argv[i]);
			if (!strcmp(larg, "-h"))
				winSizeY = atoi(argv[i]);
			if (!strcmp(larg, "-fps"))
				fps = strtoull(argv[i], 0, 10);
			if (!strcmp(larg, "-count"))
				count = strtoull(argv[i], 0, 10);
			if (!strcmp(argv[i], "-notexture") || !strcmp(larg, "-notexture"))
				doTextures = 0;
			if (!strcmp(argv[i], "-blend"))
				doblend = 1;
			larg = argv[i];
		}
	}
#ifdef PLAY_MUSIC
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
#else
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
#endif
		fprintf(stderr, "ERROR: cannot initialize SDL video.\n");
		return 1;
	}
#ifdef PLAY_MUSIC
	ainit(0);
#endif
	SDL_Surface* screen = NULL;
	if ((screen = SDL_SetVideoMode(winSizeX, winSizeY, TGL_FEATURE_RENDER_BITS, SDL_SWSURFACE)) == 0) {
		fprintf(stderr, "ERROR: Video mode set failed.\n");
		return 1;
	}

#if TGL_FEATURE_RENDER_BITS == 32
	if (screen->format->Rmask != 0x00FF0000 || screen->format->Gmask != 0x0000FF00 || screen->format->Bmask != 0x000000FF) {
		needsRGBAFix = 1;
		printf("\nYour screen is using an RGBA output different than this library expects.");
		printf("\nYou should consider using the 16 bit version for optimal performance");
	}
#endif
	fflush(stdout);
#ifdef PLAY_MUSIC
	myTrack = lmus("WWGW.mp3");
	mplay(myTrack, -1, 1000);
#endif

	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption(argv[0], 0);

	// initialize TinyGL:
	switch (screen->format->BitsPerPixel) {
	case 8:
		fprintf(stderr, "ERROR: Palettes are currently not supported.\n");
		return 1;
	case 16:
		// fprintf(stderr,"\nUnsupported by maintainer!!!");
		// return 1;
		break;
	case 24:
		fprintf(stderr, "\nUnsupported by maintainer!!!");
		return 1;
		break;
	case 32:
		break;
	default:
		return 1;
		break;
	}

	ZBuffer* frameBuffer = NULL;
	if (TGL_FEATURE_RENDER_BITS == 32)
		frameBuffer = ZB_open(winSizeX, winSizeY, ZB_MODE_RGBA, 0);
	else
		frameBuffer = ZB_open(winSizeX, winSizeY, ZB_MODE_5R6G5B, 0);
	glInit(frameBuffer);

	srand(time(NULL));
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glSetEnableSpecular(0);
	static GLfloat white[4] = {1.0, 1.0, 1.0, 0.0};
	static GLfloat pos[4] = {5, 5, 10, 0.0}; // Light at infinity.

	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	// glLightfv( GL_LIGHT0, GL_AMBIENT, white);
	// glLightfv( GL_LIGHT0, GL_SPECULAR, white);
	glEnable(GL_CULL_FACE);

	// glDisable( GL_LIGHTING );
	glEnable(GL_LIGHT0);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_COLOR_MATERIAL);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0f);

	glClearColor(0, 0, 0.3, 0);
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	// glDisable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	// glDisable(GL_DEPTH_TEST);
	double t = 0;
	glViewport(0, 0, winSizeX, winSizeY);
	// glEnable(GL_POLYGON_STIPPLE);
	glPolygonStipple(stipplepattern);
	// initScene();
	{
		objraw omodel;
		model m = initmodel();
		omodel = tobj_load("monkey3.obj");

		if (!omodel.positions) {
			puts("\nERROR! No positions in model. Aborting...\n");
			abort();
		} else {
			m = tobj_tomodel(&omodel);
			printf("\nHas %ld points.\n", m.npoints);
			modelDisplayList = createModelDisplayList(m.d, m.npoints, m.c, m.n, m.t);
			freemodel(&m);
		}
		freeobjraw(&omodel);
	}

	GLuint tex = 0;
	if (doTextures) {
		int sw = 0, sh = 0, sc = 0; // sc goes unused.
#if TGL_FEATURE_NO_DRAW_COLOR == 1
		uchar* source_data = stbi_load("tex_hole.png", &sw, &sh, &sc, 3);
#else
		uchar* source_data = stbi_load("tex.jpg", &sw, &sh, &sc, 3);
#endif
		if (source_data) {
			tex = loadRGBTexture(source_data, sw, sh);
			free(source_data);
		}
		// tex =
	}
	// glDisable(GL_LIGHTING);
	// glEnable( GL_NORMALIZE );
	// variables for timing:
	unsigned int frames = 0;
	unsigned int tNow = SDL_GetTicks();
	unsigned int tLastFps = tNow;

	// main loop:

	while (isRunning) {
		++frames;
		t += 0.016666f;
		tNow = SDL_GetTicks();
		// do event handling:
		SDL_Event evt;
		mousex = 0;
		mousey = 0;
		while (SDL_PollEvent(&evt))
			EVENT_HANDLER(&evt);
			/*
					switch(evt.type) {


					}
			*/
			// draw scene:
#define WIDTH winSizeX
#define HEIGHT winSizeY
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		mat4 matrix = perspective(70, (float)WIDTH / (float)HEIGHT, 1, 512);
		glLoadMatrixf(matrix.d);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPushMatrix(); // Pushing on the LookAt Matrix.

		vec3 right = normalizev3(crossv3(camforw, camup));
		matrix = (lookAt(campos, addv3(campos, camforw), camup)); 
		glLoadMatrixf(matrix.d);
		if (wasdstate[0])
			campos = addv3(campos, scalev3(0.1, camforw));
		if (wasdstate[2])
			campos = addv3(campos, scalev3(-0.1, camforw));
		if (wasdstate[1])
			campos = addv3(campos, scalev3(-0.1, right));
		if (wasdstate[3])
			campos = addv3(campos, scalev3(0.1, right));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		if (doTextures) glBindTexture(GL_TEXTURE_2D, tex);

		{
			if (doTextures)
				glEnable(GL_TEXTURE_2D);
			// glDisable(GL_TEXTURE_2D);
			// glDisable(GL_COLOR_MATERIAL);
			for (unsigned int i = 0; i < count; i++) {
				glPushMatrix();
				mat4 horiz_translation = translate((vec3){{8.0 * (i % 10), 0.0, 0.0}});
				mat4 vert_translation = translate((vec3){{0.0, 8.0 * (i / 10), 0.0}});
				const mat4 ztranslation = translate((vec3){{0, 0, -10}});
				mat4 total_translation = multm4(multm4(horiz_translation, vert_translation), ztranslation);
				glMultMatrixf(total_translation.d);
				glCallList(modelDisplayList);
				glPopMatrix();
			}
			if (doTextures)
				glDisable(GL_TEXTURE_2D);
		}
		// draw();
		glPopMatrix(); // The view transform.

		rotateCamera();
		glTextSize(GL_TEXT_SIZE16x16);
		glDrawText((unsigned char*)"\nGame-\nTinyGL\nSDL 1.2\n", 0, 0, 0x000000FF);

		// swap buffers:
		if (SDL_MUSTLOCK(screen) && (SDL_LockSurface(screen) < 0)) {
			fprintf(stderr, "SDL ERROR: Can't lock screen: %s\n", SDL_GetError());
			return 1;
		}
		// Quickly convert all pixels to the correct format
#if TGL_FEATURE_RENDER_BITS == 32
		if (needsRGBAFix)
			for (int i = 0; i < frameBuffer->xsize * frameBuffer->ysize; i++) {
#define DATONE (frameBuffer->pbuf[i])
				DATONE = ((DATONE & 0x000000FF)) << screen->format->Rshift | ((DATONE & 0x0000FF00) >> 8) << screen->format->Gshift |
						 ((DATONE & 0x00FF0000) >> 16) << screen->format->Bshift;
			}
#endif
		ZB_copyFrameBuffer(frameBuffer, screen->pixels, screen->pitch);
		if (SDL_MUSTLOCK(screen))
			SDL_UnlockSurface(screen);
		SDL_Flip(screen);
		if (fps > 0)
			if ((1000 / fps) > (SDL_GetTicks() - tNow)) {
				SDL_Delay((1000 / fps) - (SDL_GetTicks() - tNow)); // Yay stable framerate!
			}
		// check for error conditions:
		const char* sdl_error = SDL_GetError();
		if (sdl_error[0] != '\0') {
			fprintf(stderr, "SDL ERROR: \"%s\"\n", sdl_error);
			SDL_ClearError();
		}
		// update fps:
		if (tNow >= tLastFps + 5000) {
			printf("%i frames in %f secs, %f frames per second.\n", frames, (float)(tNow - tLastFps) * 0.001f,
				   (float)frames * 1000.0f / (float)(tNow - tLastFps));
			tLastFps = tNow;
			frames = 0;
		}
	}
	printf("%i frames in %f secs, %f frames per second.\n", frames, (float)(tNow - tLastFps) * 0.001f, (float)frames * 1000.0f / (float)(tNow - tLastFps));
	// cleanup:
	glDeleteTextures(1, &tex);
	// glDeleteList(modelDisplayList);
	if (dlExists)
		glDeleteLists(modelDisplayList, 1);
	ZB_close(frameBuffer);
	glClose();
	if (SDL_WasInit(SDL_INIT_VIDEO))
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
#ifdef PLAY_MUSIC
	mhalt();
	Mix_FreeMusic(myTrack);
	acleanup();
#endif
	SDL_Quit();
	return 0;
}
