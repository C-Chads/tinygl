CC= gcc
CFLAGS = -g -Wall -O3
GL_LIBS= -L../ 
GL_INCLUDES= -I../include/
ALL_T= gears texture model helloworld
LIB= ../lib/libTinyGL.a
all: $(ALL_T)
clean:
	rm -f $(ALL_T) *.exe
texture:
	gcc texture.c $(LIB) -o texture $(GL_INCLUDES) $(GL_LIBS) $(CFLAGS) -lSDL -lSDL_mixer -lmad -logg -lmikmod -logg -lm
helloworld:
	gcc helloworld.c $(LIB) -o helloworld $(GL_INCLUDES) $(GL_LIBS) $(CFLAGS) -lSDL -lSDL_mixer -lmad -logg -lmikmod -logg -lm
	
model:
	gcc model.c $(LIB) -o model $(GL_INCLUDES) $(GL_LIBS) $(CFLAGS) -lSDL -lSDL_mixer -lmad -logg -lmikmod -logg -lm
gears:
	gcc gears.c $(LIB) -o gears $(GL_INCLUDES) $(GL_LIBS) $(CFLAGS) -lSDL -lSDL_mixer -lmad -logg -lmikmod -logg -lm
