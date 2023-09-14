/* sdlGears.c */
/*
 * 3-D gear wheels by Brian Paul. This program is in the public domain.
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
#include "../include-demo/3dMath.h"
#ifdef PLAY_MUSIC
#include "../include-demo/api_audio.h"
#else
typedef unsigned char uchar;
#endif
#include <SDL/SDL.h>
int noSDL = 0;
#ifndef M_PI
#define M_PI 3.14159265
#endif
int override_drawmodes = 0;
int stipple = 0;
int lighting = 1;
GLubyte stipplepattern[128] = {0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
							   0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,

							   0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
							   0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,

							   0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
							   0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,

							   0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
							   0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55};

/*
 * Draw a gear wheel.  You'll probably want to call this function when
 * building a display list since we do a lot of trig here.
 *
 * Input:  inner_radius - radius of hole at center
 *         outer_radius - radius at center of teeth
 *         width - width of gear
 *         teeth - number of teeth
 *         tooth_depth - depth of tooth
 */
static void gear(GLfloat inner_radius, GLfloat outer_radius, GLfloat width, GLint teeth, GLfloat tooth_depth) {
	GLint i;
	GLfloat r0, r1, r2;
	GLfloat angle, da;
	GLfloat u, v, len;

	r0 = inner_radius;
	r1 = outer_radius - tooth_depth / 2.0;
	r2 = outer_radius + tooth_depth / 2.0;

	da = 2.0 * M_PI / teeth / 4.0;

	glNormal3f(0.0, 0.0, 1.0);

	/* draw front face */
	if (override_drawmodes == 1)
		glBegin(GL_LINES);
	else if (override_drawmodes == 2)
		glBegin(GL_POINTS);
	else {
		glBegin(GL_QUAD_STRIP);
	}
	for (i = 0; i <= teeth; i++) {
		angle = i * 2.0 * M_PI / teeth;
		glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
		glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
		glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
	}
	glEnd();

	/* draw front sides of teeth */
	if (override_drawmodes == 1)
		glBegin(GL_LINES);
	else if (override_drawmodes == 2)
		glBegin(GL_POINTS);
	else
		glBegin(GL_QUADS);
	da = 2.0 * M_PI / teeth / 4.0;
	for (i = 0; i < teeth; i++) {
		angle = i * 2.0 * M_PI / teeth;

		glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
		glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
		glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5);
		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
	}
	glEnd();

	glNormal3f(0.0, 0.0, -1.0);

	/* draw back face */
	if (override_drawmodes == 1)
		glBegin(GL_LINES);
	else if (override_drawmodes == 2)
		glBegin(GL_POINTS);
	else
		glBegin(GL_QUAD_STRIP);
	for (i = 0; i <= teeth; i++) {
		angle = i * 2.0 * M_PI / teeth;
		glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
		glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
		glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
	}
	glEnd();

	/* draw back sides of teeth */
	if (override_drawmodes == 1)
		glBegin(GL_LINES);
	else if (override_drawmodes == 2)
		glBegin(GL_POINTS);
	else
		glBegin(GL_QUADS);
	da = 2.0 * M_PI / teeth / 4.0;
	for (i = 0; i < teeth; i++) {
		angle = i * 2.0 * M_PI / teeth;

		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
		glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5);
		glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
		glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
	}
	glEnd();

	/* draw outward faces of teeth */
	if (override_drawmodes == 1)
		glBegin(GL_LINES);
	else if (override_drawmodes == 2)
		glBegin(GL_POINTS);
	else
		glBegin(GL_QUAD_STRIP);
	for (i = 0; i < teeth; i++) {
		angle = i * 2.0 * M_PI / teeth;

		glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
		glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
		u = r2 * cos(angle + da) - r1 * cos(angle);
		v = r2 * sin(angle + da) - r1 * sin(angle);
		len = sqrt(u * u + v * v);
		u /= len;
		v /= len;
		glNormal3f(v, -u, 0.0);
		glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
		glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
		glNormal3f(cos(angle), sin(angle), 0.0);
		glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5);
		glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5);
		u = r1 * cos(angle + 3 * da) - r2 * cos(angle + 2 * da);
		v = r1 * sin(angle + 3 * da) - r2 * sin(angle + 2 * da);
		glNormal3f(v, -u, 0.0);
		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
		glNormal3f(cos(angle), sin(angle), 0.0);
	}

	glVertex3f(r1 * cos(0), r1 * sin(0), width * 0.5);
	glVertex3f(r1 * cos(0), r1 * sin(0), -width * 0.5);

	glEnd();
	/* draw inside radius cylinder */
	if (override_drawmodes == 1)
		glBegin(GL_LINES);
	else if (override_drawmodes == 2)
		glBegin(GL_POINTS);
	else
		glBegin(GL_QUAD_STRIP);
	for (i = 0; i <= teeth; i++) {
		angle = i * 2.0 * M_PI / teeth;
		glNormal3f(-cos(angle), -sin(angle), 0.0);
		glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
		glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
	}
	glEnd();
}

