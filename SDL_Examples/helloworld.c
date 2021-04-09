//#define PLAY_MUSIC

#include "../include/GL/gl.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <GL/glu.h>
#include "../include/GL/gl.h"

#include "../include/zbuffer.h"
#define CHAD_API_IMPL
#define CHAD_MATH_IMPL
#include "../include-demo/3dMath.h"
#ifdef PLAY_MUSIC
#include "../include-demo/api_audio.h"
#else
typedef unsigned char uchar;
#endif
#include <SDL/SDL.h>
int noSDL = 0;
int do2 = 0;
#ifndef M_PI
#define M_PI 3.14159265
#endif

GLuint tex = 0;
double time_passed = 0.0;
int winSizeX = 640;
int winSizeY = 480;

void draw() {
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// glEnable(GL_TEXTURE_2D);
	// glBindTexture(GL_TEXTURE_2D,tex);
	// time_passed += 0.0166666;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glRotatef(time_passed, 0, 0, 1);
	glBegin(GL_TRIANGLES);
	glColor3f(0.2, 0.2, 1.0); // BLUE!
	// glColor3f(1.0, 0.2, 0.2); //RED!
	glVertex3f(-0.8, -0.8, 0.2);

	glColor3f(0.2, 1.0, 0.2); // GREEN!
	// glColor3f(1.0, 0.2, 0.2); //RED!
	glVertex3f(0.8, -0.8, 0.2);

	glColor3f(1.0, 0.2, 0.2); // RED!
	glVertex3f(0, 1.2, 0.2);
	glEnd();
	glPopMatrix();
}

