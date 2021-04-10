/* sdlGears.c */
/*
 * 3-D gear wheels by Brian Paul. This program is in the public domain.
 *
 * ported to libSDL/TinyGL by Gerald Franz (gfz@o2online.de)
 */

//#define PLAY_MUSIC

//Only C standard library includes.
//These are ALL the external dependencies of this program!!! ALL of them!!!
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


//Doesn't drag in anything.
#include "../include/GL/gl.h"

//Doesn't drag in anything.
#include "../include/zbuffer.h"
#define CHAD_MATH_IMPL

//Drags in Math and String (which are already dragged in above.)
#include "../include-demo/3dMath.h"

//Requires 
/*
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

and
#include <assert.h>
if I didn't define STBIW_ASSERT
*/
#define STBIW_ASSERT(x) /* a comment */
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include-demo/stb_image_write.h"
#include "../include-demo/stringutil.h"

typedef unsigned char uchar;
int tsize = 1;
unsigned int color = 0xFF0000;

#ifndef M_PI
#define M_PI 3.14159265
#endif

//PIXEL hugefb[1<<28]; //256 million pixels

int main(int argc, char** argv) {
	// initialize SDL video:
	unsigned int winSizeX = 640;
	unsigned int winSizeY = 480;
	PIXEL* imbuf = NULL;
	uchar* pbuf = NULL;
	if (argc > 1) {
		char* larg = "";
		for (int i = 1; i < argc; i++) {
			if (!strcmp(larg, "-w"))
				winSizeX = atoi(argv[i]);
			if (!strcmp(larg, "-h"))
				winSizeY = atoi(argv[i]);
			if (!strcmp(larg, "-ts"))
				tsize = atoi(argv[i]);
			if (!strcmp(larg, "-col"))
				color = strtoull(argv[i],0,16);
			larg = argv[i];
		}
	}
	unsigned long lout;
	char* text = read_until_terminator_alloced(stdin, &lout, '\0', 2);
	imbuf = calloc(1,sizeof(PIXEL) * winSizeX * winSizeY);
	ZBuffer* frameBuffer = NULL;
	if(TGL_FEATURE_RENDER_BITS == 32)
	 frameBuffer = ZB_open(winSizeX, winSizeY, ZB_MODE_RGBA, 0);
	else
	 frameBuffer = ZB_open(winSizeX, winSizeY, ZB_MODE_5R6G5B,0);
	if(!frameBuffer){printf("\nZB_open failed!");exit(1);}
	glInit(frameBuffer);

	// initialize GL:
//TESTING BLENDING...
	//glDisable(GL_DEPTH_TEST);

	
	// variables for timing:
	
	//unsigned int tLastFps = tNow;

	// main loop:
	//float test = 0;
	if(!text) return 1;
	{
	
		//Depending on SDL to give us ticks even without a window open...
	//	test = TEST_fastInvSqrt(tNow);
	//	printf("\n%f",test);
		// do event handling:
		
		// draw scene:
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glTextSize(tsize);
		glDrawText((unsigned char*)text, 0, 0, color);
		
		// swap buffers:
		// Quickly convert all pixels to the correct format
	
		 ZB_copyFrameBuffer(frameBuffer, imbuf, winSizeX * sizeof(PIXEL));
	}
	if(TGL_FEATURE_RENDER_BITS == 32){ //very little conversion.
		pbuf = malloc(3 * winSizeX * winSizeY);
		for(int i = 0; i < winSizeX * winSizeY; i++){
			//pbuf[3*i+0] = (imbuf[i]&0xff0000)>>16;
			//pbuf[3*i+1] = (imbuf[i]&0x00ff00)>>8;
			//pbuf[3*i+2] = (imbuf[i]&0x0000ff);
			pbuf[3*i+0] = GET_RED(imbuf[i]);
			pbuf[3*i+1] = GET_GREEN(imbuf[i]);
			pbuf[3*i+2] = GET_BLUE(imbuf[i]);
		}
		stbi_write_png("t2i.png", winSizeX, winSizeY, 3, pbuf, 0);
		free(imbuf);
		free(pbuf);
	} else if(TGL_FEATURE_RENDER_BITS == 16){
		//puts("\nTesting 16 bit rendering...\n");
		pbuf = malloc(3 * winSizeX * winSizeY);
		for(int i = 0; i < winSizeX * winSizeY; i++){
			pbuf[3*i+0] = GET_RED(imbuf[i]);
			pbuf[3*i+1] = GET_GREEN(imbuf[i]);
			pbuf[3*i+2] = GET_BLUE(imbuf[i]);
		}
		stbi_write_png("t2i.png", winSizeX, winSizeY, 3, pbuf, 0);
		free(imbuf);
		free(pbuf);
	}
	free(text);
	// cleanup:
	ZB_close(frameBuffer);
	glClose();
	return 0;
}
