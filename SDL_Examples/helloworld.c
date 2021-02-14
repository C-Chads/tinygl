
/* sdlGears.c */
/*
 * 3-D gear wheels by Brian Paul. This program is in the public domain.
 *
 * ported to libSDL/TinyGL by Gerald Franz (gfz@o2online.de)
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/GL/gl.h"
#define CHAD_API_IMPL
#include "include/api_audio.h"
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
#include <SDL/SDL.h>
#include "../include/zbuffer.h"

#ifndef M_PI
#  define M_PI 3.14159265
#endif

GLuint tex = 0;
double time_passed = 0.0;


void draw() {
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D,tex);
	//time_passed += 0.0166666;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glRotatef(time_passed,0, 0, 1);
    glBegin( GL_TRIANGLES );
		glColor3f(0.2, 0.2, 1.0); //BLUE!
		//glColor3f(1.0, 0.2, 0.2); //RED!
		glVertex3f(-0.8,-0.8,0.2); 

		glColor3f(0.2, 1.0, 0.2); //GREEN!
		//glColor3f(1.0, 0.2, 0.2); //RED!
		glVertex3f(0.8,-0.8,0.2);

		glColor3f(1.0, 0.2, 0.2); //RED!
		glVertex3f(0,1.2,0.2);
    glEnd();
   	glPopMatrix();
}


void initScene() {

    //glLightfv( GL_LIGHT0, GL_AMBIENT, white);
    //glLightfv( GL_LIGHT0, GL_SPECULAR, white);
    glEnable( GL_CULL_FACE );
    //glDisable( GL_CULL_FACE );
    glEnable( GL_LIGHTING );
    glDisable( GL_TEXTURE_2D );
    glDisable( GL_LIGHTING );
    //glEnable( GL_LIGHT0 );
    glEnable( GL_DEPTH_TEST );
    glShadeModel( GL_SMOOTH );
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
    glEnable( GL_NORMALIZE );
}

int main(int argc, char **argv) {
    // initialize SDL video:
    int winSizeX=640;
    int winSizeY=480;
	unsigned int fps =0;
    if(argc > 2){
    	char* larg = argv[1];
    	for(int i = 0; i < argc; i++){
    		if(!strcmp(larg,"-w"))
				winSizeX = atoi(argv[i]);
    		if(!strcmp(larg,"-h"))
				winSizeY = atoi(argv[i]);
			if(!strcmp(larg,"-fps"))
				fps = strtoull(argv[i],0,10);
			larg = argv[i];
    	}
    }
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)<0) {
        fprintf(stderr,"ERROR: cannot initialize SDL video.\n");
        return 1;
    }
    ainit(0);
    SDL_Surface* screen = NULL;
    if((screen=SDL_SetVideoMode( winSizeX, winSizeY, TGL_FEATURE_RENDER_BITS, SDL_SWSURFACE)) == 0 ) {
        fprintf(stderr,"ERROR: Video mode set failed.\n");
        return 1;
    }
    printf("\nRMASK IS %u",screen->format->Rmask);
    printf("\nGMASK IS %u",screen->format->Gmask);
    printf("\nBMASK IS %u",screen->format->Bmask);
    printf("\nAMASK IS %u",screen->format->Amask);


    printf("\nRSHIFT IS %u",screen->format->Rshift);
    printf("\nGSHIFT IS %u",screen->format->Gshift);
    printf("\nBSHIFT IS %u",screen->format->Bshift);
    printf("\nASHIFT IS %u",screen->format->Ashift);
    fflush(stdout);
    track* myTrack = NULL;
    myTrack = lmus("WWGW.mp3");
    mplay(myTrack, -1, 1000);
    SDL_ShowCursor(SDL_DISABLE);
    SDL_WM_SetCaption(argv[0],0);

    // initialize TinyGL:
    unsigned int pitch;
    int	mode;
    switch( screen->format->BitsPerPixel ) {
    case  8:
        fprintf(stderr,"ERROR: Palettes are currently not supported.\n");
        fprintf(stderr,"\nUnsupported by maintainer!!!");
        return 1;
    case 16:
            pitch = screen->pitch;
            //fprintf(stderr,"\nUnsupported by maintainer!!!");
            mode = ZB_MODE_5R6G5B;
            //return 1;
            break;
    case 24:
        pitch = ( screen->pitch * 2 ) / 3;
        fprintf(stderr,"\nUnsupported by maintainer!!!");
        mode = ZB_MODE_RGB24;
        return 1;
        break;
    case 32:
        pitch = screen->pitch / 2;
        mode = ZB_MODE_RGBA;
        break;
    default:
        return 1;
        break;
    }
    ZBuffer *frameBuffer = ZB_open( winSizeX, winSizeY, mode, 0, 0, 0, 0);
    glInit( frameBuffer );

    // initialize GL:
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glViewport (0, 0, winSizeX, winSizeY);
    glEnable(GL_DEPTH_TEST);
    //GLfloat  h = (GLfloat) winSizeY / (GLfloat) winSizeX;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glFrustum( -1.0, 1.0, -h, h, 5.0, 60.0 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //glTranslatef( 0.0, 0.0, -45.0 );

    initScene();

    // variables for timing:
    unsigned int frames=0;
    unsigned int tNow=SDL_GetTicks();
    unsigned int tLastFps=tNow;

    // main loop:
    int isRunning=1;
    while(isRunning) {
        ++frames;
        tNow=SDL_GetTicks();
        time_passed += 0.01666666;
        // do event handling:
        SDL_Event evt;
        while( SDL_PollEvent( &evt ) ) switch(evt.type) {
        case SDL_KEYDOWN:
            switch(evt.key.keysym.sym) {
            case SDLK_ESCAPE :
            case SDLK_q :
                isRunning=0;
            default:
                break;
            }
            break;
        case SDL_QUIT:
            isRunning=0;
            break;
        }

        // draw scene:
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        draw();
		glDrawText((unsigned char*)"Hello World!\nFrom TinyGL", 0, 0, 0x00FFFFFF);
        // swap buffers:
        if ( SDL_MUSTLOCK(screen) && (SDL_LockSurface(screen)<0) ) {
            fprintf(stderr, "SDL ERROR: Can't lock screen: %s\n", SDL_GetError());
            return 1;
        }
        /*
		printf("\nRMASK IS %u",screen->format->Rmask);
		printf("\nGMASK IS %u",screen->format->Gmask);
		printf("\nBMASK IS %u",screen->format->Bmask);
		printf("\nAMASK IS %u",screen->format->Amask);
        */
        //Quickly convert all pixels to the correct format
