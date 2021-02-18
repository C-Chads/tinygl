/* sdlGears.c */
/*
 * 3-D gear wheels by Brian Paul. This program is in the public domain.
 *
 * ported to libSDL/TinyGL by Gerald Franz (gfz@o2online.de)
 */
//#define PLAY_MUSIC

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/GL/gl.h"

#include "../include/zbuffer.h"
#define CHAD_API_IMPL
#define CHAD_MATH_IMPL
#include "include/3dMath.h"
#ifdef PLAY_MUSIC
#include "include/api_audio.h"
#else
typedef unsigned char uchar;
#endif
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
#include <SDL/SDL.h>

#ifndef M_PI
#define M_PI 3.14159265
#endif

GLuint tex = 0;
int winSizeX = 640;
int winSizeY = 480;
double tpassed = 0;

GLuint loadRGBTexture(unsigned char* buf, unsigned int w, unsigned int h) {
	GLuint t = 0;
	glGenTextures(1, &t);
	// for(unsigned int i = 0; i < w * h; i++)
	// {
	// unsigned char t = 0;
	// unsigned char* r = buf + i*3;
	// // unsigned char* g = buf + i*3+1;
	// unsigned char* b = buf + i*3+2;
	// t = *r;
	// *r = *b;
	// *b = t;
	// }
	glBindTexture(GL_TEXTURE_2D, t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, buf);
	return t;
}

void drawBox(GLfloat x, GLfloat y, GLfloat xdim, GLfloat ydim){ //0,0 is top left, 1,1 is bottom right
	x*=2;xdim*=2;
	y*=2;ydim*=2;
	glBegin(GL_TRIANGLES);
	// TRIANGLE 1,
	glTexCoord2f(0, 0); 
	glVertex3f(-1+x, 1-y-ydim, 0.5); //Bottom Left Corner

	glTexCoord2f(1, -1);
	glVertex3f(-1+x+xdim, 1-y , 0.5); //Top Right Corner

	glTexCoord2f(0, -1);
	glVertex3f(-1+x, 1-y, 0.5); //Top Left
	// TRIANGLE 2
	glTexCoord2f(0, 0);
	glVertex3f(-1+x, 1-y-ydim, 0.5); //Bottom Left Corner

	glTexCoord2f(1, 0);
	glVertex3f(-1+x+xdim, 1-y-ydim, 0.5);

	glTexCoord2f(1, -1);
	glVertex3f(-1+x+xdim, 1-y , 0.5); //Top Right Corner
	glEnd();
}

void drawTB(const char* text, GLuint textcolor, GLfloat x, GLfloat y, GLint size){
	size = (size>64)?64:((size<8)?8:size); 
	size >>= 3; //divide by 8 to get the GLTEXTSIZE
	if(!size || !text) return;
	int mw = 0, h = 1, cw = 0; //max width, height, current width
	for(int i = 0; text[i] != '\0' && (text[i] & 127);i++){
		if(text[i] != '\n') 
			cw++; 
		else 
			{cw = 0; h++;}
		if(mw<cw)mw=cw;
	}
	float w = (size+1)*8*(mw) / (float)winSizeX;
	float h_ = (size+1)*8*(h)/(float)winSizeY;
	drawBox(x-w/8,y-h_/8, w, h_);
	glTextSize(size);
	glDrawText((unsigned char*)text, x*winSizeX, y*winSizeY, textcolor);
}

void draw() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_TRIANGLES);
	// TRIANGLE 1,
	glTexCoord2f(0, 0);
	glVertex3f(-1, -1, 0.5);

	glTexCoord2f(1, -1);
	glVertex3f(1, 1, 0.5);

	glTexCoord2f(0, -1);
	glVertex3f(-1, 1, 0.5);
	// TRIANGLE 2
	glTexCoord2f(0, 0);
	glVertex3f(-1, -1, 0.5);

	glTexCoord2f(1, 0);
	glVertex3f(1, -1, 0.5);

	glTexCoord2f(1, -1);
	glVertex3f(1, 1, 0.5);
	glEnd();
	glColor4f(1,1,1,1);
	//glDisable(GL_TEXTURE_2D);
	drawBox(0.8,0.8,0.2,0.2);
	glColor3f(1,1,1);
	glDisable(GL_TEXTURE_2D);
	drawTB("I hate text", 0x00, 0.4, 0.4 + 0.1 * sinf(tpassed),24);
}

