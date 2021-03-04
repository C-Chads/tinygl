include ../config.mk

OBJS= api.o list.o vertex.o init.o matrix.o texture.o \
      misc.o clear.o light.o clip.o select.o get.o \
      zbuffer.o zline.o ztriangle.o \
      zmath.o image_util.o msghandling.o \
      arrays.o specbuf.o memory.o ztext.o zraster.o accum.o


INCLUDES = -I./include
LIB = libTinyGL.a

all: $(LIB)
#	mv $(LIB) ../lib/

$(LIB): $(OBJS)
	rm -f $(LIB)
	ar rcs $(LIB) $(OBJS)
	
#SDL_Examples:
#	cd SDL_Examples && $(MAKE) && cd ..
clean:
	rm -f *~ *.o *.a
#	cd SDL_Examples && $(MAKE) clean && cd ..
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $*.c

clip.o: zgl.h ../include/zfeatures.h
vertex.o: zgl.h ../include/zfeatures.h
light.o: zgl.h ../include/zfeatures.h
matrix.o: zgl.h ../include/zfeatures.h
list.o: zgl.h opinfo.h ../include/zfeatures.h
arrays.c: zgl.h ../include/zfeatures.h
specbuf.o: zgl.h ../include/zfeatures.h
glx.o: zgl.h ../include/zfeatures.h
nglx.o: zgl.h ../include/zfeatures.h
zline.o: zgl.h ../include/zfeatures.h zline.h

ztriangle.o: ztriangle.c ztriangle.h zgl.h ../include/zfeatures.h
	$(CC) $(CFLAGS) -Wno-uninitialized $(INCLUDES) -c $*.c
