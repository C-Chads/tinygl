CC= gcc
CFLAGS = -g -Wall -O3
GL_LIBS= -L../ 
GL_INCLUDES= -I../include/
ALL_T= gears texture model
all: $(ALL_T)
clean:
	rm -f $(ALL_T)
texture:
	gcc texture.c ../libTinyGL.a -o texture $(GL_INCLUDES) $(GL_LIBS) $(CFLAGS) -lSDL -lSDL_mixer -lmad -logg -lmikmod -logg -lm
model:
	gcc model.c ../libTinyGL.a -o model $(GL_INCLUDES) $(GL_LIBS) $(CFLAGS) -lSDL -lSDL_mixer -lmad -logg -lmikmod -logg -lm
gears:
	gcc gears.c ../libTinyGL.a -o gears $(GL_INCLUDES) $(GL_LIBS) $(CFLAGS) -lSDL -lSDL_mixer -lmad -logg -lmikmod -logg -lm