void initScene() {
	static GLfloat pos[4] = {5.0, 5.0, 10.0, 0.0};

	static GLfloat white[4] = {1.0, 1.0, 1.0, 0.0};

	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	// glLightfv( GL_LIGHT0, GL_AMBIENT, white);
	// glLightfv( GL_LIGHT0, GL_SPECULAR, white);
	//glEnable(GL_CULL_FACE);
	glDisable( GL_CULL_FACE );
	glDisable(GL_BLEND);
	//glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	// glEnable( GL_LIGHT0 );
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glShadeModel(GL_SMOOTH);
	glTextSize(GL_TEXT_SIZE24x24);
	{
		int sw = 0, sh = 0, sc = 0; // sc goes unused.
		uchar* source_data = stbi_load("texture.png", &sw, &sh, &sc, 3);
		if (source_data) {
			tex = loadRGBTexture(source_data, sw, sh);
			free(source_data);
		} else {
			printf("\nCan't load texture!\n");
		}
	}
	glEnable(GL_NORMALIZE);
}

int main(int argc, char** argv) {
	// initialize SDL video:

	unsigned int fps = 0;
	char needsRGBAFix = 0;
	if (argc > 2) {
		char* larg = argv[1];
		for (int i = 0; i < argc; i++) {
			if (!strcmp(larg, "-w"))
				winSizeX = atoi(argv[i]);
			if (!strcmp(larg, "-h"))
				winSizeY = atoi(argv[i]);
			if (!strcmp(larg, "-fps"))
				fps = strtoull(argv[i], 0, 10);
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
	printf("\nRMASK IS %u", screen->format->Rmask);
	printf("\nGMASK IS %u", screen->format->Gmask);
	printf("\nBMASK IS %u", screen->format->Bmask);
	printf("\nAMASK IS %u", screen->format->Amask);
#if TGL_FEATURE_RENDER_BITS == 32
	if (screen->format->Rmask != 0x00FF0000 || screen->format->Gmask != 0x0000FF00 || screen->format->Bmask != 0x000000FF) {
		needsRGBAFix = 1;
		printf("\nYour screen is using an RGBA output different than this library expects.");
		printf("\nYou should consider using the 16 bit version for optimal performance");
	}
#endif

	printf("\nRSHIFT IS %u", screen->format->Rshift);
	printf("\nGSHIFT IS %u", screen->format->Gshift);
	printf("\nBSHIFT IS %u", screen->format->Bshift);
	printf("\nASHIFT IS %u\n", screen->format->Ashift);
	fflush(stdout);
	
#ifdef PLAY_MUSIC
	track* myTrack = NULL;
	myTrack = lmus("WWGW.mp3");
	mplay(myTrack, -1, 1000);
#endif
	SDL_ShowCursor(SDL_ENABLE);
	SDL_WM_SetCaption(argv[0], 0);

	// initialize TinyGL:

	int mode;
	switch (screen->format->BitsPerPixel) {
	case 16:

		// fprintf(stderr,"\nUnsupported by maintainer!!!");
		mode = ZB_MODE_5R6G5B;
		// return 1;
		break;
	case 32:

		mode = ZB_MODE_RGBA;
		break;
	default:
		return 1;
		break;
	}
	ZBuffer* frameBuffer = ZB_open(winSizeX, winSizeY, mode, 0);
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
	long long unsigned int frames = 0;
	unsigned int tNow = SDL_GetTicks();
	unsigned int tLastFps = tNow;

	// main loop:
	int isRunning = 1;
	while (isRunning) {
		++frames;
		tpassed += frames * 16.666666/1000.0;
		tNow = SDL_GetTicks();
		// do event handling:
		SDL_Event evt;
		while (SDL_PollEvent(&evt))
			switch (evt.type) {
			case SDL_KEYDOWN:
				switch (evt.key.keysym.sym) {
					break;
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
		draw();
		//glDrawText((unsigned char*)"\nBlitting text\nto the screen!", 0, 0, 0x000000FF);
		// swap buffers:
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
		ZB_copyFrameBuffer(frameBuffer, screen->pixels, screen->pitch);
		if (SDL_MUSTLOCK(screen))
			SDL_UnlockSurface(screen);
		SDL_Flip(screen);
		if (fps > 0)
			if ((1000 / fps) > (SDL_GetTicks() - tNow)) {
				SDL_Delay((1000 / fps) - (SDL_GetTicks() - tNow)); // Yay stable framerate!
			}
		// check for error conditions:
		char* sdl_error = SDL_GetError();
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
