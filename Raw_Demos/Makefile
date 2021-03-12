include ../config.mk
#CC= gcc
#CFLAGS = -g -Wall -O3 -w
GL_LIBS= -L../ 
GL_INCLUDES= -I../include/
ALL_T= gears t2i bigfont
LIB= ../lib/libTinyGL.a

#For GCC on good OSes:
#
#for MinGW users:
#SDL_LIBS= -lmingw32 -lSDLmain -lSDL 



all: $(ALL_T)
clean:
	rm -f $(ALL_T) *.exe
	rm -f render.png
	rm -f t2i.png
gears:
	$(CC) gears.c $(LIB) -o gears $(GL_INCLUDES) $(GL_LIBS) $(CFLAGS) -lm
t2i:
	$(CC) t2i.c $(LIB) -o t2i $(GL_INCLUDES) $(GL_LIBS) $(CFLAGS) -lm
bigfont:
	$(CC) bigfont.c $(LIB) -o bigfont $(GL_INCLUDES) $(GL_LIBS) $(CFLAGS) -lm