static GLfloat view_rotx = 20.0, view_roty = 30.0;
static GLint gear1, gear2, gear3;
static GLfloat angle = 0.0;

void draw() {
	angle += 2.0;
	glPushMatrix();
	glRotatef(view_rotx, 1.0, 0.0, 0.0);
	glRotatef(view_roty, 0.0, 1.0, 0.0);
	// glRotatef( view_rotz, 0.0, 0.0, 1.0 );

	glPushMatrix();
	glTranslatef(-3.0, -2.0, 0.0);
	glRotatef(angle, 0.0, 0.0, 1.0);
	glCallList(gear1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(3.1, -2.0, 0.0);
	glRotatef(-2.0 * angle - 9.0, 0.0, 0.0, 1.0);
	glCallList(gear2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-3.1, 4.2, 0.0);
	glRotatef(-2.0 * angle - 25.0, 0.0, 0.0, 1.0);
	glCallList(gear3);
	glPopMatrix();

	glPopMatrix();
}

void initScene() {
	// static GLfloat pos[4] = {0.408248290463863, 0.408248290463863, 0.816496580927726, 0.0 }; //Light at infinity.
	static GLfloat pos[4] = {5, 5, 10, 0.0}; // Light at infinity.
	// static GLfloat pos[4] = {5, 5, -10, 0.0}; // Light at infinity.

	static GLfloat red[4] = {1.0, 0.0, 0.0, 0.0};
	static GLfloat green[4] = {0.0, 1.0, 0.0, 0.0};
	static GLfloat blue[4] = {0.0, 0.0, 1.0, 0.0};
	static GLfloat white[4] = {1.0, 1.0, 1.0, 0.0};
	static GLfloat shininess = 5;
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	// glLightfv( GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	glEnable(GL_CULL_FACE);

	if(lighting)glEnable(GL_LIGHT0);
	// glEnable(GL_DEPTH_TEST);

	if(stipple)	glEnable(GL_POLYGON_STIPPLE);
	else glDisable(GL_POLYGON_STIPPLE);
	glPolygonStipple(stipplepattern);
	glPointSize(10.0f);
	glTextSize(GL_TEXT_SIZE24x24);
	/* make the gears */
	gear1 = glGenLists(1);
	glNewList(gear1, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);
	glColor3fv(red);
	gear(1.0, 4.0, 1.0, 20, 0.7); // The largest gear.
	glEndList();

	gear2 = glGenLists(1);
	glNewList(gear2, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glColor3fv(green);
	gear(0.5, 2.0, 2.0, 10, 0.7); // The small gear with the smaller hole, to the right.
	glEndList();

	gear3 = glGenLists(1);
	glNewList(gear3, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glColor3fv(blue);
	gear(1.3, 2.0, 0.5, 10, 0.7); // The small gear above with the large hole.
	glEndList();
	// glEnable( GL_NORMALIZE );
}

int main(int argc, char** argv) {
	// initialize SDL video:
	int winSizeX = 640;
	int winSizeY = 480;
	unsigned int fps = 0;
	unsigned int flat = 1;
	unsigned int setenspec = 1;
	unsigned int dotext = 1;
	unsigned int blending = 0;
	char needsRGBAFix = 0;
	if (argc > 1) {
		char* larg = "";
		for (int i = 1; i < argc; i++) {
			if (!strcmp(larg, "-w"))
				winSizeX = atoi(argv[i]);
			if (!strcmp(larg, "-h"))
				winSizeY = atoi(argv[i]);
			if (!strcmp(larg, "-fps"))
				fps = strtoull(argv[i], 0, 10);
			if (!strcmp(argv[i], "-flat"))
				flat = 1;
			if(!strcmp(argv[i], "-nostipple"))
				stipple=0;
			if(!strcmp(argv[i], "-stipple"))
				stipple=1;
			if(!strcmp(argv[i], "-lighting"))
				lighting=1;
			if(!strcmp(argv[i], "-nolighting"))
				lighting=0;
			if (!strcmp(argv[i], "-smooth"))
				flat = 0;
			if (!strcmp(argv[i], "-blend"))
				blending = 1;
			if (!strcmp(argv[i], "-nospecular"))
				setenspec = 0;
			if (!strcmp(argv[i], "-lines"))
				override_drawmodes = 1;
			if (!strcmp(argv[i], "-points"))
				override_drawmodes = 2;
			if (!strcmp(argv[i], "-nosdl"))
				noSDL = 1;
			if (!strcmp(argv[i], "-notext"))
				dotext = 0;
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
	// int mode;
	if (!noSDL)
		switch (screen->format->BitsPerPixel) {
		case 8:
			fprintf(stderr, "ERROR: Palettes are currently not supported.\n");
			fprintf(stderr, "\nUnsupported by maintainer!!!");
			return 1;
		case 16:
			// pitch = screen->pitch;
			// fprintf(stderr,"\nUnsupported by maintainer!!!");
			// mode = ZB_MODE_5R6G5B;
			// return 1;
			break;
		case 24:
			// pitch = ( screen->pitch * 2 ) / 3;
			fprintf(stderr, "\nUnsupported by maintainer!!!");
			// mode = ZB_MODE_RGB24;
			return 1;
			break;
		case 32:
			// pitch = screen->pitch / 2;
			// mode = ZB_MODE_RGBA;
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
	if (!frameBuffer) {
		printf("\nZB_open failed!");
		exit(1);
	}
	glInit(frameBuffer);

	// Print version info
	printf("\nVersion string:\n%s", glGetString(GL_VERSION));
	printf("\nVendor string:\n%s", glGetString(GL_VENDOR));
	printf("\nRenderer string:\n%s", glGetString(GL_RENDERER));
	printf("\nExtensions string:\n%s", glGetString(GL_EXTENSIONS));
	printf("\nLicense string:\n%s", glGetString(GL_LICENSE));
	// initialize GL:
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glViewport(0, 0, winSizeX, winSizeY);
	if (flat)
		glShadeModel(GL_FLAT);
	else
		glShadeModel(GL_SMOOTH);
	// TESTING BLENDING...
	// glDisable(GL_DEPTH_TEST);

	// glDisable( GL_LIGHTING );
	if(lighting)
		glEnable(GL_LIGHTING);
	else
		glDisable( GL_LIGHTING );	
	// glBlendFunc(GL_ONE_MINUS_SRC_COLOR, GL_ZERO);
	glBlendEquation(GL_FUNC_ADD);
	if (blending) {
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
		glBlendEquation(GL_FUNC_ADD);
	} else {
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}
	GLfloat h = (GLfloat)winSizeY / (GLfloat)winSizeX;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -45.0);

	initScene();
	if (setenspec)
		glSetEnableSpecular(GL_TRUE);
	else
		glSetEnableSpecular(GL_FALSE);
	// variables for timing:
	unsigned int frames = 0;
	unsigned int tNow = SDL_GetTicks();
	unsigned int tLastFps = tNow;

	// main loop:
	int isRunning = 1;
	// float test = 0;
	while (isRunning) {
		++frames;
		// Depending on SDL to give us ticks even without a window open...
		tNow = SDL_GetTicks();
		//	test = TEST_fastInvSqrt(tNow);
		//	printf("\n%f",test);
		// do event handling:
		if (!noSDL) {
			SDL_Event evt;
			while (SDL_PollEvent(&evt))
				switch (evt.type) {
				case SDL_KEYDOWN:
					switch (evt.key.keysym.sym) {
					case SDLK_UP:
						view_rotx += 5.0;
						break;
					case SDLK_DOWN:
						view_rotx -= 5.0;
						break;
					case SDLK_LEFT:
						view_roty += 5.0;
						break;
					case SDLK_RIGHT:
						view_roty -= 5.0;
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
		}
		// draw scene:
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		draw();
		if (dotext) {
			glDrawText((unsigned char*)"RED text", 0, 0, 0xFF0000);

			glDrawText((unsigned char*)"GREEN text", 0, 24, 0x00FF00);

			glDrawText((unsigned char*)"\xa2"
									   "BLUE text"
									   "\xa1",
					   0, 48, 0xFF);
		}
		// swap buffers:
		if (!noSDL)
			if (SDL_MUSTLOCK(screen) && (SDL_LockSurface(screen) < 0)) {
				fprintf(stderr, "SDL ERROR: Can't lock screen: %s\n", SDL_GetError());
				return 1;
			}
			// Quickly convert all pixels to the correct format
#if TGL_FEATURE_RENDER_BITS == 32
		// for testing!
		if (needsRGBAFix)
			for (int i = 0; i < frameBuffer->xsize * frameBuffer->ysize; i++) {
#define DATONE (frameBuffer->pbuf[i])
				DATONE = ((DATONE & 0x00FF0000)) << screen->format->Rshift | ((DATONE & 0x0000FF00) >> 8) << screen->format->Gshift |
						 ((DATONE & 0x000000FF) >> 16) << screen->format->Bshift;
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
		if (!noSDL) {
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
	// glDeleteList(gear1);
	// glDeleteList(gear2);
	// glDeleteList(gear3);
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
