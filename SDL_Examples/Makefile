include ../config.mk
#CC= gcc
#CFLAGS = -g -Wall -O3 -w
GL_LIBS= -L../ 
GL_INCLUDES= -I../include/
ALL_T= gears texture model helloworld menu game
LIB= ../lib/libTinyGL.a

#For GCC on good OSes:
SDL_CONFIG?=sdl-config
SDL_CFLAGS=$(shell $(SDL_CONFIG) --cflags)
SDL_LIBS=$(shell $(SDL_CONFIG) --libs)
#for MinGW users:
#SDL_LIBS= -lmingw32 -lSDLmain -lSDL 


#SDL_MIXERLIBS= -lSDL_mixer -lmad -logg -lmikmod -logg -DPLAY_MUSIC
SDL_MIXERLIBS= 
all: $(ALL_T)
clean:
	rm -f $(ALL_T) *.exe
texture:
	$(CC) texture.c $(LIB) -o texture $(SDL_CFLAGS) $(GL_INCLUDES) $(SDL_LIBS) $(SDL_MIXERLIBS) $(GL_LIBS) $(CFLAGS) -lm
menu:
	$(CC) menu.c $(LIB) -o menu $(SDL_CFLAGS) $(GL_INCLUDES) $(SDL_LIBS) $(SDL_MIXERLIBS) $(GL_LIBS) $(CFLAGS) -lm
helloworld:
	$(CC) helloworld.c $(LIB) -o helloworld $(SDL_CFLAGS) $(GL_INCLUDES) $(GL_LIBS) $(CFLAGS) $(SDL_LIBS) $(SDL_MIXERLIBS) -lm
model:
	$(CC) model.c $(LIB) -o model $(SDL_CFLAGS) $(GL_INCLUDES) $(GL_LIBS) $(CFLAGS) $(SDL_LIBS) $(SDL_MIXERLIBS) -lm
game:
	$(CC) game.c $(LIB) -o game $(SDL_CFLAGS) $(GL_INCLUDES) $(GL_LIBS) $(CFLAGS) $(SDL_LIBS) $(SDL_MIXERLIBS) -lm
gears:
	$(CC) gears.c $(LIB) -o gears $(SDL_CFLAGS) $(GL_INCLUDES) $(GL_LIBS) $(CFLAGS) $(SDL_LIBS) $(SDL_MIXERLIBS) -lm
