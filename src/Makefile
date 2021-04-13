include ../config.mk

OBJS= api.o list.o vertex.o init.o matrix.o texture.o \
      misc.o clear.o light.o clip.o select.o get.o \
      zbuffer.o zline.o ztriangle.o \
      zmath.o image_util.o msghandling.o \
      arrays.o specbuf.o memory.o ztext.o zraster.o accum.o zpostprocess.o


INCLUDES = -I./include
LIB = libTinyGL.a

all: $(LIB)

$(LIB): $(OBJS)
	rm -f $(LIB)
	ar rcs $(LIB) $(OBJS)
clean:
	rm -f *~ *.o *.a
.c.o:
	$(CC) $(CFLAGS_LIB) -c $*.c
