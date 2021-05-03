/* Public Domain / CC0 Audio Playback Mini Library


Written by Gek (DMHSW) in 2020


*/

/*
HOW TO BUILD THINGS USING THIS LIBRARY
#define CHAD_API_IMPL
^ This line goes in the file you want the "implementation" in.
#include "api_audio.h"


*/
#define USE_MIXER

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>



#ifdef CHAD_API_IMPL
#define CHAD_MATH_IMPL
#endif
#include "3dMath.h"
typedef unsigned char uchar;

extern uint R_;
extern uint G_;
extern uint B_;
extern uint A_;


#ifdef USE_MIXER
#include<SDL/SDL_mixer.h>
void ainit(int needsSDLINIT);
void acleanup();
typedef Mix_Chunk samp;
typedef Mix_Music track;
samp* lwav(const char* t);
track* lmus(const char* t);
samp* qlwav(Uint8* w);
int aplay(samp* samp, int loops);
void aPos(int chan, int angle, unsigned char dist);
void aHalt(int chan);
int mplay(track* mus,int loops, int ms);
void mhalt();
#ifdef CHAD_API_IMPL
void ainit(int needsSDLINIT){
	if(needsSDLINIT)
		if (SDL_Init(SDL_INIT_AUDIO)!=0)  
			{fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());exit(0);}
	Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3);
	if(-1 == Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)) {printf("\nAudio can't init :(");exit(2);}
}
void acleanup(){
	Mix_CloseAudio();	
	Mix_Quit(); 
	SDL_Quit();
}


samp* lwav(const char* t){return Mix_LoadWAV(t);}
track* lmus(const char* t){return Mix_LoadMUS(t);}
samp* qlwav(Uint8* w){return Mix_QuickLoad_WAV(w);}
int aplay(samp* samp, int loops){return Mix_PlayChannel(-1, samp, loops);}
void aPos(int chan, int angle, unsigned char dist){Mix_SetPosition(chan,angle,dist);}
void aHalt(int chan){Mix_HaltChannel(chan);}
int mplay(track* mus,int loops, int ms){return Mix_FadeInMusic(mus,loops,ms);}
void mhalt(){Mix_HaltMusic();}

#endif

#endif