void draw2() {
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int x = 0;
	// Select and setup the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	mat4 m = perspective(65.0f, (GLfloat)winSizeX / (GLfloat)winSizeY, 1.0f, 100.0f);
	glLoadMatrixf(m.d);
	// GLfloat  h = (GLfloat) winSizeY / (GLfloat) winSizeX;
	// glFrustum( -1.0, 1.0, -h, h, 5.0, 60.0 );
	// Select and setup the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(-90, 1, 0, 0);
	glTranslatef(0, 0, -1.0f);

	// Draw a rotating colorful triangle
	glTranslatef(0.0f, 14.0f, 0.0f);
	glRotatef(0.3f * (GLfloat)x + (GLfloat)time_passed * 100.0f, 0.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	glColor4f(1.0f, 0.01f, 0.01f, 0.0f);
	glVertex3f(-5.0f, 0.0f, -4.0f);
	glColor4f(0.01f, 1.0f, 0.01f, 0.0f);
	glVertex3f(5.0f, 0.0f, -4.0f);
	glColor4f(0.01f, 0.01f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 6.0f);
	glEnd();
}

void initScene() {

	// glLightfv( GL_LIGHT0, GL_AMBIENT, white);
	// glLightfv( GL_LIGHT0, GL_SPECULAR, white);
	// glEnable( GL_CULL_FACE );
	glDisable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	// glEnable( GL_LIGHT0 );
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glTextSize(GL_TEXT_SIZE24x24);
	/*
	{
		int sw = 0, sh = 0, sc = 0; //sc goes unused.
		uchar* source_data = stbi_load("texture.png", &sw, &sh, &sc, 3);
		if(source_data){
			tex = loadRGBTexture(source_data, sw, sh);
			free(source_data);
		} else {
			printf("\nCan't load texture!\n");
		}
	}*/
	glEnable(GL_NORMALIZE);
}

int main(int argc, char** argv) {
	// initialize SDL video:

	unsigned int fps = 0;
	char needsRGBAFix = 0;
	if (argc > 1) {
		char* larg = argv[1];
		for (int i = 0; i < argc; i++) {
			if (!strcmp(larg, "-w"))
				winSizeX = atoi(argv[i]);
			if (!strcmp(larg, "-h"))
				winSizeY = atoi(argv[i]);
			if (!strcmp(larg, "-fps"))
				fps = strtoull(argv[i], 0, 10);
			if (!strcmp(argv[i], "-nosdl"))
				noSDL = 1;
			if (!strcmp(argv[i], "-2"))
				do2 = 1;
			larg = argv[i];
		}
	}
	if (!noSDL) {
#ifdef PLAY_MUSIC
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
#else
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
#endif
			fprintf(stderr, "ERROR: cannot initialize SDL video.\n");
			return 1;
		}
	} else if (SDL_Init(0) < 0)
		fprintf(stderr, "ERROR: cannot initialize SDL without video.\n");
#ifdef PLAY_MUSIC
	if (!noSDL)
		ainit(0);
#endif
	SDL_Surface* screen = NULL;
	if (!noSDL)
		if ((screen = SDL_SetVideoMode(winSizeX, winSizeY, TGL_FEATURE_RENDER_BITS, SDL_SWSURFACE)) == 0) {
			fprintf(stderr, "ERROR: Video mode set failed.\n");
			return 1;
		}
	if (!noSDL) {
		printf("\nRMASK IS %u", screen->format->Rmask);
		printf("\nGMASK IS %u", screen->format->Gmask);
		printf("\nBMASK IS %u", screen->format->Bmask);
		printf("\nAMASK IS %u", screen->format->Amask);
	}
#if TGL_FEATURE_RENDER_BITS == 32
	if (!noSDL)
		if (screen->format->Rmask != 0x00FF0000 || screen->format->Gmask != 0x0000FF00 || screen->format->Bmask != 0x000000FF) {
			needsRGBAFix = 1;
			printf("\nYour screen is using an RGBA output different than this library expects.");
			printf("\nYou should consider using the 16 bit version for optimal performance");
		}
#endif
	if (!noSDL) {
		printf("\nRSHIFT IS %u", screen->format->Rshift);
		printf("\nGSHIFT IS %u", screen->format->Gshift);
		printf("\nBSHIFT IS %u", screen->format->Bshift);
		printf("\nASHIFT IS %u\n", screen->format->Ashift);
	}
	fflush(stdout);
#ifdef PLAY_MUSIC
	track* myTrack = NULL;
	if (!noSDL)
		myTrack = lmus("WWGW.mp3");
	if (!noSDL)
		mplay(myTrack, -1, 1000);
#endif
	if (!noSDL)
		SDL_ShowCursor(SDL_DISABLE);
	if (!noSDL)
		SDL_WM_SetCaption(argv[0], 0);

	// initialize TinyGL:
	// unsigned int pitch;
	int mode;
	if (!noSDL)
		switch (screen->format->BitsPerPixel) {
		case 8:
			fprintf(stderr, "ERROR: Palettes are currently not supported.\n");
			fprintf(stderr, "\nUnsupported by maintainer!!!");
			return 1;
		case 16:

			// fprintf(stderr,"\nUnsupported by maintainer!!!");
			mode = ZB_MODE_5R6G5B;
			// return 1;
			break;
		case 24:

			fprintf(stderr, "\nUnsupported by maintainer!!!");
			mode = ZB_MODE_RGB24;
			return 1;
			break;
		case 32:

			mode = ZB_MODE_RGBA;
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

	// initialize GL:
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glViewport(0, 0, winSizeX, winSizeY);
	glEnable(GL_DEPTH_TEST);
	// GLfloat  h = (GLfloat) winSizeY / (GLfloat) winSizeX;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// glFrustum( -1.0, 1.0, -h, h, 5.0, 60.0 );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// glTranslatef( 0.0, 0.0, -45.0 );

	initScene();

	// variables for timing:
	unsigned int frames = 0;
	unsigned int tNow = SDL_GetTicks();
	unsigned int tLastFps = tNow;

	// main loop:
	int isRunning = 1;
	while (isRunning) {
		++frames;
		tNow = SDL_GetTicks();
		time_passed += 0.01666666;
		// do event handling:
		SDL_Event evt;
		while (SDL_PollEvent(&evt))
			switch (evt.type) {
			case SDL_KEYDOWN:
				switch (evt.key.keysym.sym) {
				case SDLK_ESCAPE:
				case SDLK_q:
					isRunning = 0;
				default:
					break;
				}
				break;
			case SDL_QUIT:
				isRunning = 0;
				break;
			}

		// draw scene:
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (do2)
			draw2();
		else
			draw();
		glDrawText((unsigned char*)"Hello World!\nFrom TinyGL", 0, 0, 0x00FFFFFF);
		// swap buffers:
		if (!noSDL)
			if (SDL_MUSTLOCK(screen) && (SDL_LockSurface(screen) < 0)) {
				fprintf(stderr, "SDL ERROR: Can't lock screen: %s\n", SDL_GetError());
				return 1;
			}
			/*
			printf("\nRMASK IS %u",screen->format->Rmask);
			printf("\nGMASK IS %u",screen->format->Gmask);
			printf("\nBMASK IS %u",screen->format->Bmask);
			printf("\nAMASK IS %u",screen->format->Amask);
			*/
			// Quickly convert all pixels to the correct format
#if TGL_FEATURE_RENDER_BITS == 32
		if (needsRGBAFix)
			for (int i = 0; i < frameBuffer->xsize * frameBuffer->ysize; i++) {
#define DATONE (frameBuffer->pbuf[i])
				DATONE = ((DATONE & 0x000000FF)) << screen->format->Rshift | ((DATONE & 0x0000FF00) >> 8) << screen->format->Gshift |
						 ((DATONE & 0x00FF0000) >> 16) << screen->format->Bshift;
			}
#endif
		if (!noSDL)
			ZB_copyFrameBuffer(frameBuffer, screen->pixels, screen->pitch);
		if (!noSDL)
			if (SDL_MUSTLOCK(screen))
				SDL_UnlockSurface(screen);
		if (!noSDL)
			SDL_Flip(screen);
		if (!noSDL)
			if (fps > 0)
				if ((1000 / fps) > (SDL_GetTicks() - tNow)) {
					SDL_Delay((1000 / fps) - (SDL_GetTicks() - tNow)); // Yay stable framerate!
				}
		// check for error conditions:
		{
			char* sdl_error = SDL_GetError();
			if (sdl_error[0] != '\0') {
				fprintf(stderr, "SDL ERROR: \"%s\"\n", sdl_error);
				SDL_ClearError();
			}
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
	ZB_close(frameBuffer);
	glClose();
	if (!noSDL)
		if (SDL_WasInit(SDL_INIT_VIDEO))
			SDL_QuitSubSystem(SDL_INIT_VIDEO);
#ifdef PLAY_MUSIC
	if (!noSDL)
		mhalt();
	if (!noSDL)
		Mix_FreeMusic(myTrack);
	if (!noSDL)
		acleanup();
#endif
	SDL_Quit();
	return 0;
}
