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
#include <SDL/SDL.h>


#ifndef M_PI
#define M_PI 3.14159265
#endif

int winSizeX = 640;
int winSizeY = 480;
int mousepos[2];
int mb = 0;
double tpassed = 0;
int isRunning = 1;

#define BEGIN_EVENT_HANDLER void events(SDL_Event* e){switch(e->type){
#define E_KEYSYM e->key.keysym.sym

#define END_EVENT_HANDLER }}
#define EVENT_HANDLER events
#define E_MOTION e->motion
#define E_BUTTON e->button.button
#define E_WINEVENT e->window.event
#define E_WINW e->window.data1
#define E_WINH e->window.data2

vec3 mouse_to_normal(){
	vec3 r;
	r.d[0] = mousepos[0] / (float) winSizeX;
	r.d[1] = mousepos[1] / (float) winSizeY;
	return r;
}

int drawBox(GLfloat x, GLfloat y, GLfloat xdim, GLfloat ydim){ //0,0 is top left, 1,1 is bottom right
	vec3 r = mouse_to_normal(); 
	int retval = 0;
	if(
		(x <= r.d[0]) &&
		(x+xdim >= r.d[0]) &&
		(y <= r.d[1]) &&
		(y+ydim >= r.d[1])
	) retval = 1;

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
	return retval;
}



void drawMouse(){
	vec3 r;
	r.d[0] = mousepos[0] / (float) winSizeX;
	r.d[1] = mousepos[1] / (float) winSizeY;
	if(!mb)
		glColor3f(0.7,0.7,0.7);
	else
		glColor3f(1.0,0.1,0.1);
	drawBox(r.d[0], r.d[1], 0.03, 0.03);
}

int drawTB(const char* text, GLuint textcolor, GLfloat x, GLfloat y, GLint size){
	size = (size>64)?64:((size<8)?8:size); 
	size >>= 3; //divide by 8 to get the GLTEXTSIZE
	if(!size || !text) return 0;
	int mw = 0, h = 1, cw = 0; //max width, height, current width
	for(int i = 0; text[i] != '\0' && (text[i] & 127);i++){
		if(text[i] != '\n') 
			cw++; 
		else 
			{cw = 0; h++;}
		if(mw<cw)mw=cw;
	}
	float w = (size)*8*(mw) / (float)winSizeX;
	float bw = 3*size/(float)winSizeX;
	float h_ = (size)*8*(h)/(float)winSizeY;
	float bh = 3*size/(float)winSizeY;
	int retval = drawBox(x-bw/2,y-bh/2, w+bw, h_+bh);
	glTextSize(size);
	glDrawText((unsigned char*)text, x*winSizeX, y*winSizeY, textcolor);
	return retval;
}

int haveclicked = 0;
vec3 tbcoords = (vec3){{0.4,0.4,0}};
void draw() {
	glColor3f(1,1,1);
	if(drawTB("I hate text", 0x00, tbcoords.d[0], tbcoords.d[1],16) && mb > 0)
		puts("Detected click!\n");
	else
		puts("No Click!\n");
	drawMouse();
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
	//glEnable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	// glEnable( GL_LIGHT0 );
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glShadeModel(GL_SMOOTH);
	glTextSize(GL_TEXT_SIZE24x24);
	glEnable(GL_NORMALIZE);
}





BEGIN_EVENT_HANDLER
	case SDL_KEYDOWN:
		switch(E_KEYSYM){
			case SDLK_ESCAPE:
			case SDLK_q:
				isRunning = 0;
			break;
			case SDLK_UP: mousepos[1] -= 4;  mousepos[1]%= winSizeY; break;
			case SDLK_DOWN: mousepos[1] += 4;mousepos[1]%= winSizeY; break;
			case SDLK_LEFT: mousepos[0] -= 4;mousepos[0]%= winSizeX; break;
			case SDLK_RIGHT: mousepos[0] += 4;mousepos[0]%= winSizeX; break;
			case SDLK_SPACE: case SDLK_RETURN:
				mb = 1;
			break;
			default: break;
		}
	break;
	case SDL_KEYUP:
		switch(E_KEYSYM){
			case SDLK_SPACE: case SDLK_RETURN:
				mb = 0;
			break;
			default: break;
		}
	break;
	case SDL_QUIT:isRunning = 0;break;
	case SDL_MOUSEBUTTONDOWN:
		if(E_BUTTON==SDL_BUTTON_LEFT) mb = 1;
		if(E_BUTTON==SDL_BUTTON_RIGHT) {
			tbcoords = mouse_to_normal();
		}
		break;
	case SDL_MOUSEBUTTONUP:
		if(E_BUTTON==SDL_BUTTON_LEFT) mb = 0;
		break;
	case SDL_MOUSEMOTION:
		mousepos[0] = E_MOTION.x;
		mousepos[1] = E_MOTION.y;
	break;
END_EVENT_HANDLER


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
	
	while (isRunning) {
		++frames;
		tpassed += frames * 16.666666/1000.0;
		tNow = SDL_GetTicks();
		// do event handling:
		SDL_Event ev;
		SDL_Event* e = &ev;
		while (SDL_PollEvent(e)) events(e);

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
			printf("%llu frames in %f secs, %f frames per second.\n", frames, (float)(tNow - tLastFps) * 0.001f,
				   (float)frames * 1000.0f / (float)(tNow - tLastFps));
			tLastFps = tNow;
			frames = 0;
		}
	}
	printf("%llu frames in %f secs, %f frames per second.\n", frames, (float)(tNow - tLastFps) * 0.001f, (float)frames * 1000.0f / (float)(tNow - tLastFps));
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
