include ../config.mk
#CC= gcc
#CFLAGS = -g -Wall -O3 -w
GL_LIBS= -L../ 
GL_INCLUDES= -I../include/
ALL_T= gears
LIB= ../lib/libTinyGL.a

#For GCC on good OSes:
#
#for MinGW users:
#SDL_LIBS= -lmingw32 -lSDLmain -lSDL 



all: $(ALL_T)
clean:
	rm -f $(ALL_T) *.exe
	rm -f render.png
gears:
	$(CC) gears.c $(LIB) -o gears $(GL_INCLUDES) $(GL_LIBS) $(CFLAGS) -lm
