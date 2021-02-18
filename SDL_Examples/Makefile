CC= gcc
CFLAGS = -g -Wall -O3 -w
GL_LIBS= -L../ 
GL_INCLUDES= -I../include/
ALL_T= gears texture model helloworld
LIB= ../lib/libTinyGL.a
SDL_LIBS= -lSDL 
#SDL_MIXERLIBS= -lSDL_mixer -lmad -logg -lmikmod -logg
SDL_MIXERLIBS= 
all: $(ALL_T)
clean:
	rm -f $(ALL_T) *.exe
	rm -f menu
texture:
	gcc texture.c $(LIB) -o texture $(GL_INCLUDES) $(SDL_LIBS) $(SDL_MIXERLIBS) $(GL_LIBS) $(CFLAGS) -lm
menu:
	gcc menu.c $(LIB) -o menu $(GL_INCLUDES) $(SDL_LIBS) $(SDL_MIXERLIBS) $(GL_LIBS) $(CFLAGS) -lm
helloworld:
	gcc helloworld.c $(LIB) -o helloworld $(GL_INCLUDES) $(GL_LIBS) $(CFLAGS) $(SDL_LIBS) $(SDL_MIXERLIBS) -lm
model:
	gcc model.c $(LIB) -o model $(GL_INCLUDES) $(GL_LIBS) $(CFLAGS) $(SDL_LIBS) $(SDL_MIXERLIBS) -lm
gears:
	gcc gears.c $(LIB) -o gears $(GL_INCLUDES) $(GL_LIBS) $(CFLAGS) $(SDL_LIBS) $(SDL_MIXERLIBS) -lm