#if TGL_FEATURE_RENDER_BITS == 32        
        for(int i = 0; i < frameBuffer->xsize* frameBuffer->ysize;i++){
#define DATONE (frameBuffer->pbuf[i])
			DATONE = ((DATONE & 0x000000FF)     ) << screen->format->Rshift | 
					 ((DATONE & 0x0000FF00) >> 8) << screen->format->Gshift |
					 ((DATONE & 0x00FF0000) >>16) << screen->format->Bshift;
        }
#endif
        ZB_copyFrameBuffer(frameBuffer, screen->pixels, screen->pitch);
        if ( SDL_MUSTLOCK(screen) ) SDL_UnlockSurface(screen);
        SDL_Flip(screen);
        if(fps>0)
			if((1000/fps)>(SDL_GetTicks()-tNow))
			{
				SDL_Delay((1000/fps)-(SDL_GetTicks()-tNow)); //Yay stable framerate!
			}
        // check for error conditions:
        char* sdl_error = SDL_GetError( );
        if( sdl_error[0] != '\0' ) {
            fprintf(stderr,"SDL ERROR: \"%s\"\n",sdl_error);
            SDL_ClearError();
        }
        // update fps:
        if(tNow>=tLastFps+5000) {
            printf("%i frames in %f secs, %f frames per second.\n",frames,(float)(tNow-tLastFps)*0.001f,(float)frames*1000.0f/(float)(tNow-tLastFps));
            tLastFps=tNow;
            frames=0;
        }
    }
    printf("%i frames in %f secs, %f frames per second.\n",frames,(float)(tNow-tLastFps)*0.001f,(float)frames*1000.0f/(float)(tNow-tLastFps));
    // cleanup:
    ZB_close(frameBuffer);
    if(SDL_WasInit(SDL_INIT_VIDEO))
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    mhalt();
    Mix_FreeMusic(myTrack);
    acleanup();
    SDL_Quit();
    return 0;
}
